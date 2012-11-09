/******************************************************
 This is the main file used in the project for modelling
 an ARM platform similar to Freescale iMX53.

 This project uses ArchC to generate code to mimic ARM
 core behavior. This is a functional model of the
 platform, no cycle accuracy is intended. Currently
 off-core IPs modeled are:

 - TZIC, the interrupt control IP for the iMX53 SoC
 - General Purpose Timer
 - UART-1
 - Generic RAM/ROM devices
 - System control coprocessor - CP15
 - Memory Management Unit

 In this platform design, the ARM core runs one instruction, possibly
 interacting with external modules via the bus functional model.
 But in this instruction simulation cycle, the core may only change external
 modules hardware registers. It will not observe immediate response until
 the end of cycle. After finishing the execution of one instruction,
 SystemC kernel executes other modules threads and they
 must prepare information so the next instruction of the ARM core may
 observe external modules responses.

 Author : Rafael Auler, 10/10/2011

******************************************************/


const char *project_name="armv5e";
const char *project_file="armv5e.ac";
const char *archc_version="2.1";
const char *archc_options="-abi ";

#include <iostream>
#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "armv5e.H"
#include "gpt.h"
#include "tzic.h"
#include "ram.h"
#include "rom.h"
#include "uart.h"
#include "bus.h"
#include "coprocessor.h"
#include "cp15.h"
#include "mmu.h"

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

static unsigned CYCLES = 0;
static unsigned BATCH_SIZE = 100;
static unsigned GDB_PORT = 5000;
static bool ENABLE_GDB = false;
static char* SYSCODE = 0;
static char* BOOTCODE = 0;

coprocessor *CP[16];
MMU *mmu;

void process_params(int ac, char *av[]) {
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
        } else if (strncmp(cur, "-cycles=", 8) == 0) {
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
        } else if (strcmp(cur, "--help") == 0) {
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
            std::cout << "\t-debug-cp15" << std::endl;
            std::cout << "\t-debug-mmu"  << std::endl;
            std::cout << "\t--load-sys=<path>\t\tLoad system software." << std::endl;
            std::cout << "\t--load=<path\t\tLoad application software." << std::endl;
            std::cout << "\t-cycles=<num>\t\tRun for <num> platform cycles."
                      << std::endl;
            std::cout << "\t-batch-size=<num>\tRun <num>+1 processor cycles for each"
                "\n\t\t\t\tplatform cycle." << std::endl << std::endl;
        }
    }
}

int sc_main(int ac, char *av[])
{
    //!  ISA simulator
    // If CP pointers arent initialized as NULL,
    // we might get Segmentation Fault
    // when executing CP instructions.

        process_params(ac, av);
    for(int i = 0; i < 16; i++) CP[i] = NULL;

    //--- Devices -----
    armv5e armv5e_proc1 ("armv5e");                                                                          // Core
    tzic_module tzic    ("tzic",         (uint32_t) 0x0FFFC000, (uint32_t) 0x0FFFFFFF);                     // TZIC
    gpt_module  gpt     ("gpt",    tzic, (uint32_t) 0x53FA0000, (uint32_t) 0x53FA3FFF);                     // GPT1
    uart_module uart    ("uart",   tzic, (uint32_t) 0x53FBC000, (uint32_t) 0x53FBFFFF);                     // UART1
    rom_module  bootmem ("bootmem",tzic, BOOTCODE, (uint32_t) 0x0, (uint32_t)0xFFFFF);  // Boot Memory
    ram_module  mem     ("mem",    tzic, (uint32_t) 0x70000000, (uint32_t) 0x71000000, (uint32_t)0x1000000);// Internal RAM
    imx53_bus mainBus("imx53bus");

    //--- Connect devices to bus ----
    mainBus.connectDevice(&bootmem);
    mainBus.connectDevice(&tzic);
    mainBus.connectDevice(&gpt);
    mainBus.connectDevice(&uart);

    //--- Coprocessors ----
    cp15 *coprocessor15 = new cp15();
    CP[15] = coprocessor15;

    //---Memory Management Unit ----
    mmu = new MMU("MMU", *coprocessor15, mainBus);

#ifdef AC_DEBUG
    ac_trace("armv5e_proc1.trace");
#endif


    armv5e_proc1.set_instr_batch_size(BATCH_SIZE);

    tzic.proc_port(armv5e_proc1.inta);
    mainBus.proc_port(armv5e_proc1.inta);
    armv5e_proc1.MEM_port(*mmu);

    if (SYSCODE != 0) {
        std::cout << "Loading system kernel: " << SYSCODE << std::endl;
        armv5e_proc1.APP_MEM->load(SYSCODE);
    }
    if (ENABLE_GDB) {
        armv5e_proc1.enable_gdb(GDB_PORT);
    }
    armv5e_proc1.init(ac, av);
    cerr << endl;

    double duration = CYCLES;
    if (duration == 0)
        duration = -1.0;
    if (SYSCODE != 0) {
        armv5e_proc1.ac_start_addr = 0;
        //armv5e_proc1.ac_heap_ptr = 10485700;
        armv5e_proc1.dec_cache_size = armv5e_proc1.ac_heap_ptr;
    }

    sc_start(duration, SC_NS);

    armv5e_proc1.PrintStat();
    cerr << endl;

#ifdef AC_STATS
    ac_stats_base::print_all_stats(std::cerr);
#endif

#ifdef AC_DEBUG
    ac_close_trace();
#endif
    if (SYSCODE != 0)
        free(SYSCODE);

    return armv5e_proc1.ac_exit_status;
}
