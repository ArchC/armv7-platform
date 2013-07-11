// arm-sim - ArchC ARM plataform simulator.
//
// Copyright (C) 2013 The ArchC team.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------
//
// This is the main file used in the project for modeling
// an ARM platform similar to Freescale iMX53.
//
// This project uses ArchC to generate code to mimic ARM
// core behavior. This is a functional model of the
// platform, no cycle accuracy is intended.
//
// In this platform design, the ARM core runs one instruction, possibly
// interacting with external modules via the bus functional model. But
// in this instruction simulation cycle, the core may only change
// external modules hardware registers. It will not observe immediate
// response until the end of cycle. After finishing the execution of one
// instruction, SystemC kernel executes other modules threads and they
// must prepare information so the next instruction of the ARM core may
// observe external modules responses.
//
// Author : Rafael Auler,            10/10/2011
//          Gabriel Krisman Bertazi, 10/08/2012
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

// Constant definitions required by ArchC.
const char *project_name = "arm";
const char *project_file = "arm.ac";
const char *archc_version = "2.1";
const char *archc_options = "-abi ";
const char *model_version = "2.0";
// --

#include <iostream>
#include <argp.h>
#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"

#include "arm.H"
#include "gpt.h"
#include "tzic.h"
#include "ram.h"
#include "rom.h"
#include "uart.h"
#include "bus.h"
#include "coprocessor.h"
#include "cp15.h"
#include "mmu.h"
#include "sd.h"
#include "defines.H"
#include "esdhcv2.h"
#include "dpllc.h"
#include "src.h"
#include "ccm.h"
#include "pins.h"

#define iMX53_MODEL

// Debug switches - global variables defined by application parameters
bool DEBUG_BUS = false;
bool DEBUG_CORE = false;
bool DEBUG_GPT = false;
bool DEBUG_TZIC = false;
bool DEBUG_UART = false;
bool DEBUG_RAM = false;
bool DEBUG_CP15 = false;
bool DEBUG_MMU = false;
bool DEBUG_ROM = false;
bool DEBUG_SD = false;
bool DEBUG_ESDHCV2 = false;
bool DEBUG_DPLLC = false;
bool DEBUG_CCM = false;
bool DEBUG_FLOW = false;
bool DEBUG_SRC = false;

static unsigned CYCLES = 0;
static unsigned BATCH_SIZE = 100;
static unsigned GDB_PORT = 5000;
static bool ENABLE_GDB = false;
static char *SYSCODE = 0;
static char *BOOTCODE = 0;
static char *SDCARD = 0;

coprocessor *CP[16];
MMU *mmu;

//--
const char *argp_program_bug_address = "<krisman.gabriel@gmail.com>";

// Print model version information.
void
model_print_version (FILE * stream, struct argp_state *state)
{
  fprintf (stream, "ArchC ARM iMX.53 plataform model %s.\n", model_version);
  fprintf (stream, "Copyright (c) 2013 The ArchC team.\n");

  fprintf (stream, "This is free software; see the source for copying "
	   "conditions.\nThere is NO warranty; not even for MERCHANTABILITY"
	   "or FITNESS\nFOR A PARTICULAR PURPOSE.\n");

  fprintf (stream, "This simulator was generated using ArchC %s "
	   "and configured with %s\n", archc_version, archc_options);

  fprintf (stream, "Report bugs to %s\n", argp_program_bug_address);
}

static char doc[] = "ARMv7 iMX53_loco plataform ArchC simulator";
static char args_doc[] =
  "--bootrom=<boot_image> --sd=<sd_image> --debug-<device>";

bool
activate_debug_mode (const char *mode)
{
  if (strcmp (mode, "core") == 0)
    DEBUG_CORE = true;
  else if (strcmp (mode, "bus") == 0)
    DEBUG_BUS = true;
  else if (strcmp (mode, "tzic") == 0)
    DEBUG_TZIC = true;
  else if (strcmp (mode, "gpt") == 0)
    DEBUG_GPT = true;
  else if (strcmp (mode, "uart") == 0)
    DEBUG_UART = true;
  else if (strcmp (mode, "ram") == 0)
    DEBUG_RAM = true;
  else if (strcmp (mode, "rom") == 0)
    DEBUG_ROM = true;
  else if (strcmp (mode, "cp15") == 0)
    DEBUG_CP15 = true;
  else if (strcmp (mode, "mmu") == 0)
    DEBUG_MMU = true;
  else if (strcmp (mode, "esdhc") == 0)
    DEBUG_ESDHCV2 = true;
  else if (strcmp (mode, "sd") == 0)
    DEBUG_SD = true;
  else if (strcmp (mode, "dpllc") == 0)
    DEBUG_DPLLC = true;
  else if (strcmp (mode, "ccm") == 0)
    DEBUG_CCM = true;
  else if (strcmp (mode, "flow") == 0)
    DEBUG_FLOW = true;
  else
    return false;
  return true;
}

// Class of command line arguments we understand.
enum
{
  CMD_CLASS_DEBUG,

  CMD_CLASS_GDB,

  CMD_CLASS_CTL,

  CMD_CLASS_CODE,
};

// Command line options we can understand.
static argp_option arm_model_options[] = {
  {"cycles", 'c', "<cycles>", 0,
   "Run for <cycles> plataform cycles",
   CMD_CLASS_CTL},

  {"batch-cycles", 'r', "<cycles>", 0,
   "Run n+1 processor cycles for each plataform cycle",
   CMD_CLASS_CTL},

  {"debug", 'D',
   "[core,][bus,][gpt,][tzic,][uart,][ram,][rom,][cp15,]\n"
   "[mmu,][sd,][esdhc,][dpllc,][ccm,][src]", 0,
   "Activate device depuration mode",
   CMD_CLASS_DEBUG},

  {"trace-flow", 'f', 0, 0,
   "Activate flow debug mode",
   CMD_CLASS_DEBUG},

  {"enable-gdb", 'g', 0, 0,
   "Wait for GDB connection",
   CMD_CLASS_GDB},

  {"gdb-port", 'p', "<port>", 0,
   "Define port to expect a GDB connection",
   CMD_CLASS_GDB},

  {"bootrom", 'b', "<image>", 0,
   "Define bootstrapping code image",
   CMD_CLASS_CODE},

  {"sd", 's', "<image>", 0,
   "Load image to SD card device",
   CMD_CLASS_CODE},

  {"load-sys", 'y', "<file>", 0,
   "Load ELF image as system code",
   CMD_CLASS_CODE},

  {NULL}
};

// Command line arguments parser.
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
      // Activate debug modes
    case 'D':
      {
	char *s = arg;
	char *init = arg;

	while (s != NULL)
	  {
	    if (*s == ',' || *s == '\0')
	      {
		char c;

		// Junk at the end of the string, or comma as first char.
		if (*s == ',' && s[1] == '\0')
		  argp_error (state,
			      "Extra comma at the end of the argument for "
			      "'--debug'.");

		// Copy the account name.  Check to see if the user hasn't
		// provided an invalid account name as 'acc1,,acc2'.
		if (*init == ',' || *init == '\0')
		  argp_error (state, "Invalid null peripheral for debug.");

		c = *s;
		*s = '\0';
		if (activate_debug_mode (init) == false)
		  {
		    argp_error (state, "Invalid peripheral '%s'.", init);
		  }
		*s = c;
		init = s + 1;
	      }
	    if (*s == '\0')
	      break;

	    ++s;
	  }
	break;
      }
      break;

      // Alias to activate debug flow mode.
    case 'f':
      activate_debug_mode ("flow");
      break;

      // Enable GDB.
    case 'g':
      ENABLE_GDB = true;
      break;

      // Define port to wait GDB connection.
    case 'p':
      {
	int r = sscanf (arg, "%d", &GDB_PORT);
	if (r != 1)
	  argp_error (state, "Port defined is invalid");
      }
      break;

      // Define number of plataform simulation cycles
    case 'c':
      {
	int r = sscanf (arg, "%d", &CYCLES);
	if (r != 1)
	  argp_error (state, "Invalid number of cycles");
      }
      break;

      // Define number of plataform simulation cycles
    case 'r':
      {
	int r = sscanf (arg, "%d", &BATCH_SIZE);
	if (r != 1)
	  argp_error (state, "Invalid number of batch cycles");
      }
      break;

      // Inform bootstrapping code image path.
    case 'b':
      BOOTCODE = strdup (arg);
      break;

      // Inform SD CARD image path.
    case 's':
      SDCARD = strdup (arg);
      break;

      // Inform an ELF file path to be used as system code.
    case 'y':
      SYSCODE = strdup (arg);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { arm_model_options, parse_opt, 0, doc };

// Main function for ARM model simulator.
int
sc_main (int ac, char *av[])
{
  argp_program_version_hook = model_print_version;
  argp_parse (&argp, ac, av, 0, 0, 0);

  // Devices
  arm arm_proc1 ("arm");
  imx53_bus ip_bus ("ip_bus");

#ifdef iMX53_MODEL
  // Trust zone interrupt control.
  tzic_module tzic ("tzic");

  // General Purpose timer.
  gpt_module gpt ("gpt", tzic);

  // Universal asynchronous receiver/transmitter.
  uart_module uart ("uart", tzic);

  // Internal RAM.
  ram_module iram ("iram", tzic, 0x0001FFFF);

  // Bootstrap memory.
  rom_module bootmem ("bootmem", tzic, BOOTCODE);

  // DDR_1 RAM Memory.
  ram_module ddr1 ("ram_ddr_1", tzic, 0x3FFFFFFF);
  ddr1.populate
    ("/home/gabriel/unicamp/ic/arm/system_code/my_image/u-boot.bin",
     0x7800000);

  // DDR_1 RAM Memory.
  ram_module ddr2 ("ram_ddr_2", tzic, 0x3FFFFFFF);

  // Enhanced Secured Digital Host Controller 1.
  esdhc_module esdhc1 ("esdhcv2_1", tzic);

  // Digital Phase-Locked Loop Control 1.
  dpllc_module dpllc1 ("dpllc_1", tzic);

  // Digital Phase-Locked Loop Control 2.
  dpllc_module dpllc2 ("dpllc_2", tzic);

  // Digital Phase-Locked Loop Control 3.
  dpllc_module dpllc3 ("dpllc_3", tzic);

  // Digital Phase-Locked Loop Control 4.
  dpllc_module dpllc4 ("dpllc_4", tzic);

  // System Reset Control.
  src_module src ("SCR", tzic);

  // Clock Control Module.
  ccm_module ccm ("ccm", tzic);

  // Primary boot SD card.
  sd_card card ("microSD", SDCARD);
  esdhc1.connect_card (card);

  // Device's connection to the bus.
  // Peripheral Memory Map.
  ip_bus.connect_device (&bootmem, 0x00000000, 0x000FFFFF);
  ip_bus.connect_device (&tzic, 0x0FFFC000, 0x0FFFFFFF);
  ip_bus.connect_device (&esdhc1, 0x50004000, 0x50007FFF);
  ip_bus.connect_device (&gpt, 0x53FA0000, 0x53FA3FFF);
  ip_bus.connect_device (&uart, 0x53FBC000, 0x53FBFFFF);
  ip_bus.connect_device (&src, 0x53FD0000, 0x53FD3FFF);
  ip_bus.connect_device (&ccm, 0x53FD4000, 0x53FD7FFF);
  ip_bus.connect_device (&dpllc1, 0x63F80000, 0x63F83FFF);
  ip_bus.connect_device (&dpllc2, 0x63F84000, 0x63F87FFF);
  ip_bus.connect_device (&dpllc3, 0x63F88000, 0x63F8bFFF);
  ip_bus.connect_device (&dpllc4, 0x63F8C000, 0x63F8FFFF);
  ip_bus.connect_device (&ddr1, 0x70000000, 0xAFFFFFFF);
  ip_bus.connect_device (&ddr2, 0xB0000000, 0xEFFFFFFF);
  ip_bus.connect_device (&iram, 0xF8000000, 0xF801FFFF);

#else // iMX53_MODEL.

  // Main Memory.
  ram_module bootmem ("mainMem", tzic, 0x1000000);
  ip_bus.connect_device (&bootmem, 0x0, 0xFFFFF);

#endif // !iMX53_MODEL.

  // Coprocessors
  memset (CP, 0, (16 * sizeof (coprocessor *)));

  CP[15] = new cp15 ();

  // Memory Management Unit
  mmu = new MMU ("MMU", *((cp15 *) CP[15]), ip_bus);

#ifdef AC_DEBUG
  ac_trace ("arm_proc1.trace");
#endif

  arm_proc1.set_instr_batch_size (BATCH_SIZE);

  tzic.proc_port (arm_proc1.inta);
  ip_bus.proc_port (arm_proc1.inta);
  arm_proc1.MEM_port (*mmu);

#ifndef iMX53_MODEL
  if (SYSCODE != 0)
    {
      std::cout << "Loading system kernel: " << SYSCODE << std::endl;
      arm_proc1.APP_MEM->load (SYSCODE);
    }
#endif

  if (ENABLE_GDB)
    {
      arm_proc1.enable_gdb (GDB_PORT);
    }
  arm_proc1.init (ac, av);
  cerr << endl;

  double duration = CYCLES;
  if (duration == 0)
    duration = -1.0;

#ifdef iMX53_MODEL
//        arm_proc1.ac_start_addr = 0;
//        arm_proc1.ac_heap_ptr = 10485700;
//        arm_proc1.dec_cache_size = arm_proc1.ac_heap_ptr;
#else
  if (SYSCODE != 0)
    {
      arm_proc1.ac_start_addr = 0;
      // arm_proc1.ac_heap_ptr = 10485700;
      arm_proc1.dec_cache_size = arm_proc1.ac_heap_ptr;
    }
#endif
  sc_start (duration, SC_NS);

  arm_proc1.PrintStat ();
  cerr << endl;

#ifdef AC_STATS
  ac_stats_base::print_all_stats (std::cerr);
#endif

#ifdef AC_DEBUG
  ac_close_trace ();
#endif
  if (SYSCODE != 0)
    free (SYSCODE);

  return arm_proc1.ac_exit_status;
}
