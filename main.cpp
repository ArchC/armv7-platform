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
#include "uart.h"

// Debug switches - global variables defined by application parameters
bool DEBUG_BUS = false;
bool DEBUG_CORE = false;
bool DEBUG_GPT = false;
bool DEBUG_TZIC = false;
bool DEBUG_UART = false;
static unsigned CYCLES = 0;
static unsigned BATCH_SIZE = 100;
static unsigned GDB_PORT = 5000;
static bool ENABLE_GDB = false;
static char* SYSCODE = 0;

#include <stdarg.h>
static inline int dprintf(const char *format, ...) {
  int ret;
  if (DEBUG_BUS) {
    va_list args;
    va_start(args, format);
    ret = vfprintf(ac_err, format, args);
    va_end(args);
  }
  return ret;
}

// Custom functional model / fast simulation for imx53 bus.
// This class has instances for all the models plugged into the soc bus and
// callback the appropriate method for each module by checking the transaction
// address. It does the comparison via fast hardwired IFs tailored for this
// platform.
class imx53_bus : public sc_module, public ac_tlm_transport_if {

  tzic_module &tzic;
  gpt_module &gpt;
  uart_module &uart;
  
  unsigned *bootmem;
  unsigned *mem;

public:
  // signal data_abort/fetch_abort directly to the processor core
  sc_port<ac_tlm_transport_if> proc_port;

  // The constructor caller must provide all instances/callbacks used to access
  // the SoC modules.
  imx53_bus (sc_module_name name_, tzic_module &tzic_, gpt_module &gpt_, 
             uart_module &uart_) :
    sc_module(name_), tzic(tzic_), gpt(gpt_), uart(uart_) {
    bootmem = new unsigned[0x100000/4]; // 1MB from 0000 to fffff - real platform has only 128k
    mem =     new unsigned[0x1000000/4]; // 16MB of real memory (starting at 
                                         //  7000-0000h)
  }

  ~imx53_bus() {
    delete [] bootmem;
    delete [] mem;
  }

  // Main handling method
  ac_tlm_rsp transport(const ac_tlm_req& req) {
    ac_tlm_rsp ans;
    unsigned addr = req.addr;
    unsigned offset = (addr % 4) * 8;
    addr = (addr >> 2) << 2;
    ans.status = SUCCESS;
    if (offset) {
      dprintf(" ! Next bus access is misaligned. Byte offset: %d\n", offset/8);
    }
    if (req.type == READ) {
      // Priority comparisons - mem first
      if (addr <= 0x100000) {
        dprintf(" <--> BUS TRANSACTION: READ 0x%X (bootmem)\n", addr);
        ans.data = *(bootmem + addr/4);
        if (offset)
          ans.data = ans.data >> offset;
        return ans;
      }
      if (addr >= 0x70000000) {
        dprintf(" <--> BUS TRANSACTION: READ 0x%X (mem)\n", addr);
        ans.data = *(mem + (addr - 0x70000000)/4);
        if (offset)
          ans.data = ans.data >> offset;
        return ans;
      }
      // Other devices
      // TZIC
      if (addr >= 0x0FFFC000 && addr <= 0x0FFFFFFF) {
        dprintf(" <--> BUS TRANSACTION: READ 0x%X (TZIC)\n", addr);
        ans.data = tzic.fast_read(addr - 0xFFFC000);
        if (offset)
          ans.data = ans.data >> offset;
        return ans;
      }
      // GPT
      if (addr >= 0x53FA0000 && addr <= 0x53FA3FFF) {
        dprintf(" <--> BUS TRANSACTION: READ 0x%X (GPT)\n", addr);
        ans.data = gpt.fast_read(addr - 0x53FA0000);
        if (offset)
          ans.data = ans.data >> offset;
        return ans;
      }
      // UART
      if (addr >= 0x53FBC000 && addr <= 0x53FBFFFF) {
        dprintf(" <--> BUS TRANSACTION: READ 0x%X (UART)\n", addr);
        ans.data = uart.fast_read(addr - 0x53FBC000);
        if (offset)
          ans.data = ans.data >> offset;
        return ans;
      }
      dprintf(" <--> BUS TRANSACTION: READ 0x%X (*NOT MAPPED*)\n", addr);
      
    } else if (req.type == WRITE) {
      // Priority comparisons - mem first
      if (addr <= 0x100000) {
        dprintf(" <--> BUS TRANSACTION: WRITE 0x%X @0x%X (bootmem)\n",
                req.data, addr);
        if (offset) {
          unsigned old_data = *(bootmem + addr/4);
          unsigned data = req.data;
          old_data &= (0xFFFFFFFF << (32 - offset)) >> (32 - offset);
          old_data |= ((data << offset) >> offset) << offset;
          *(bootmem + addr/4) = old_data;
          return ans;
        }
        *(bootmem + addr/4) = req.data;
        return ans;
      }
      if (addr >= 0x70000000) {
        dprintf(" <--> BUS TRANSACTION: WRITE 0x%X @0x%X (mem)\n", req.data,
                addr);
        if (offset) {
          unsigned old_data = *(mem + addr/4);
          unsigned data = req.data;
          old_data &= (0xFFFFFFFF << (32 - offset)) >> (32 - offset);
          old_data |= ((data << offset) >> offset) << offset;
          *(mem + (addr - 0x70000000)/4) = old_data;
          return ans;
        }
        *(mem + (addr - 0x70000000)/4) = req.data;
        return ans;
      }
      // Other devices
      // TZIC
      if (addr >= 0x0FFFC000 && addr <= 0x0FFFFFFF) {
        dprintf(" <--> BUS TRANSACTION: WRITE 0x%X @0x%X (TZIC)\n", req.data,
                addr);
        tzic.fast_write(addr - 0xFFFC000, req.data);
        return ans;
      }
      // GPT
      if (addr >= 0x53FA0000 && addr <= 0x53FA3FFF) {
        dprintf(" <--> BUS TRANSACTION: WRITE 0x%X @0x%X (GPT)\n", req.data,
                addr);
        gpt.fast_write(addr - 0x53FA0000, req.data);
        return ans;
      }
      // UART
      if (addr >= 0x53FBC000 && addr <= 0x53FBFFFF) {
        dprintf(" <--> BUS TRANSACTION: WRITE 0x%X @0x%X (UART)\n", req.data,
                addr);
        uart.fast_write(addr - 0x53FBC000, req.data);
        return ans;
      }
      dprintf(" <--> BUS TRANSACTION: WRITE 0x%X @0x%X (*NOT MAPPED*)\n",
              req.data, addr);
    }

    // Fail - warn core about failure
    ac_tlm_req abrt_req;
    abrt_req.type = READ;
    abrt_req.dev_id = 0;
    abrt_req.addr = 0;
    abrt_req.data = arm_impl::EXCEPTION_DATA_ABORT;
    proc_port->transport(abrt_req);
    ans.status = ERROR;
    return ans;
    }
};

void process_params(int ac, char *av[]) {
  for (int i = 0, e = ac; i != e; ++i) {
    const char * cur = av[i];
    if (strcmp(cur, "-enable-gdb") == 0) {
      ENABLE_GDB = true;
    } else if (strcmp(cur, "-debug-core") == 0) {
      DEBUG_CORE = true;
    } else if (strcmp(cur, "-debug-bus") == 0) {
      DEBUG_BUS = true;
    } else if (strcmp(cur, "-debug-tzic") == 0) {
      DEBUG_TZIC = true;
    } else if (strcmp(cur, "-debug-gpt") == 0) {
      DEBUG_GPT = true;
    } else if (strcmp(cur, "-debug-uart") == 0) {
      DEBUG_UART = true;
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
      std::cout << "\t-debug-bus" << std::endl;
      std::cout << "\t-debug-tzic" << std::endl;
      std::cout << "\t-debug-gpt" << std::endl;
      std::cout << "\t-debug-uart" << std::endl;
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
  armv5e armv5e_proc1("armv5e");
  tzic_module tzic("tzic");
  gpt_module gpt("gpt", tzic);
  uart_module uart("uart", tzic);
  imx53_bus bus("imx53bus", tzic, gpt, uart);

#ifdef AC_DEBUG
  ac_trace("armv5e_proc1.trace");
#endif 
  process_params(ac, av);

  armv5e_proc1.set_instr_batch_size(BATCH_SIZE);

  tzic.proc_port(armv5e_proc1.inta);  
  bus.proc_port(armv5e_proc1.inta);
  armv5e_proc1.MEM_port(bus);

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
