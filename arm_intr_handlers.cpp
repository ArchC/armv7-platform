#include "arm_interrupts.h"

#include "ac_intr_handler.H"
#include "arm_intr_handlers.H"
#include "arm_ih_bhv_macros.H"

//!'using namespace' statement to allow access to all arm-specific datatypes
using namespace arm_parms;

// Whoever calls this interrupt, it must enforce correct exception priority,
// since we can't enforce this here. We simply service the first one to call
// this method. The correct exception priority is:
// Highest    1   Reset (async)
//            2   Data abort (async)
//            3   FIQ (instruction boundaries)
//            4   IRQ (instruction boundaries)
//            5   Prefetch abort (async)
// Lowest     6   Undefined instruction (async)
//                SWI (sync)

// Interrupt handler behavior for interrupt port inta.
void ac_behavior(inta, value) {
  service_interrupt(*this, value);  
}

