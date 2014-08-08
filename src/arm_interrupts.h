#ifndef __ARM_INTERRUPTS_H__
#define __ARM_INTERRUPTS_H__

#include "arm_arch_ref.H"


namespace arm_impl {

    enum PrivilegeLevel { PL0=0, PL1};

    class processor_mode {
    public:
        static const unsigned int USER_MODE       = 0x10; // 0b10000;
        static const unsigned int FIQ_MODE        = 0x11; // 0b10001;
        static const unsigned int IRQ_MODE        = 0x12; // 0b10010;
        static const unsigned int SUPERVISOR_MODE = 0x13; // 0b10011;
        static const unsigned int ABORT_MODE      = 0x17; // 0b10111;
        static const unsigned int UNDEFINED_MODE  = 0x1B; // 0b11011;
        static const unsigned int SYSTEM_MODE     = 0x1F; // 0b11111;
        static const unsigned int MODE_MASK       = 0x1F; // 0b11111;

        bool fiq, irq, thumb;
        unsigned int mode;

        // Initializes with:
        // FIQ enabled  (CPSR FIQ disable bit set = false)
        // IRQ enabled
        // Thumb disabled
        // User mode
    processor_mode() : fiq(false), irq(false), thumb(false), mode (USER_MODE) {}

        //Returns PrivilegedLevel, PLx, based on current processor mode
        PrivilegeLevel getPrivilegeLevel();

        //Returns current processor mode name for debugging and status reasons.
        const char* currentMode_str();

    };

    enum exception_type {
        EXCEPTION_RESET, EXCEPTION_UNDEFINED_INSTR,  EXCEPTION_SWI,
        EXCEPTION_PREFETCH_ABORT, EXCEPTION_DATA_ABORT, EXCEPTION_IRQ,
        EXCEPTION_FIQ
    };
}

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
void service_interrupt(arm_arch_ref& ref, unsigned excep_type);

#endif

