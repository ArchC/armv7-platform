#include "arm_interrupts.h"
#include "armv5e_arch_ref.H"
#include <cassert>

// Exception vector addresses
static const unsigned int RESET_ADDR             = 0x00000000;
static const unsigned int RESET_ADDR_HI          = 0xffff0000;
static const unsigned int UNDEFINED_ADDR         = 0x00000004;
static const unsigned int UNDEFINED_ADDR_HI      = 0xffff0004;
static const unsigned int SWI_ADDR               = 0x00000008;
static const unsigned int SWI_ADDR_HI            = 0xffff0008;
static const unsigned int PREFETCH_ABORT_ADDR    = 0x0000000c;
static const unsigned int PREFETCH_ABORT_ADDR_HI = 0xffff000c;
static const unsigned int DATA_ABORT_ADDR        = 0x00000010;
static const unsigned int DATA_ABORT_ADDR_HI     = 0xffff0010;
static const unsigned int IRQ_ADDR               = 0x00000018;
static const unsigned int IRQ_ADDR_HI            = 0xffff0018;
static const unsigned int FIQ_ADDR               = 0x0000001c;
static const unsigned int FIQ_ADDR_HI            = 0xffff001c;

unsigned readCPSR();
void writeCPSR(unsigned);

void service_interrupt(armv5e_arch_ref& ref, unsigned excep_type) {
  unsigned cpsr = readCPSR();

  // FIQ disabled?
  if ((cpsr & (1 << 6)) && excep_type == arm_impl::EXCEPTION_FIQ)
    return;

  // IRQ disabled?
  if ((cpsr & (1 << 7)) && excep_type == arm_impl::EXCEPTION_IRQ)
    return;

  switch(excep_type) {
  case arm_impl::EXCEPTION_RESET:
    ref.R14_svc = 0;
    ref.SPSR_svc = 0;
    cpsr = cpsr & ~arm_impl::processor_mode::MODE_MASK;
    cpsr = cpsr | arm_impl::processor_mode::SUPERVISOR_MODE;
    cpsr = cpsr | (1 << 6); // disable FIQ
#ifdef HIGH_VECTOR
    ref.ac_pc = RESET_ADDR_HI;
#else
    ref.ac_pc = RESET_ADDR;
#endif
    break;
  case arm_impl::EXCEPTION_UNDEFINED_INSTR:
    ref.R14_und = ref.ac_pc; // address of the next instruction after the undef
                             // instruction. we expect this to run only after a
                             // cycle of archc behavioral simulation has been
                             // completed, because ac_pc is set with pc+4
                             // in the end of each cycle.
    ref.SPSR_und = cpsr;
    cpsr = cpsr & ~arm_impl::processor_mode::MODE_MASK;
    cpsr = cpsr | arm_impl::processor_mode::UNDEFINED_MODE;
#ifdef HIGH_VECTOR
    ref.ac_pc = UNDEFINED_ADDR_HI;
#else
    ref.ac_pc = UNDEFINED_ADDR;
#endif
    break;
  case arm_impl::EXCEPTION_SWI:
    ref.R14_svc = ref.ac_pc;  // remember ac_pc is pc+4 at each end of cycle
    ref.SPSR_svc = cpsr;
    cpsr = cpsr & ~arm_impl::processor_mode::MODE_MASK;
    cpsr = cpsr | arm_impl::processor_mode::SUPERVISOR_MODE;
#ifdef HIGH_VECTOR
    ref.ac_pc = SWI_ADDR_HI;
#else
    ref.ac_pc = SWI_ADDR;
#endif
    break;
  case arm_impl::EXCEPTION_PREFETCH_ABORT:
    ref.R14_abt = ref.ac_pc;  // remember ac_pc is pc+4 at each end of cycle
    ref.SPSR_abt = cpsr;
    cpsr = cpsr & ~arm_impl::processor_mode::MODE_MASK;
    cpsr = cpsr | arm_impl::processor_mode::ABORT_MODE;
#ifdef HIGH_VECTOR
    ref.ac_pc = PREFETCH_ABORT_ADDR_HI;
#else
    ref.ac_pc = PREFETCH_ABORT_ADDR;
#endif
    break;
  case arm_impl::EXCEPTION_DATA_ABORT:
    ref.R14_abt = ref.ac_pc + 4;  // remember ac_pc is pc+4 at each end of 
                                  // cycle data aborts sets R14_abt to pc+8
    ref.SPSR_abt = cpsr;
    cpsr = cpsr & ~arm_impl::processor_mode::MODE_MASK;
    cpsr = cpsr | arm_impl::processor_mode::ABORT_MODE;
#ifdef HIGH_VECTOR
    ref.ac_pc = DATA_ABORT_ADDR_HI;
#else
    ref.ac_pc = DATA_ABORT_ADDR;
#endif
    break;
  case arm_impl::EXCEPTION_IRQ:
    ref.R14_irq = ref.ac_pc + 4;  // remember ac_pc is pc+4 at each end of cycle
                                  // or the address of a branch target.
                                  // irq sets R14_irq to next instruction to be
                                  // executed +4
    ref.SPSR_irq = cpsr;
    cpsr = cpsr & ~arm_impl::processor_mode::MODE_MASK;
    cpsr = cpsr | arm_impl::processor_mode::IRQ_MODE;
#ifdef HIGH_VECTOR
    ref.ac_pc = IRQ_ADDR_HI;
#else
    ref.ac_pc = IRQ_ADDR;
#endif
    break;
  case arm_impl::EXCEPTION_FIQ:
    ref.R14_fiq = ref.ac_pc + 4;  // remember ac_pc is pc+4 at each end of cycle
                                  // or the address of a branch target.
                                  // irq sets R14_irq to next instruction to be
                                  // executed +4
    ref.SPSR_fiq = cpsr;
    cpsr = cpsr & ~arm_impl::processor_mode::MODE_MASK;
    cpsr = cpsr | arm_impl::processor_mode::FIQ_MODE;
    cpsr = cpsr | (1 << 6); // disable FIQ
#ifdef HIGH_VECTOR
    ref.ac_pc = FIQ_ADDR_HI;
#else
    ref.ac_pc = FIQ_ADDR;
#endif
    break;
  default:
    assert(0 && "Unknown interrupt type!");
    abort();
    break;
  }
  cpsr = cpsr & ~(1 << 5); // execute in ARM state
  cpsr = cpsr | (1 << 7); // disable normal interrupts

  writeCPSR(cpsr);
}

arm_impl::PrivilegeLevel arm_impl::processor_mode::getPrivilegeLevel()
{
    switch(this->mode) {
    case SYSTEM_MODE:
    case FIQ_MODE:
    case IRQ_MODE:
    case SUPERVISOR_MODE:
    case ABORT_MODE:
    case UNDEFINED_MODE:
        return PL1;
    }
    return PL0;
}

const char * arm_impl::processor_mode::currentMode_str() {
    switch (this->mode) {
    case SYSTEM_MODE:
        return "SYSTEM";
    case USER_MODE:
        return "USER";
    case FIQ_MODE:
        return "FIQ";
    case IRQ_MODE:
        return "IRQ";
    case SUPERVISOR_MODE:
        return "SUPERVISOR";
    case ABORT_MODE:
        return "ABORT";
    case UNDEFINED_MODE:
        return "UNDEFINED";
    }
    return 0;
}


