/******************************************************
 This is the main file used in the project for modelling
 an ARM platform similar to Freescale iMX53.

 This project uses ArchC to generate code to mimic ARM
 core behavior. This is a functional model of the
 platform, no cycle accuracy is intended.

 In this platform design, the ARM core runs one instruction, possibly
 interacting with external modules via the bus functional model.
 But in this instruction simulation cycle, the core may only change external
 modules hardware registers. It will not observe immediate response until
 the end of cycle. After finishing the execution of one instruction,
 SystemC kernel executes other modules threads and they
 must prepare information so the next instruction of the ARM core may
 observe external modules responses.

 Author : Rafael Auler,            10/10/2011
          Gabriel Krisman Bertazi, 10/08/2012
******************************************************/

const char *project_name="arm";
const char *project_file="arm.ac";
const char *archc_version="2.1";
const char *archc_options="-abi ";

#include <iostream>
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

// Debug switches - global variables defined by application parameters
bool DEBUG_BUS  = false;
bool DEBUG_CORE = false;
bool DEBUG_GPT  = false;
bool DEBUG_TZIC = false;
bool DEBUG_UART = false;
bool DEBUG_RAM  = false;
bool DEBUG_CP15 = false;
bool DEBUG_MMU  = false;
bool DEBUG_ROM  = false;
bool DEBUG_SD   = false;
bool DEBUG_ESDHCV2 = false;
bool DEBUG_DPLLC = false;
bool DEBUG_CCM   = false;
bool DEBUG_FLOW  = false;
bool DEBUG_SRC   = false;

static unsigned CYCLES = 0;
static unsigned BATCH_SIZE = 100;
static unsigned GDB_PORT = 5000;
static bool ENABLE_GDB = false;
static char* SYSCODE = 0;
static char* BOOTCODE = 0;
static char* SDCARD = 0;

coprocessor *CP[16];
MMU *mmu;

static void process_params(int ac, char *av[]) {
    for (int i = 0, e = ac; i != e; ++i) {
        const char * cur = av[i];
        if (strcmp(cur, "-enable-gdb") == 0) {
            ENABLE_GDB = true;
        } else if (strcmp(cur, "-debug-core")  == 0) {
            DEBUG_CORE = true;
        } else if (strcmp(cur, "-debug-bus")   == 0) {
            DEBUG_BUS = true;
        } else if (strcmp(cur, "-debug-tzic")  == 0) {
            DEBUG_TZIC = true;
        } else if (strcmp(cur, "-debug-gpt")   == 0) {
            DEBUG_GPT = true;
        } else if (strcmp(cur, "-debug-uart")  == 0) {
            DEBUG_UART = true;
        } else if (strcmp(cur, "-debug-ram")   == 0) {
            DEBUG_RAM = true;
        } else if (strcmp(cur, "-debug-rom")   == 0) {
            DEBUG_ROM = true;
        } else if (strcmp(cur, "-debug-cp15")  == 0) {
            DEBUG_CP15 = true;
        } else if (strcmp(cur, "-debug-mmu")   == 0) {
            DEBUG_MMU = true;
        }else if (strcmp(cur, "-debug-esdhc")   == 0) {
            DEBUG_ESDHCV2 = true;
        }else if (strcmp(cur, "-debug-sd")   == 0) {
            DEBUG_SD = true;
        }else if (strcmp(cur, "-debug-dpllc")   == 0) {
            DEBUG_DPLLC = true;
        }else if (strcmp(cur, "-debug-ccm")   == 0) {
            DEBUG_CCM = true;
        }else if (strcmp(cur, "-debug-flow")   == 0) {
            DEBUG_FLOW = true;
        }else if (strncmp(cur, "-cycles=", 8) == 0) {
            char buf[20];
            const char *src = cur + 8;
            strncpy(buf, src, 20);
            sscanf(buf, "%d", &CYCLES);
        } else if (strncmp(cur, "-batch-size=", 12) == 0) {
            char buf[20];
            const char *src = cur + 12;
            strncpy(buf, src, 20);
            sscanf(buf, "%d", &BATCH_SIZE);
        } else if (strncmp(cur, "-gdb-port=", 10) == 0) {
            char buf[20];
            int gdb_port;
            const char *src = cur + 10;
            strncpy(buf, src, 20);
            sscanf(buf, "%d", &GDB_PORT);
        } else if (strncmp(cur, "--load-sys=", 11) == 0) {
            SYSCODE = (char *) malloc(sizeof(char)*(strlen(cur+11)+1));
            strcpy(SYSCODE, cur+11);
        }else if (strncmp(cur, "--boot-rom=", 11) == 0) {
            BOOTCODE = (char *) malloc(sizeof(char)*(strlen(cur+11)+1));
            strcpy(BOOTCODE, cur+11);
        }else if (strncmp(cur, "--sd=", 5) == 0) {
            SDCARD = (char *) malloc(sizeof(char)*(strlen(cur+11)+1));
            strcpy(SDCARD, cur+5);
        }  else if (strcmp(cur, "--help") == 0) {
            std::cout << std::endl;
            std::cout << "ARM i.MX53 platform simulator." << std::endl;
            std::cout << "Written by Rafael Auler, University of Campinas."
                      << std::endl;
            std::cout << "Version 07 Jun 2012" << std::endl;
            std::cout << "Report bugs to rafael.auler@lsc.ic.unicamp.br" << std::endl;
            std::cout << "Platform options:" << std::endl;
            std::cout << "\t-enable-gdb" << std::endl;
            std::cout << "\t-debug-core" << std::endl;
            std::cout << "\t-debug-bus"  << std::endl;
            std::cout << "\t-debug-tzic" << std::endl;
            std::cout << "\t-debug-gpt"  << std::endl;
            std::cout << "\t-debug-uart" << std::endl;
            std::cout << "\t-debug-ram"  << std::endl;
            std::cout << "\t-debug-rom"  << std::endl;
            std::cout << "\t-debug-sd"  << std::endl;
            std::cout << "\t-debug-cp15" << std::endl;
            std::cout << "\t-debug-mmu"  << std::endl;
            std::cout << "\t-debug-esdhc"  << std::endl;
            std::cout << "\t-debug-dpllc"  << std::endl;
            std::cout << "\t-debug-ccm"  << std::endl;
            std::cout << "\t-debug-flow"  << std::endl;
            std::cout << "\t--boot-rom=<path>\t\tLoad Bootstrap ROM code." << std::endl;
            std::cout << "\t--sd=<path>\t\tLoad SD card image to SD1 slot." << std::endl;
            std::cout << "\t--load-sys=<path>\t\tLoad system software." << std::endl;
            std::cout << "\t--load=<path\t\tLoad application software." << std::endl;
            std::cout << "\t-cycles=<num>\t\tRun for <num> platform cycles."
                      << std::endl;
            std::cout << "\t-batch-size=<num>\tRun <num>+1 processor cycles for each"
                "\n\t\t\t\tplatform cycle." << std::endl << std::endl;
        }
    }
}

#define iMX53_MODEL

int sc_main(int ac, char *av[])
{
    //!  ISA simulator
    process_params(ac, av);

    //--- Devices -----
    arm arm_proc1 ("arm");    // Core
    imx53_bus ip_bus  ("ip_bus");  // Core Bus

#ifdef iMX53_MODEL
    /* Trust zone interrupt control.  */
    tzic_module tzic ("tzic");

    /* General Purpose timer.  */
    gpt_module  gpt ("gpt", tzic);

    /* Universal asynchronous receiver/transmitter.  */
    uart_module uart ("uart", tzic);

    /* Internal RAM.  */
    ram_module  iram ("iram", tzic, 0x0001FFFF);

    /* Bootstrap memory.  */
    rom_module bootmem ("bootmem", tzic, BOOTCODE);

    /* DDR_1 RAM Memory.  */
    ram_module ddr1 ("ram_ddr_1", tzic, 0x3FFFFFFF);
    ddr1.populate("/home/gabriel/unicamp/ic/arm/system_code/my_image/u-boot_DEBUG.bin", 0x7800000);

    /* DDR_1 RAM Memory.  */
    ram_module ddr2 ("ram_ddr_2", tzic, 0x3FFFFFFF);

    /* Enhanced Secured Digital Host Controller 1.  */
    ESDHCV2_module esdhc1 ("esdhcv2_1",tzic);

    /* Digital Phase-Locked Loop Control 1.  */
    dpllc_module dpllc1 ("dpllc_1", tzic);

    /* Digital Phase-Locked Loop Control 2.  */
    dpllc_module dpllc2 ("dpllc_2", tzic);

    /* Digital Phase-Locked Loop Control 3.  */
    dpllc_module dpllc3 ("dpllc_3", tzic);

    /* Digital Phase-Locked Loop Control 4.  */
    dpllc_module dpllc4 ("dpllc_4", tzic);

    /* System Reset Control.  */
    src_module src ("SCR", tzic);

    /* Clock Control Module.  */
    ccm_module ccm ("ccm", tzic);

    /* Primary boot SD card.  */
    sd_card    card    ("microSD", SDCARD);
    esdhc1.connect_card(card);

    /* Device's connection to the bus.  */
    /* Peripheral Memory Map.  */
    ip_bus.connect_device (&bootmem, 0x00000000, 0x000FFFFF);
    ip_bus.connect_device (&tzic,    0x0FFFC000, 0x0FFFFFFF);
    ip_bus.connect_device (&esdhc1,  0x50004000, 0x50007FFF);
    ip_bus.connect_device (&gpt,     0x53FA0000, 0x53FA3FFF);
    ip_bus.connect_device (&uart,    0x53FBC000, 0x53FBFFFF);
    ip_bus.connect_device (&src,     0x53FD0000, 0x53FD3FFF);
    ip_bus.connect_device (&ccm,     0x53FD4000, 0x53FD7FFF);
    ip_bus.connect_device (&dpllc1,  0x63F80000, 0x63F83FFF);
    ip_bus.connect_device (&dpllc2,  0x63F84000, 0x63F87FFF);
    ip_bus.connect_device (&dpllc3,  0x63F88000, 0x63F8bFFF);
    ip_bus.connect_device (&dpllc4,  0x63F8C000, 0x63F8FFFF);
    ip_bus.connect_device (&ddr1,    0x70000000, 0xAFFFFFFF);
    ip_bus.connect_device (&ddr2,    0xB0000000, 0xEFFFFFFF);
    ip_bus.connect_device (&iram,    0xF8000000, 0xF801FFFF);

#else /* iMX53_MODEL.  */

    /* Main Memory.  */
    ram_module  bootmem ("mainMem",tzic, 0x1000000);
    ip_bus.connect_device(&bootmem, 0x0, 0xFFFFF);

#endif /* !iMX53_MODEL.  */

    //--- Coprocessors ----
    memset(CP, 0, (16 * sizeof(coprocessor *)));

    CP[15] = new cp15();

    //---Memory Management Unit ----
    mmu = new MMU("MMU", *((cp15*)CP[15]), ip_bus);

#ifdef AC_DEBUG
    ac_trace("arm_proc1.trace");
#endif

    arm_proc1.set_instr_batch_size(BATCH_SIZE);

    tzic.proc_port(arm_proc1.inta);
    ip_bus.proc_port(arm_proc1.inta);
    arm_proc1.MEM_port(*mmu);

#ifndef iMX53_MODEL
    if (SYSCODE != 0) {
        std::cout << "Loading system kernel: " << SYSCODE << std::endl;
        arm_proc1.APP_MEM->load(SYSCODE);
    }
#endif

    if (ENABLE_GDB) {
        arm_proc1.enable_gdb(GDB_PORT);
    }
    arm_proc1.init(ac, av);
    cerr << endl;

    double duration = CYCLES;
    if (duration == 0)
        duration = -1.0;

#ifdef iMX53_MODEL
//        arm_proc1.ac_start_addr = 0;
//        arm_proc1.ac_heap_ptr = 10485700;
//        arm_proc1.dec_cache_size = arm_proc1.ac_heap_ptr;
#else
    if (SYSCODE != 0) {
        arm_proc1.ac_start_addr = 0;
        // arm_proc1.ac_heap_ptr = 10485700;
        arm_proc1.dec_cache_size = arm_proc1.ac_heap_ptr;
    }
#endif
    sc_start(duration, SC_NS);

    arm_proc1.PrintStat();
    cerr << endl;

#ifdef AC_STATS
    ac_stats_base::print_all_stats(std::cerr);
#endif

#ifdef AC_DEBUG
    ac_close_trace();
#endif
    if (SYSCODE != 0)
        free(SYSCODE);

    return arm_proc1.ac_exit_status;
}
