/**
   * @file      arm_isa.cpp
   * @author    Danilo Marcolin Caravana
   *            Rafael Auler
   *            Gabriel Krisman Bertazi
   *
   *            The ArchC Team
   *            http://www.archc.org/
   *
   *            Computer Systems Laboratory (LSC)
   *            IC-UNICAMP
   *            http://www.lsc.ic.unicamp.br/
   *
   * @version   0.7
   * @date      Jul 2009
   *
   * @brief     The ArchC ARMv5e functional model.
   *
   * @attention Copyright (C) 2002-2009 --- The ArchC Team
   *
   */

#include "arm_isa.H"
#include "arm_isa_init.cpp"
#include "arm_bhv_macros.H"
#include "arm_interrupts.h"
#include "debug_backtrace.h"
#include <stdint.h> // define types uint32_t, etc
#include "coprocessor.h"
#include "mmu.h"

using namespace arm_parms;

extern bool DEBUG_FLOW;
extern bool DEBUG_CORE;
extern coprocessor * CP[16];
extern MMU *mmu;

#include "defines.H"

#define dprintf(args...) if(DEBUG_CORE){        \
        dprint_flow_indentation();              \
        fflush(stderr);                         \
        fprintf(stderr,args);                   \
    }

//! User defined macros to access a single bit
#define isBitSet(variable, position) (((variable & (1 << (position))) != 0) ? true : false)
#define getBit(variable, position) (((variable & (1 << (position))) != 0) ? true : false)
#define setBit(variable, position) variable = variable | (1 << (position))
#define clearBit(variable, position) variable = variable & (~(1 << (position)))

//! User defined macros to reference registers
#define LR 14 // link return
#define PC 15 // program counter

#define WORD 4
#define HALF 2
#define BYTE 1

arm_impl::processor_mode arm_proc_mode;
static arm_arch_ref *ref = 0;

//! Useful abstract data types defining ARM flags and register access
typedef struct flag_s {
    bool N; // Negative
    bool Z; // Zero
    bool C; // Carry
    bool V; // Overflow
    bool Q; // DSP
    bool T; // Thumb
} flag_t;

typedef union {
    int8_t byte[4];
    int32_t entire;
    uint32_t uentire;
} reg_t;

typedef union {
    int32_t reg[2];
    int64_t hilo;
    uint64_t uhilo;
} r64bit_t;

//! Global instances used throughout the model.
static flag_t flags;
static bool execute;

static reg_t dpi_shiftop;
static bool dpi_shiftopcarry;

static reg_t ls_address;
static reg_t lsm_startaddress;
static reg_t lsm_endaddress;

static reg_t OP1;
static reg_t OP2;

// #ifdef SYSTEM_MODEL
#define RB_write       bypass_write
#define RB_read        bypass_read
#define MEM_read       MEM.read
#define MEM_read_byte  MEM.read_byte
#define MEM_write      MEM.write
#define MEM_write_half MEM.write_half
#define MEM_write_byte MEM.write_byte
//#endif

// If SYSTEM_MODEL, These methods take control whenever
// a instruciton attempts to write/read the main
// Register Bank.
// they route the read/write procedure to the correct
// register based upon current processor state.
void bypass_write(unsigned address, unsigned datum) {
    if(address == 15)
    {
        bool aux = dprintbt_verifyEnd(datum);
        if(aux == true)
            dprintbt_leave();
    }

    if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
        arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
        ref->RB.write(address, datum);
        return;
    }
    switch (arm_proc_mode.mode) {
    case arm_impl::processor_mode::FIQ_MODE:
        switch (address) {
        case 14:
            ref->R14_fiq = datum;
            break;
        case 13:
            ref->R13_fiq = datum;
            break;
        case 12:
            ref->R12_fiq = datum;
            break;
        case 11:
            ref->R11_fiq = datum;
            break;
        case 10:
            ref->R10_fiq = datum;
            break;
        case 9:
            ref->R9_fiq = datum;
            break;
        case 8:
            ref->R8_fiq = datum;
            break;
        default:
            ref->RB.write(address, datum);
            break;
        }
        break;
    case arm_impl::processor_mode::IRQ_MODE:
        if (address == 14)
            ref->R14_irq = datum;
        else if (address == 13)
            ref->R13_irq = datum;
        else
            ref->RB .write(address, datum);
        break;
    case arm_impl::processor_mode::SUPERVISOR_MODE:
        if (address == 14)
            ref->R14_svc = datum;
        else if (address == 13)
            ref->R13_svc = datum;
        else
            ref->RB.write(address, datum);
        break;
    case arm_impl::processor_mode::ABORT_MODE:
        if (address == 14)
            ref->R14_abt = datum;
        else if (address == 13)
            ref->R13_abt = datum;
        else
            ref->RB.write(address, datum);
        break;
    case arm_impl::processor_mode::UNDEFINED_MODE:
        if (address == 14)
            ref->R14_und = datum;
        else if (address == 13)
            ref->R13_und = datum;
        else
            ref->RB.write(address, datum);
        break;
    }
}

unsigned bypass_read(unsigned address) {
    if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
        arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE)
        return ref->RB .read(address);
    switch (arm_proc_mode.mode) {
    case arm_impl::processor_mode::FIQ_MODE:
        switch (address) {
        case 14:
            return ref->R14_fiq;
            break;
        case 13:
            return ref->R13_fiq;
            break;
        case 12:
            return ref->R12_fiq;
            break;
        case 11:
            return ref->R11_fiq;
            break;
        case 10:
            return ref->R10_fiq;
            break;
        case 9:
            return ref->R9_fiq;
            break;
        case 8:
            return ref->R8_fiq;
            break;
        default:
            return ref->RB.read(address);
            break;
        }
        break;
    case arm_impl::processor_mode::IRQ_MODE:
        if (address == 14)
            return ref->R14_irq;
        else if (address == 13)
            return ref->R13_irq;
        else
            return ref->RB.read(address);
        break;
    case arm_impl::processor_mode::SUPERVISOR_MODE:
        if (address == 14)
            return ref->R14_svc;
        else if (address == 13)
            return ref->R13_svc;
        else
            return ref->RB.read(address);
        break;
    case arm_impl::processor_mode::ABORT_MODE:
        if (address == 14)
            return ref->R14_abt;
        else if (address == 13)
            return ref->R13_abt;
        else
            return ref->RB.read(address);
        break;
    case arm_impl::processor_mode::UNDEFINED_MODE:
        if (address == 14)
            return ref->R14_und;
        else if (address == 13)
            return ref->R13_und;
        else
            return ref->RB.read(address);
        break;
    }
}

//! Useful functions to easily describe arm instructions behavior

inline reg_t ArithmeticShiftRight(int shiftamount, reg_t reg) {

    reg_t tmp = reg;
    tmp.entire = ((int32_t)tmp.entire) >> shiftamount;
    return tmp;
}

inline reg_t RotateRight(int shiftamount, reg_t reg) {

    reg_t ret;
    ret.entire = (((uint32_t)reg.entire) >> shiftamount) | (((uint32_t)reg.entire) << (32 - shiftamount));

    return ret;
}

inline int32_t SignExtend(int32_t word, int word_length) {
#if 1
    const int32_t m = 1U << (word_length - 1);
    int32_t x = (word) & ((1ULL << word_length) - 1);
    return ((x ^ m) - m);
#else // does not work in x86_64
    return (int32_t)((int32_t)(word << (32 - word_length))) >> (32 - word_length);
#endif
}

inline int LSM_CountSetBits(reg_t registerList) {
    int i, count;

    count = 0;
    for (i=0; i<16; i++) { // Verify limits for big/little endian
        if (isBitSet(registerList.entire,i)) count++;
    }
    return count;
}

inline reg_t CPSRBuild() {

    reg_t CPSR;
    CPSR.entire = arm_proc_mode.mode;
    if (arm_proc_mode.fiq)
        setBit(CPSR.entire,6); // FIQ disable
    if (arm_proc_mode.irq)
        setBit(CPSR.entire,7); // IRQ disable
    if (arm_proc_mode.thumb)
        setBit(CPSR.entire,5); // Thumb
    if (flags.N) setBit(CPSR.entire,31); // N flag
    else clearBit(CPSR.entire,31);
    if (flags.Z) setBit(CPSR.entire,30); // Z flag
    else clearBit(CPSR.entire,30);
    if (flags.C) setBit(CPSR.entire,29); // C flag
    else clearBit(CPSR.entire,29);
    if (flags.V) setBit(CPSR.entire,28); // V flag
    else clearBit(CPSR.entire,28);
    if (flags.Q) setBit(CPSR.entire,27); // Q flag
    else clearBit(CPSR.entire,27);
    if (flags.T) setBit(CPSR.entire, 5); // T flag

    return CPSR;
}

uint32_t readCPSR() {
    return (uint32_t) CPSRBuild().entire;
}

void writeCPSR(unsigned value) {
    reg_t CPSR;

    CPSR.entire = value;
    flags.N = (getBit(CPSR.entire,31))? true : false;
    flags.Z = (getBit(CPSR.entire,30))? true : false;
    flags.C = (getBit(CPSR.entire,29))? true : false;
    flags.V = (getBit(CPSR.entire,28))? true : false;
    flags.Q = (getBit(CPSR.entire,27))? true : false;
    flags.T = (getBit(CPSR.entire,5))? true : false;
    arm_proc_mode.fiq = getBit(CPSR.entire,6)? true : false;
    arm_proc_mode.irq = getBit(CPSR.entire,7)? true : false;
    arm_proc_mode.mode = value & arm_impl::processor_mode::MODE_MASK;
}

// This function implements the transfer of the SPSR of the current processor
// mode to the CPSR, usually executed when exiting from an exception handler.
static void SPSRtoCPSR() {
    switch (arm_proc_mode.mode) {
    case arm_impl::processor_mode::FIQ_MODE:
        writeCPSR(ref->SPSR_fiq);
        break;
    case arm_impl::processor_mode::IRQ_MODE:
        writeCPSR(ref->SPSR_irq);
        break;
    case arm_impl::processor_mode::SUPERVISOR_MODE:
        writeCPSR(ref->SPSR_svc);
        break;
    case arm_impl::processor_mode::ABORT_MODE:
        writeCPSR(ref->SPSR_abt);
        break;
    case arm_impl::processor_mode::UNDEFINED_MODE:
        writeCPSR(ref->SPSR_und);
        break;
    default:
        printf("Invalid processor mode.\n");
        return;
    }
}
void writeSPSR(unsigned value) {
    switch (arm_proc_mode.mode) {
    case arm_impl::processor_mode::FIQ_MODE:
        ref->SPSR_fiq = value;
        break;
    case arm_impl::processor_mode::IRQ_MODE:
        ref->SPSR_irq = value;
        break;
    case arm_impl::processor_mode::SUPERVISOR_MODE:
        ref->SPSR_svc = value;
        break;
    case arm_impl::processor_mode::ABORT_MODE:
        ref->SPSR_abt = value;
        break;
    case arm_impl::processor_mode::UNDEFINED_MODE:
        ref->SPSR_und = value;
        break;
    }
}

unsigned readSPSR() {
    switch (arm_proc_mode.mode) {
    case arm_impl::processor_mode::FIQ_MODE:
        return ref->SPSR_fiq;
    case arm_impl::processor_mode::IRQ_MODE:
        return ref->SPSR_irq;
    case arm_impl::processor_mode::SUPERVISOR_MODE:
        return ref->SPSR_svc;
    case arm_impl::processor_mode::ABORT_MODE:
        return ref->SPSR_abt;
    case arm_impl::processor_mode::UNDEFINED_MODE:
        return ref->SPSR_und;
    }
    return 0;
}

void ac_behavior( begin ) {
    ref = this;
#ifdef SYSTEM_MODEL
    arm_proc_mode.mode = arm_impl::processor_mode::SUPERVISOR_MODE;
    arm_proc_mode.thumb = false;
    arm_proc_mode.fiq = true;
    arm_proc_mode.irq = true;
    ac_pc = 0x0;
    //    ac_pc = 0x70008000;
    //    RB_write (1, 3273); // imx53_loco machine type
#endif
}

//!Generic instruction behavior method.
void ac_behavior( instruction ) {

    dprintf("-------------------- PC=%#x -------------------- %lld\n", (uint32_t)ac_pc, ac_instr_counter);

    // Conditionally executes instruction based on COND field, common to all ARM instructions.
    execute = false;

    switch(cond) {
    case  0: if (flags.Z == true) execute = true; break;
    case  1: if (flags.Z == false) execute = true; break;
    case  2: if (flags.C == true) execute = true; break;
    case  3: if (flags.C == false) execute = true; break;
    case  4: if (flags.N == true) execute = true; break;
    case  5: if (flags.N == false) execute = true; break;
    case  6: if (flags.V == true) execute = true; break;
    case  7: if (flags.V == false) execute = true; break;
    case  8: if ((flags.C == true)&&(flags.Z == false)) execute = true; break;
    case  9: if ((flags.C == false)||(flags.Z == true)) execute = true; break;
    case 10: if (flags.N == flags.V) execute = true; break;
    case 11: if (flags.N != flags.V) execute = true; break;
    case 12: if ((flags.Z == false)&&(flags.N == flags.V)) execute = true; break;
    case 13: if ((flags.Z == true)||(flags.N != flags.V)) execute = true;  break;
    case 14: execute = true; break;
    default: execute = false;
    }

    // PC increment
    ac_pc += 4;
    RB_write(PC, ac_pc);

    if(!execute) {
        dprintf("cond=0x%X\n", cond);
        dprintf("Instruction will not be executed due to condition flags.\n");
        ac_annul();
    }
}

// Instruction Format behavior methods.

//!DPI1 - Second operand is register with imm shift
void ac_behavior( Type_DPI1 ) {

    reg_t RM2;
    // Special case: rm = 15
    if (rm == 15) {
        // PC is already incremented by four, so only add 4 again (not 8)
        RM2.entire = RB_read(rm) + 4;
    }
    else RM2.entire = RB_read(rm);

    switch(shift) {
    case 0: // Logical shift left
        if ((shiftamount >= 0) && (shiftamount <= 31)) {
            if (shiftamount == 0) {
                dpi_shiftop.entire = RM2.entire;
                dpi_shiftopcarry = flags.C;
            } else {
                dpi_shiftop.entire = RM2.entire << shiftamount;
                dpi_shiftopcarry = getBit(RM2.entire, 32 - shiftamount);
            }
        }
        break;
    case 1: // Logical shift right
        if ((shiftamount >= 0) && (shiftamount <= 31)) {
            if (shiftamount == 0) {
                dpi_shiftop.entire = 0;
                dpi_shiftopcarry = getBit(RM2.entire, 31);
            } else {
                dpi_shiftop.entire = ((uint32_t) RM2.entire) >> shiftamount;
                dpi_shiftopcarry = getBit(RM2.entire, shiftamount - 1);
            }
        }
        break;
    case 2: // Arithmetic shift right
        if ((shiftamount >= 0) && (shiftamount <= 31)) {
            if (shiftamount == 0) {
                if (!isBitSet(RM2.entire, 31)) {
                    dpi_shiftop.entire = 0;
                    dpi_shiftopcarry = getBit(RM2.entire, 31);
                } else {
                    dpi_shiftop.entire = 0xFFFFFFFF;
                    dpi_shiftopcarry = getBit(RM2.entire, 31);
                }
            } else {
                dpi_shiftop.entire = ((int32_t) RM2.entire) >> shiftamount;
                dpi_shiftopcarry = getBit(RM2.entire, shiftamount - 1);
            }
        }
        break;
    default: // Rotate right
        if ((shiftamount >= 0) && (shiftamount <= 31)) {
            if (shiftamount == 0) { //Rotate right with extend
                dpi_shiftopcarry = getBit(RM2.entire, 0);
                dpi_shiftop.entire = (((uint32_t)RM2.entire) >> 1);
                if (flags.C) setBit(dpi_shiftop.entire, 31);
            } else {
                dpi_shiftop.entire = (RotateRight(shiftamount, RM2)).entire;
                dpi_shiftopcarry = getBit(RM2.entire, shiftamount - 1);
            }
        }
    }
}

//!DPI2 - Second operand is shifted (shift amount given by third register operand)
void ac_behavior( Type_DPI2 ) {

    int rs40;
    reg_t RS2, RM2;

    // Special case: r* = 15
    if ((rd == 15)||(rm == 15)||(rn == 15)||(rs == 15)) {
        printf("Register 15 cannot be used in this instruction.\n");
        ac_annul();
    }

    RM2.entire = RB_read(rm);
    RS2.entire = RB_read(rs);
    rs40 = ((uint32_t)RS2.entire) & 0x0000000F;

    switch(shift){
    case 0: // Logical shift left
        if (RS2.byte[0] == 0) {
            dpi_shiftop.entire = RM2.entire;
            dpi_shiftopcarry = flags.C;
        }
        else if (((uint8_t)RS2.byte[0]) < 32) {
            dpi_shiftop.entire = RM2.entire << (uint8_t)RS2.byte[0];
            dpi_shiftopcarry = getBit(RM2.entire, 32 - ((uint8_t)RS2.byte[0]));
        }
        else if (RS2.byte[0] == 32) {
            dpi_shiftop.entire = 0;
            dpi_shiftopcarry = getBit(RM2.entire, 0);
        }
        else { // rs > 32
            dpi_shiftop.entire = 0;
            dpi_shiftopcarry = 0;
        }
        break;
    case 1: // Logical shift right
        if (RS2.byte[0] == 0) {
            dpi_shiftop.entire = RM2.entire;
            dpi_shiftopcarry = flags.C;
        }
        else if (((uint8_t)RS2.byte[0]) < 32) {
            dpi_shiftop.entire = ((uint32_t) RM2.entire) >> ((uint8_t)RS2.byte[0]);
            dpi_shiftopcarry = getBit(RM2.entire, (uint8_t)RS2.byte[0] - 1);
        }
        else if (RS2.byte[0] == 32) {
            dpi_shiftop.entire = 0;
            dpi_shiftopcarry = getBit(RM2.entire, 31);
        }
        else { // rs > 32
            dpi_shiftop.entire = 0;
            dpi_shiftopcarry = 0;
        }
        break;
    case 2: // Arithmetical shift right
        if (RS2.byte[0] == 0) {
            dpi_shiftop.entire = RM2.entire;
            dpi_shiftopcarry = flags.C;
        }
        else if (((uint8_t)RS2.byte[0]) < 32) {
            dpi_shiftop.entire = ((int32_t) RM2.entire) >> ((uint8_t)RS2.byte[0]);
            dpi_shiftopcarry = getBit(RM2.entire, ((uint8_t)RS2.byte[0]) - 1);
        } else { // rs >= 32
            if (!isBitSet(RM2.entire, 31)) {
                dpi_shiftop.entire = 0;
                dpi_shiftopcarry = getBit(RM2.entire, 31);
            }
            else { // rm_31 == 1
                dpi_shiftop.entire = 0xFFFFFFFF;
                dpi_shiftopcarry = getBit(RM2.entire, 31);
            }
        }
        break;
    default: // Rotate right
        if (RS2.byte[0] == 0) {
            dpi_shiftop.entire = RM2.entire;
            dpi_shiftopcarry = flags.C;
        }
        else if (rs40 == 0) {
            dpi_shiftop.entire = RM2.entire;
            dpi_shiftopcarry = getBit(RM2.entire, 31);
        }
        else { // rs40 > 0
            dpi_shiftop.entire = (RotateRight(rs40, RM2)).entire;
            dpi_shiftopcarry = getBit(RM2.entire, rs40 - 1);
        }
    }
}

//!DPI3 - Second operand is immediate shifted by another imm
void ac_behavior( Type_DPI3 ){
    int32_t tmp;
    tmp = (uint32_t)imm8;
    dpi_shiftop.entire = (((uint32_t)tmp) >> (2 * rotate)) | (((uint32_t)tmp) << (32 - (2 * rotate)));

    if (rotate == 0)
        dpi_shiftopcarry = flags.C;
    else
        dpi_shiftopcarry = getBit(dpi_shiftop.entire, 31);
}

//DPI4 behavior
void ac_behavior( Type_DPI4 ) {
    dprintf("Instruction type: DPI4\n");
    // Concatenate immediate
    long tmp = (unsigned long) imm4;
    tmp = (tmp << 12) | imm12;
    dpi_shiftop.entire = (unsigned long) tmp;
}


//DPI5 behavior
void ac_behavior( Type_DPI5 ) {
    dprintf("Instruction type: DPI5\n");
    // Apply Shift to Rm
    if(tb == 1) {
        dpi_shiftop.entire  =  RB.read(rm);
        dpi_shiftop = ArithmeticShiftRight(shiftamount, dpi_shiftop);
    }
    else
        dpi_shiftop.entire  =  RB.read(rm) << shiftamount;
}

void ac_behavior(Type_BTM1 )  {
    // no special actions necessary
}
void ac_behavior( Type_BBL )  {
    // no special actions necessary
}
void ac_behavior( Type_BBLT ) {
    // no special actions necessary
}
void ac_behavior( Type_MBXBLX ) {
    // no special actions necessary
}

//!MULT1 - 32-bit result multiplication
void ac_behavior( Type_MULT1 ) {
    // no special actions necessary
}

//!MULT2 - 64-bit result multiplication
void ac_behavior( Type_MULT2 ) {
    // no special actions necessary
}

//!MULT2 - 64-bit result multiplication
void ac_behavior( Type_MEMEX ) {
    // no special actions necessary
}

//!LSI - Load Store Immediate Offset/Index
void ac_behavior( Type_LSI ) {

    reg_t RN2;
    RN2.entire = RB_read(rn);
    ls_address.entire = 0;

    if((p == 1)&&(w == 0)) { // immediate pre-indexed without writeback
        // Special case: Rn = PC
        if (rn == PC)
            ls_address.entire = 4;

        if(u == 1) {
            ls_address.entire += RN2.entire + (uint32_t) imm12;
        } else {
            ls_address.entire += RN2.entire - (uint32_t) imm12;
        }
    }

    else if((p == 1)&&(w == 1)) { // immediate pre-indexed with writeback
        // Special case: Rn = PC
        if (rn == PC) {
            printf("Unpredictable LSI instruction result (Can't writeback to PC, Rn = PC)\n");
            ac_annul();
            return;
        }
        // Special case: Rn = Rd
        if (rn == rd) {
            printf("Unpredictable LSI instruction result  (Can't writeback to loaded register, Rn = Rd)\n");
            ac_annul();
            return;
        }

        if(u == 1) {
            ls_address.entire = RN2.entire + (uint32_t) imm12;
        } else {
            ls_address.entire = RN2.entire - (uint32_t) imm12;
        }
        RB_write(rn,ls_address.entire);
    }

    else if((p == 0)&&(w == 0)) { // immediate post-indexed (writeback)
        // Special case: Rn = PC
        if (rn == PC) {
            printf("Unpredictable LSI instruction result (Can't writeback to PC, Rn = PC)\n");
            ac_annul();
            return;
        }
        // Special case Rn = Rd
        if (rn == rd) {
            printf("Unpredictable LSI instruction result (Can't writeback to loaded register, Rn = Rd)\n");
            ac_annul();
            return;
        }

        ls_address.entire = RN2.entire;
        if(u == 1) {
            //checar se imm12 soma direto
            RB_write(rn, ls_address.entire + (uint32_t) imm12);
        } else {
            RB_write(rn, ls_address.entire - (uint32_t) imm12);
        }
    }
    /* FIXME: Check word alignment (Rd = PC) Address[1:0] = 0b00 */

}

//!LSR - Scaled Register Offset/Index
void ac_behavior( Type_LSR ) {

    reg_t RM2, RN2, index, tmp;

    RM2.entire = RB_read(rm);
    RN2.entire = RB_read(rn);
    ls_address.entire = 0;

    if ((p == 1)&&(w == 0)) { // offset
        // Special case: PC
        if(rn == PC)
            ls_address.entire = 4;

        if(rm == PC) {
            printf("Unpredictable LSR instruction result (Illegal usage of PC, Rm = PC)\n");
            return;
        }

        switch(shift){
        case 0:
            if(shiftamount == 0) { // Register
                index.entire = RM2.entire;
            } else { // Scalled logical shift left
                index.entire = RM2.entire << shiftamount;
            }
            break;
        case 1: // logical shift right
            if(shiftamount == 0) index.entire = 0;
            else index.entire = ((uint32_t) RM2.entire) >> shiftamount;
            break;
        case 2: // arithmetic shift right
            if(shiftamount == 0) {
                if (isBitSet(RM2.entire, 31)) index.entire = 0xFFFFFFFF;
                else index.entire = 0;
            } else index.entire = ((int32_t) RM2.entire) >> shiftamount;
            break;
        default:
            if(shiftamount == 0) { // RRX
                tmp.entire = 0;
                if(flags.C) setBit(tmp.entire, 31);
                index.entire = tmp.entire | (((uint32_t) RM2.entire) >> 1);
            } else { // rotate right
                index.entire = (RotateRight(shiftamount, RM2)).entire;
            }
        }

        if(u == 1) {
            ls_address.entire += (RN2.entire + index.entire);
        } else {
            ls_address.entire += (RN2.entire - index.entire);
        }
    }

    else if((p == 1)&&(w == 1)) { // pre-indexed
        // Special case: Rn = PC
        if (rn == PC) {
            printf("Unpredictable LSR instruction result (Can't writeback to PC, Rn = PC)\n");
            ac_annul();
            return;
        }
        // Special case Rn = Rd
        if (rn == rd) {
            printf("Unpredictable LSR instruction result (Can't writeback to loaded register, Rn = Rd)\n");
            ac_annul();
            return;
        }
        // Special case Rm = PC
        if (rm == PC) {
            printf("Unpredictable LSR instruction result (Illegal usage of PC, Rm = PC)\n");
            ac_annul();
            return;
        }
        // Special case Rn = Rm
        if (rn == rm) {
            printf("Unpredictable LSR instruction result (Can't use the same register for Rn and Rm\n");
            ac_annul();
            return;
        }

        switch(shift){
        case 0:
            if(shiftamount == 0) { // Register
                index.entire = RM2.entire;
            } else { // Scaled logical shift left
                index.entire = RM2.entire << shiftamount;
            }
            break;
        case 1: // logical shift right
            if(shiftamount == 0) index.entire = 0;
            else index.entire = ((uint32_t) RM2.entire) >> shiftamount;
            break;
        case 2: // arithmetic shift right
            if(shiftamount == 0) {
                if (isBitSet(RM2.entire,31))
                    index.entire = 0xFFFFFFFF;
                else
                    index.entire = 0;
            } else index.entire = ((int32_t) RM2.entire) >> shiftamount;
            break;
        default:
            if(shiftamount == 0) { // RRX
                tmp.entire = 0;
                if (flags.C) setBit(tmp.entire,31);
                index.entire = tmp.entire | (((uint32_t) RM2.entire) >> 1);
            } else { // rotate right
                index.entire = (RotateRight(shiftamount, RM2)).entire;
            }
        }

        if(u == 1) {
            ls_address.entire = RN2.entire + index.entire;
        } else {
            ls_address.entire = RN2.entire - index.entire;
        }

        RB_write(rn, ls_address.entire);
    }

    else if((p == 0)&&(w == 0)) { // post-indexed
        // Special case: Rn = PC
        if (rn == PC) {
            printf("Unpredictable LSR instruction result (Can't writeback to PC, Rn = PC)\n");
            ac_annul();
            return;
        }
        // Special case Rn = Rd
        if (rn == rd) {
            printf("Unpredictable LSR instruction result (Can't writeback to loaded register, Rn = Rd)\n");
            ac_annul();
            return;
        }
        // Special case Rm = PC
        if (rm == PC) {
            printf("Unpredictable LSR instruction result (Illegal usage of PC, Rm = PC)\n");
            ac_annul();
            return;
        }
        // Special case Rn = Rm
        if (rn == rm) {
            printf("Unpredictable LSR instruction result (Can't use the same register for Rn and Rm\n");
            ac_annul();
            return;
        }

        ls_address.entire = RN2.entire;

        switch(shift) {
        case 0:
            if(shiftamount == 0) { // Register
                index.entire = RM2.entire;
            } else { // Scaled logical shift left
                index.entire = RM2.entire << shiftamount;
            }
            break;
        case 1: // logical shift right
            if(shiftamount == 0) index.entire = 0;
            else index.entire = ((uint32_t) RM2.entire) >> shiftamount;
            break;
        case 2: // arithmetic shift right
            if(shiftamount == 0) {
                if (isBitSet(RM2.entire, 31))
                    index.entire = 0xFFFFFFFF;
                else
                    index.entire = 0;
            } else index.entire = ((int32_t) RM2.entire) >> shiftamount;
            break;
        default:
            if(shiftamount == 0) { // RRX
                tmp.entire = 0;
                if(flags.C) setBit(tmp.entire, 31);
                index.entire = tmp.entire | (((uint32_t) RM2.entire) >> 1);
            } else { // rotate right
                index.entire = (RotateRight(shiftamount, RM2)).entire;
            }
        }

        if(u == 1) {
            RB_write(rn, RN2.entire + index.entire);
        } else {
            RB_write(rn, RN2.entire - index.entire);
        }
    }
}

//!LSE - Load Store HalfWord
void ac_behavior( Type_LSE ){

    int32_t off8;
    reg_t RM2, RN2;

    // Special cases handling
    if((p == 0)&&(w == 1)) {
        printf("Unpredictable LSE instruction result");
        ac_annul();
        return;
    }
    if((ss == 0)&&(hh == 0)) {
        printf("Decoding error: this is not a LSE instruction");
        ac_annul();
        return;
    }
    if((ss == 1)&&(l == 0))
        dprintf("Special DSP\n");
    // FIXME: Test LDRD and STRD second registers in case of writeback

    RN2.entire = RB_read(rn);

    // nos LSE's que usam registrador, o campo addr2 armazena Rm
    RM2.entire = RB_read(addr2);
    off8 = ((uint32_t)(addr1 << 4) | addr2);
    ls_address.entire = 0;

    if(p == 1) { // offset ou pre-indexed
        if((i == 1)&&(w == 0)) { // immediate offset
            if(rn == PC)
                ls_address.entire = 4;
            if(u == 1) {
                ls_address.entire += (RN2.entire + off8);
            } else {
                ls_address.entire += (RN2.entire - off8);
            }
        }

        else if((i == 0)&&(w == 0)) { // register offset
            // Special case Rm = PC
            if (addr2 == PC) {
                printf("Unpredictable LSE instruction result (Illegal usage of PC, Rm = PC)\n");
                ac_annul();
                return;
            }

            if(rn == PC)
                ls_address.entire = 4;

            if(u == 1) {
                ls_address.entire += (RN2.entire + RM2.entire);
            } else  {
                ls_address.entire += (RN2.entire - RM2.entire);
            }
        }

        else if ((i == 1)&&(w == 1)) { // immediate pre-indexed
            // Special case: Rn = PC
            if (rn == PC) {
                printf("Unpredictable LSE instruction result (Can't writeback to PC, Rn = PC)\n");
                ac_annul();
                return;
            }
            // Special case Rn = Rd
            if (rn == rd) {
                printf("Unpredictable LSE instruction result (Can't writeback to loaded register, Rn = Rd)\n");
                ac_annul();
                return;
            }

            if(u == 1) {
                ls_address.entire = (RN2.entire + off8);
            } else {
                ls_address.entire = (RN2.entire - off8);
            }

            RB_write(rn, ls_address.entire);
        }

        else { // i == 0 && w == 1: register pre-indexed
            // Special case: Rn = PC
            if (rn == PC) {
                printf("Unpredictable LSE instruction result (Can't writeback to PC, Rn = PC)\n");
                ac_annul();
                return;
            }
            // Special case Rn = Rd
            if (rn == rd) {
                printf("Unpredictable LSE instruction result (Can't writeback to loaded register, Rn = Rd)\n");
                ac_annul();
                return;
            }
            // Special case Rm = PC
            if (addr2 == PC) {
                printf("Unpredictable LSE instruction result (Illegal usage of PC, Rm = PC)\n");
                ac_annul();
                return;
            }
            // Special case Rn = Rm
            if (rn == addr2) {
                printf("Unpredictable LSE instruction result (Can't use the same register for Rn and Rm\n");
                ac_annul();
                return;
            }

            if(u == 1) {
                ls_address.entire = (RN2.entire + RM2.entire);
            } else {
                ls_address.entire = (RN2.entire - RM2.entire);
            }

            RB_write(rn, ls_address.entire);
        }

    } else { // p == 0: post-indexed
        if((i == 1)&&(w == 0)) { // immediate post-indexed
            if(rn == PC) {
                printf("Unpredictable LSE instruction result");
                ac_annul();
                return;
            }

            ls_address.entire = RN2.entire;
            if(u == 1) {
                RB_write(rn, RN2.entire + off8);
            } else {
                RB_write(rn, RN2.entire - off8);
            }
        }
        else if((i == 0)&&(w == 0)) { // register post-indexed
            // Special case: Rn = PC
            if (rn == PC) {
                printf("Unpredictable LSE instruction result (Can't writeback to PC, Rn = PC)\n");
                ac_annul();
                return;
            }
            // Special case Rn = Rd
            if (rn == rd) {
                printf("Unpredictable LSE instruction result (Can't writeback to loaded register, Rn = Rd)\n");
                ac_annul();
                return;
            }
            // Special case Rm = PC
            if (addr2 == PC) {
                printf("Unpredictable LSE instruction result (Illegal usage of PC, Rm = PC)\n");
                ac_annul();
                return;
            }
            // Special case Rn = Rm
            if (rn == addr2) {
                printf("Unpredictable LSE instruction result (Can't use the same register for Rn and Rm\n");
                ac_annul();
                return;
            }

            ls_address.entire = RN2.entire;
            if(u == 1) {
                RB_write(rn, RN2.entire + RM2.entire);
            } else {
                RB_write(rn, RN2.entire - RM2.entire);
            }
        }
    }
}

//!LSM - Load Store Multiple
void ac_behavior( Type_LSM ){

    reg_t RN2;
    int setbits;

    // Put registers list in a variable capable of addressing individual bits
    reg_t registerList;
    registerList.entire = (uint32_t) rlist;

    // Special case - empty list
    if (registerList.entire == 0) {
        printf("Unpredictable LSM instruction result (No register specified)\n");
        ac_annul();
        return;
    }

    RN2.entire = RB_read(rn);
    setbits = LSM_CountSetBits(registerList);

    if((p == 0)&&(u == 1)) { // increment after
        lsm_startaddress.entire = RN2.entire;
        lsm_endaddress.entire = RN2.entire + (setbits * 4) - 4;
        if(w == 1) RN2.entire += (setbits * 4);
    }
    else if((p == 1)&&(u == 1)) { // increment before
        lsm_startaddress.entire = RN2.entire + 4;
        lsm_endaddress.entire = RN2.entire + (setbits * 4);
        if(w == 1) RN2.entire += (setbits * 4);
    }
    else if((p == 0)&&(u == 0)) { // decrement after
        lsm_startaddress.entire = RN2.entire - (setbits * 4) + 4;
        lsm_endaddress.entire = RN2.entire;
        if(w == 1) RN2.entire -= (setbits * 4);
    }
    else { // decrement before
        lsm_startaddress.entire = RN2.entire - (setbits * 4);
        lsm_endaddress.entire = RN2.entire - 4;
        if(w == 1) RN2.entire -= (setbits * 4);
    }

    // Special case Rn in Rlist
    if((w == 1)&&(isBitSet(rlist,rn))) {
        printf("Unpredictable LSM instruction result (Can't writeback to loaded register, Rn in Rlist)\n");
        ac_annul();
        return;
    }

    RB_write(rn,RN2.entire);
}

void ac_behavior( Type_PCK1 ){
  reg_t RN2;
  RN2.entire = RB_read(rm);
  dpi_shiftop = RotateRight(rotate<<1, RN2);
}

void ac_behavior( Type_CDP ){
    // no special actions necessary
}
void ac_behavior( Type_CRT ){
    // no special actions necessary
}
void ac_behavior( Type_CLS ){
    // no special actions necessary
}
void ac_behavior( Type_MBKPT ){
    // no special actions necessary
}
void ac_behavior( Type_MSWI ){
    // no special actions necessary
}
void ac_behavior( Type_MCLZ ){
    // no special actions necessary
}
void ac_behavior( Type_MMSR1 ){
    // no special actions necessary
}
void ac_behavior( Type_MMSR2 ){
    // no special actions necessary
}

void ac_behavior( Type_DSPSM ){

    reg_t RM2, RS2;

    RM2.entire = RB_read(rm);
    RS2.entire = RB_read(rs);

    // Special cases
    if((drd == PC)||(drn == PC)||(rm == PC)||(rs == PC)) {
        printf("Unpredictable SMLA<y><x> instruction result\n");
        return;
    }

    if(xx == 0)
        OP1.entire = SignExtend(RM2.entire, 16);
    else
        OP1.entire = SignExtend((RM2.entire >> 16), 16);

    if(yy == 0)
        OP2.entire = SignExtend(RS2.entire, 16);
    else
        OP2.entire = SignExtend((RS2.entire >> 16), 16);
}

void ac_behavior( Type_MSMC ) {
    // no special actions necessary
}

void ac_behavior( Type_MED1 ) {
    // no special actions necessary
}

//! Behavior Methods

//------------------------------------------------------
inline void ADC(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2;
    r64bit_t soma;

    dprintf("Instruction: ADC\n");
    RN2.entire = RB_read(rn);
    if(rn == PC) RN2.entire += 4;
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n  Carry=%d\n", RN2.entire,dpi_shiftop.entire,flags.C);
    soma.uhilo = (uint64_t)(uint32_t)RN2.uentire + (uint64_t)(uint32_t)dpi_shiftop.uentire;
    if (flags.C) soma.uhilo++;
    RD2.entire = soma.reg[0];
    RB_write(rd, RD2.entire);
    if ((s == 1)&&(rd == PC)) {
        printf("Unpredictable ADC instruction result\n");
        return;
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire,31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = ((soma.reg[1] != 0) ? true : false);
            flags.V = (((getBit(RN2.entire,31) && getBit(dpi_shiftop.entire,31) && (!getBit(RD2.entire,31))) ||
                        ((!getBit(RN2.entire,31)) && (!getBit(dpi_shiftop.entire,31)) && getBit(RD2.entire,31))) ? true : false);
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n", flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void ADD(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2;
    r64bit_t soma;

    dprintf("Instruction: ADD\n");
    RN2.entire = RB_read(rn);
    if(rn == PC) RN2.entire += 4;
    dprintf("Operands:\n", RN2.entire,dpi_shiftop.entire);
    dprintf("A = 0x%lX\n", RN2.entire);
    dprintf("B = 0x%lX\n",dpi_shiftop.entire);
    soma.uhilo = (uint64_t)(uint32_t)RN2.uentire + (uint64_t)(uint32_t)dpi_shiftop.uentire;
    RD2.entire = soma.reg[0];
    RB_write(rd, RD2.entire);
    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable ADD instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire,31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = ((soma.reg[1] != 0) ? true : false);
            flags.V = (((getBit(RN2.entire,31) && getBit(dpi_shiftop.entire,31) && (!getBit(RD2.entire,31))) ||
                        ((!getBit(RN2.entire,31)) && (!getBit(dpi_shiftop.entire,31)) && getBit(RD2.entire,31))) ? true : false);
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n", flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void AND(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2;

    dprintf("Instruction: AND\n");
    RN2.entire = RB_read(rn);
    dprintf("Operands:  A = 0x%lX  B = 0x%lX\n", RN2.entire,dpi_shiftop.entire);
    RD2.entire = RN2.entire & dpi_shiftop.entire;
    RB_write(rd, RD2.entire);

    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable AND instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire, 31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = dpi_shiftopcarry;
            // nothing happens with flags.V
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n", flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void B(int h, int offset,
              ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
              ac_reg<unsigned>& ac_pc) {

    uint32_t mem_pos, s_extend;

    // Note that PC is already incremented by 4, i.e., pointing to the next instruction

    if(h == 1) { // h? it is really "l"
        dprintf("Instruction: BL\n");
        RB_write(LR, RB_read(PC));
        dprintf("Branch return address: 0x%lX\n", RB_read(LR));
    } else dprintf("Instruction: B\n");
    s_extend = SignExtend((int32_t)(offset << 2), 26);
    mem_pos = (uint32_t)RB_read(PC) + 4 + s_extend;
    dprintf("Calculated branch destination: 0x%X\n", mem_pos);
    if((mem_pos < 0)) {
        fprintf(stderr, "Branch destination out of bounds\n");
        exit(EXIT_FAILURE);
        return;
    } else RB_write(PC, mem_pos);

    if(h == 1) // BL
        dprintbt_enter(mem_pos, RB_read(LR));

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void BX(int rm,
               ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
               ac_reg<unsigned>& ac_pc) {

    dprintf("Instruction: BX\n");

    if(isBitSet(rm,0)) {
        printf("Change to thumb not implemented in this model. PC=%X\n", ac_pc.read());
        exit(0);
        return;
    }

    flags.T = isBitSet(rm, 0);
    ac_pc = RB_read(rm) & 0xFFFFFFFE;

    //dprintf("Pc = 0x%X",ac_pc);
}

//------------------------------------------------------
inline void BFI(int rd, int rn, int lsb, int msb,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    uint32_t dest = RB_read(rd);
    uint32_t orig;
    uint32_t mask = 0;

    //Generate bitmask
    int  width = (msb - lsb) + 1;
    for(int i = 0; i < width; i++)
        mask = (mask << 1) | 0x1;

    mask = mask << lsb;

    if(rn == 0x0F)
        orig = 0x0;          //BFC: clear bits in range
    else
        orig = RB_read(rn);  //BFI: copy bits in range from Rn

    //Writes modified bits to register
    dest = (dest & ~mask) | (mask & orig);
    RB_write(rd,dest);

    dprintf("Instruction: BFI\n");
    dprintf("Operands:\nRn=0x%X, contains 0x%lX\n\nDestination: Rd=0x%X\n", rn,orig,rd);
}

//------------------------------------------------------

inline void UBFX(int rd, int rn, int lsb, int width,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc) {

  uint32_t n = RB_read(rn);
  uint32_t msb = lsb + width - 1;
  uint32_t mask = 0;
  dprintf("Instruction: UBFX\n");

  dprintf("Operands:\n  Rd = 0x%lX\n  Rn(r%d) = 0x%lX lsb=%d width=%d\n",
          rd, rn, n, lsb, width);

  if(rd == 15 || rn == 15) {
      printf("Unpredictable UBFX instruction result\n");
    }

  for(int i = lsb; i <= msb; i++)
    setBit(mask, i);

  n = (n & mask) >> lsb;
  RB_write(rd,n);
}

inline void SBFX(int rd, int rn, int lsb, int width,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc) {

  uint32_t n = RB_read(rn);
  uint32_t msb = lsb + width - 1;
  uint32_t mask = 0;
  dprintf("Instruction: SBFX\n");

  dprintf("Operands:\n  Rd = 0x%lX\n  Rn(r%d) = 0x%lX lsb=%d width=%d\n",
          rd, rn, n, lsb, width);

  if(rd == 15 || rn == 15 || msb > 31) {
      printf("Unpredictable SBFX instruction result\n");
    }

  for(int i = lsb; i <= msb; i++)
    setBit(mask, i);

  n = (n & mask) >> lsb;
  n = SignExtend (n, msb);

  RB_write(rd,n);
}

//------------------------------------------------------
inline void UXTB(int rd,int rm,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc) {

    dprintf("Instruction: UXTB\n");
    dprintf("Operands:\n  Rm(rotated) = 0x%lX\n", dpi_shiftop.entire);

    if(rd == 15 || rm == 15) {
        printf("Unpredictable UXTB instruction result\n");
        return;
    }

    RB_write(rd, (dpi_shiftop.entire & 0xFF));

}
//------------------------------------------------------
inline void UXTH(int rd,int rm,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc) {

    dprintf("Instruction: UXTH\n");
    dprintf("Operands:\n  Rm(rotated) = 0x%lX\n", dpi_shiftop.entire);

    if(rd == 15 || rm == 15) {
        printf("Unpredictable UXTH instruction result\n");
        return;
    }

    RB_write(rd, (dpi_shiftop.entire & 0xFFFF));

}

//------------------------------------------------------
inline void SXTH(int rd,int rm,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc) {

    dprintf("Instruction: SXTH\n");
    dprintf("Operands:\n  Rm(rotated) = 0x%lX\n", dpi_shiftop.entire);

    if(rd == 15 || rm == 15) {
        printf("Unpredictable SXTH instruction result\n");
        return;
    }

    RB_write(rd, SignExtend((dpi_shiftop.entire & 0xFFFF), 16));

}

inline void REV(int rd, int rm,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

  uint32_t rm_r = RB_read(rm);
  uint32_t aux;

  dprintf("Instruction: REV\n");
  dprintf("Operands:\n  Rm) = 0x%lX\n", rm_r);

  if(rd == 15 || rm == 15) {
        printf("Unpredictable REV instruction result\n");
        return;
  }

  // Revert bits
  // AA BB CC DD becomes DD CC BB AA
  aux = ((rm_r & 0xFF) << 24
         | (rm_r & 0xFF<<8) << 8
         | (rm_r & (0xFF<<16)) >> 8
         | (rm_r & (0xFF<<24)) >> 24);

  RB_write(rd, aux);
}


//------------------------------------------------------
inline void BIC(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2;

    dprintf("Instruction: BIC\n");
    RN2.entire = RB_read(rn);
    RD2.entire = RN2.entire & ~dpi_shiftop.entire;
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n", RN2.entire,dpi_shiftop.entire);
    RB_write(rd,RD2.entire);

    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable BIC instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire,31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = dpi_shiftopcarry;
            // nothing happens with flags.V
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n", flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void CDP(){
    dprintf("Instruction: CDP\n");
    fprintf(stderr,"Warning: CDP is not implemented in this model.\n");
}

//------------------------------------------------------
inline void CLZ(int rd, int rm,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RM2;
    int i;

    dprintf("Instruction: CLZ\n");

    // Special cases
    if((rd == PC)||(rm == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable CLZ instruction result\n");
            return;
        }
        SPSRtoCPSR();
    }

    RM2.entire = RB_read(rm);

    if(RM2.entire == 0) RD2.entire = 32;
    else {
        i = 31;
        while((i>=0)&&(!isBitSet(RM2.entire,i))) i--;
        RD2.entire = 31 - i;
    }

    RB_write(rd, RD2.entire);
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void CMN(int rn,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RN2, alu_out;
    r64bit_t soma;

    dprintf("Instruction: CMN\n");
    RN2.entire = RB_read(rn);
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n", RN2.entire,dpi_shiftop.entire);
    soma.uhilo = (uint64_t)(uint32_t)RN2.uentire + (uint64_t)(uint32_t)dpi_shiftop.uentire;
    alu_out.entire = soma.reg[0];

    flags.N = getBit(alu_out.entire,31);
    flags.Z = ((alu_out.entire == 0) ? true : false);
    flags.C = ((soma.reg[1] != 0) ? true : false);
    flags.V = (((getBit(RN2.entire,31) && getBit(dpi_shiftop.entire,31) && (!getBit(alu_out.entire,31))) ||
                ((!getBit(RN2.entire,31)) && (!getBit(dpi_shiftop.entire,31)) && getBit(alu_out.entire,31))) ? true : false);

    dprintf("Results: 0x%lX\n *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n", alu_out.entire,flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void CMP(int rn,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RN2, alu_out, neg_shiftop;
    r64bit_t result;

    dprintf("Instruction: CMP\n");
    RN2.entire = RB_read(rn);
    dprintf("Operands: A = 0x%lX  B = 0x%lX\n", RN2.entire,dpi_shiftop.entire);
    neg_shiftop.entire = - dpi_shiftop.entire;
    result.hilo = (uint64_t)(uint32_t)RN2.entire + (uint64_t)(uint32_t)neg_shiftop.entire;
    alu_out.entire = result.reg[0];

    flags.N = getBit(alu_out.entire,31);
    flags.Z = ((alu_out.entire == 0) ? true : false);
    flags.C = !(((uint32_t) dpi_shiftop.uentire > (uint32_t) RN2.uentire) ? true : false);
    flags.V = (((getBit(RN2.entire,31) && getBit(neg_shiftop.entire,31) && (!getBit(alu_out.entire,31))) ||
                ((!getBit(RN2.entire,31)) && (!getBit(neg_shiftop.entire,31)) && getBit(alu_out.entire,31))) ? true : false);

    dprintf("Results: 0x%lX\n *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n", alu_out.entire,flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void EOR(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2;

    dprintf("Instruction: EOR\n");
    RN2.entire = RB_read(rn);
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n", RN2.entire,dpi_shiftop.entire);
    RD2.entire = RN2.entire ^ dpi_shiftop.entire;
    RB_write(rd, RD2.entire);

    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable EOR instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire, 31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = dpi_shiftopcarry;
            // nothing happens with flags.V
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void LDC(){
    dprintf("Instruction: LDC\n");
    fprintf(stderr,"Warning: LDC instruction is not implemented in this model.\n");
}

//------------------------------------------------------
inline void LDM(int rlist, bool r,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    // todo special cases

    int i;
    int32_t value;

    if (r == 0) { // LDM(1)
        dprintf("Instruction: LDM\n");
        ls_address = lsm_startaddress;
        dprintf("Initial address: 0x%lX\n",ls_address.entire);
        for(i=0;i<15;i++){
            if(isBitSet(rlist,i)) {
                dprintf("*  Loaded register: 0x%X; Value: 0x%X; Next address:0x%lX\n", i,RB_read(i),ls_address.entire);
                RB_write(i,MEM_read(ls_address.entire));
                ls_address.entire += 4;
            }
        }

        if((isBitSet(rlist,PC))) { // LDM(1)
            value = MEM_read(ls_address.entire);
            dprintf("*  Loaded register: PC; Next address: 0x%lX\n", ls_address.entire+4);
            RB_write(PC,value & 0xFFFFFFFE);
            ls_address.entire += 4;

        }
    } else {
        // LDM(2) similar to LDM(1), except for the above "if"
        if (arm_proc_mode.getPrivilegeLevel() == arm_impl::PL0) {
            fprintf (stderr, "Unpredictable behavior for LDM2/LDM3\n");
            abort();
        }
        dprintf("Instruction: LDM\n");
        ls_address = lsm_startaddress;
        dprintf("Initial address: 0x%lX\n",ls_address.entire);
        for(i=0;i<15;i++){
            if(isBitSet(rlist,i)) {
                dprintf("*  Loaded register: 0x%X; Value: 0x%X; Next address: 0x%lX\n", i,RB_read(i),ls_address.entire+4);
                RB.write(i,MEM_read(ls_address.entire));
                ls_address.entire += 4;
            }
        }
        if((isBitSet(rlist,PC))) { // LDM(3)
            value = MEM_read(ls_address.entire);
            dprintf("*  Loaded register: PC; Next address: 0x%lX\n", ls_address.entire+4);
            RB.write(PC,value & 0xFFFFFFFE);
            ls_address.entire += 4;
            SPSRtoCPSR();
        }
    }
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void LDR(int rd, int rn,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    int32_t value;
    reg_t tmp;
    int addr10;

    dprintf("Instruction: LDR\n");
    addr10 = (uint32_t) ls_address.entire & 0x00000003;

    // Special cases
    // TODO: Verify coprocessor cases (alignment)
#ifdef UNALIGNED_ACCESS_SUPPORT
    value = MEM_read(ls_address.entire);
#else
    ls_address.entire &= 0xFFFFFFFC;
    switch(addr10) {
    case 0:
        value = MEM_read(ls_address.entire);
        break;
    case 1:
        tmp.entire = MEM_read(ls_address.entire);
        value = (RotateRight(8,tmp)).entire;
        break;
    case 2:
        tmp.entire = MEM_read(ls_address.entire);
        value = (RotateRight(16,tmp)).entire;
        break;
    default:
        tmp.entire = MEM_read(ls_address.entire);
        value = (RotateRight(24,tmp)).entire;
    }
#endif // UNALIGNED_ACCESS_SUPPORT

    dprintf("Reading memory position 0x%08X\n", ls_address.entire);

    if(rd == PC) {
        RB_write(PC,(value & 0xFFFFFFFE));
        flags.T = isBitSet(value,0);
        dprintf(" *  PC <= 0x%08X\n", value & 0xFFFFFFFE);
    }
    else
    {
        RB_write(rd,value);
        dprintf(" *  R%d <= 0x%08X\n", rd, value);
    }

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void LDRB(int rd, int rn,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {
    uint8_t value;

    dprintf("Instruction: LDRB\n");

    // Special cases
    dprintf("Reading memory position 0x%08X\n", ls_address.entire);
    value = (uint8_t) MEM_read_byte(ls_address.entire);

    dprintf("Byte: 0x%X\n", value);
    RB_write(rd, ((uint32_t)value));

    dprintf(" *  R%d <= 0x%02X\n", rd, value);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void LDRBT(int rd, int rn,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    uint8_t value;

    dprintf("Instruction: LDRBT\n");

    // Special cases
    dprintf("Reading memory position 0x%08X\n", ls_address.entire);
    value = (uint8_t) MEM_read_byte(ls_address.entire);

    dprintf("Byte: 0x%X\n", (uint32_t) value);
    RB_write(rd, (uint32_t) value);

    dprintf(" *  R%d <= 0x%02X\n", rd, value);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void LDRD(int rd, int rn,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {
    uint32_t value1, value2;

    dprintf("Instruction: LDRD\n");
    dprintf("Reading memory position 0x%08X\n", ls_address.entire);
    value1 = MEM_read_byte(ls_address.entire);
    value2 = MEM_read_byte(ls_address.entire+4);

    // Special cases
    // Registrador destino deve ser par
    if(isBitSet(rd,0)){
        printf("Undefined LDRD instruction result (Rd must be even)\n");
        return;
    }
    // Verificar alinhamento do doubleword
    if((rd == LR)||(ls_address.entire & 0x00000007)){
        printf("Unpredictable LDRD instruction result (Address is not doubleword aligned) @ 0x%08X\n", RB_read(PC)-4);
        return;
    }

    //FIXME: Verify if writeback receives +4 from address
    RB_write(rd, value1);
    RB_write(rd+1, value2);

    dprintf(" *  R%d <= 0x%08X\n *  R%d <= 0x%08X\n (little) value = 0x%08X%08X\n (big) value = 0x%08X08X\n", rd, value1, rd+1, value2, value2, value1, value1, value2);

    ac_pc = RB_read(PC);
}
//------------------------------------------------------
inline void LDRH(int rd, int rn,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {
    uint32_t value;

    dprintf("Instruction: LDRH\n");
    dprintf("Reading memory position 0x%08X\n", ls_address.entire);

#ifndef UNALIGNED_ACCESS_SUPPORT
    // Special cases
    // verify coprocessor alignment
    // verify halfword alignment
    if(isBitSet(ls_address.entire,0)){
        printf("Unpredictable LDRH instruction result (Address is not Halfword Aligned)\n");
        return;
    }
#endif

    value = MEM_read(ls_address.entire);
    value &= 0xFFFF; /* Zero extends halfword value
                        BUG: Model must be little endian in order to the code work  */

    RB_write(rd, value);

    dprintf(" *  R%d <= 0x%04X\n", rd, value);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void LDRSB(int rd, int rn,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    uint32_t data;

    dprintf("Instruction: LDRSB\n");

    // Special cases
    dprintf("Reading memory position 0x%08X\n", ls_address.entire);
    data = MEM_read_byte(ls_address.entire);
    data = SignExtend(data, 8);

    RB_write(rd, data);

    dprintf(" *  R%d <= 0x%08X\n", rd, data);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void LDRSH(int rd, int rn,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc, ac_memory& MEM){

    uint32_t data;

    dprintf("Instruction: LDRSH\n");
    dprintf("Reading memory position 0x%08X\n", ls_address.entire);

#ifdef UNALIGNED_ACCESS_SUPPORT
    // Special cases
    // verificar alinhamento do halfword
    if(isBitSet(ls_address.entire, 0)) {
        printf("Unpredictable LDRSH instruction result (Address is not halfword aligned)\n");
        return;
    }
#endif
    // Verify coprocessor alignment

    data = MEM_read(ls_address.entire);
    data &= 0xFFFF; /* Extracts halfword
                       BUG: Model must be little endian */
    data = SignExtend(data,16);
    RB_write(rd, data);

    dprintf(" *  R%d <= 0x%08X\n", rd, data);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void LDRT(int rd, int rn,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    int addr10;
    reg_t tmp;
    uint32_t value;

    dprintf("Instruction: LDRT\n");

    addr10 = (int)ls_address.entire & 0x00000003;

    // Special cases
    // Verify coprocessor alignment

#ifdef UNALIGNED_ACCESS_SUPPORT
    value = MEM_read(ls_address.entire);
#else
    ls_address.entire &= 0xFFFFFFFC;
    switch(addr10) {
    case 0:
        value = MEM_read(ls_address.entire);
        RB_write(rd, value);
        break;
    case 1:
        tmp.entire = MEM_read(ls_address.entire);
        value = RotateRight(8,tmp).entire;
        RB_write(rd, value);
        break;
    case 2:
        tmp.entire = MEM_read(ls_address.entire);
        value = RotateRight(16,tmp).entire;
        RB_write(rd, value);
        break;
    default:
        tmp.entire = MEM_read(ls_address.entire);
        value = RotateRight(24, tmp).entire;
        RB_write(rd, value);
    }
#endif // UNALIGNED_ACCESS_SUPPORT

    dprintf("Reading memory position 0x%08X\n", ls_address.entire);
    dprintf(" *  R%d <= 0x%08X\n", rd, value);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------

inline void MLA(int rd, int rn, int rm, int rs, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2, RM2, RS2;
    RN2.entire = RB_read(rn);
    RM2.entire = RB_read(rm);
    RS2.entire = RB_read(rs);

    dprintf("Instruction: MLA\n");
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n  C = 0x%lX\n", RM2.entire, RS2.entire, RN2.entire);

    // Special cases
    // Since ARMv6, the behavior of MLA when Rd==Rm is defined.
    //    if((rd == PC)||(rm == PC)||(rs == PC)||(rn == PC)||(rd == rm)) {
    if((rd == PC)||(rm == PC)||(rs == PC)||(rn == PC)) {
        fprintf(stderr, "Unpredictable MLA instruction result\n");
        return;
    }

    RD2.entire = RM2.entire * RS2.entire + RN2.entire;
    if(s == 1) {
        flags.N = getBit(RD2.entire,31);
        flags.Z = ((RD2.entire == 0) ? true : false);
        // nothing happens with flags.C and flags.V
    }
    RB_write(rd,RD2.entire);

    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void MOV(int rd, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    dprintf("Instruction: MOV\n");
//    RB_write(rd, dpi_shiftop.entire);

    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable MOV instruction result\n");
            return;
        }
        SPSRtoCPSR();
    }
    if (s == 1){
        flags.N = getBit(dpi_shiftop.entire, 31);
        flags.Z = ((dpi_shiftop.entire == 0) ? true : false);
        flags.C = dpi_shiftopcarry;
        // nothing happens with flags.V
    }

    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, dpi_shiftop.entire, dpi_shiftop.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    RB_write(rd, dpi_shiftop.entire);
    ac_pc = RB_read(PC);
}


//------------------------------------------------------
inline void MOVT(int rd, bool s,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc) {

    //Concatenate imm16 with current low part of Rd
    uint32_t tmp = (unsigned) RB_read(rd) & 0xFFFF;
    dpi_shiftop.entire = (dpi_shiftop.entire << 16) | (unsigned) tmp;
    MOV(rd,s,RB,ac_pc);
}

//------------------------------------------------------
inline void MCR(unsigned cp_num,int funcc2,int funcc3, int crn,int crm,
                unsigned rd, ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    dprintf("Instruction: MCR: crn=%d crm=%d opc1=%d opc2=%d <= (R%d=0x%X)\n", crn, crm,
            funcc2, funcc3, rd, RB_read(rd) );
#ifdef SYSTEM_MODEL
    if(rd == PC) {
        fprintf(stderr, "Warning: MRC unpredictable behavior.\n");
        return;
    }

    if(CP[cp_num] == NULL){
        //This Coprocessor was not implemented.
        fprintf(stderr, "Warning Coprocessor cp%d not implemented in this model", cp_num);
        service_interrupt(*ref, arm_impl::EXCEPTION_UNDEFINED_INSTR);
        return;
    }
    uint32_t rd_val = RB_read(rd);

    //Call Coprocessor implementation of MCR
    (CP[cp_num])->MCR(ref, arm_proc_mode.getPrivilegeLevel(),
                      funcc2, funcc3, crn, crm, rd_val);

#else
    fprintf(stderr, "Warning Coprocessor simulation not implemented in this model");
#endif
}
//------------------------------------------------------
inline void MRC(unsigned cp_num,int funcc2,int funcc3, int crn,
                int crm,unsigned rd, ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {


    dprintf("Instruction: MRC: crn=%d crm=%d opc1=%d opc2=%d <= (R%d=0x%X)\n", crn, crm,
            funcc2, funcc3, rd, RB_read(rd) );
#ifdef SYSTEM_MODEL

    if(CP[cp_num] == NULL){
        //This Coprocessor was not implemented.
        fprintf(stderr, "Warning Coprocessor cp%d not implemented in this model", cp_num);
        service_interrupt(*ref, arm_impl::EXCEPTION_UNDEFINED_INSTR);
        return;
    }

    //Call Coprocessor implementation of MRC
    uint32_t cp_val = (CP[cp_num])->MRC(ref,arm_proc_mode.getPrivilegeLevel(),
                                        funcc2, funcc3, crn, crm);

    if(rd != PC)
        RB_write(rd,cp_val);
    else{
        flags.N = getBit(cp_val, 31);
        flags.Z = getBit(cp_val, 30);
        flags.C = getBit(cp_val, 29);
        flags.V = getBit(cp_val, 28);
        dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n", flags.N,flags.Z,flags.C,flags.V);
    }

#else
    fprintf(stderr, "Warning Coprocessor simulation not implemented in this model");
#endif
}
//------------------------------------------------------

inline void MRS(int rd, bool r, int zero3, int subop2, int func2, int subop1, int rm, int field,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    unsigned res;

    dprintf("Instruction: MRS\n");

    // Special cases
    if((rd == PC)||((zero3 != 0)&&(subop2 != 0)&&(func2 != 0)&&(subop1 != 0)&&(rm != 0))||(field != 15)) {
        printf("Unpredictable MRS instruction result\n");
        dprintf("**! Unpredictable MRS instruction result\n");
        return;
    }

    if (r == 1) {
        if (arm_proc_mode.getPrivilegeLevel() == arm_impl::PL0) {
            printf("Unpredictable MRS instruction result\n");
            dprintf("**! Unpredictable MRS instruction result\n");
            return;
        }
        res = readSPSR();
        dprintf("Read SPSR.\n");
    } else {
        res = readCPSR();
        dprintf("Read CPSR.\n");
    }

    RB_write(rd,res);

    dprintf(" *  R%d <= 0x%08X\n", rd, res);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void MUL(int rd, int rm, int rs, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RM2, RS2;
    RM2.entire = RB_read(rm);
    RS2.entire = RB_read(rs);

    dprintf("Instruction: MUL\n");
    dprintf("Operands:\n  A = 0x%lX  B = 0x%lX", RM2.entire, RS2.entire);

    //  Since ARMv6, the behavior of MUL instruction when Rd==Rm is well
    // defined.
    //    if((rd == PC)||(rm == PC)||(rs == PC)||(rd == rm)) {
    if((rd == PC)||(rm == PC)||(rs == PC)) {
        fprintf(stderr, "Unpredictable MUL instruction result\n");
//    return;
    }

    RD2.entire = RM2.entire * RS2.entire;
    if(s == 1) {
        flags.N = getBit(RD2.entire, 31);
        flags.Z = ((RD2.entire == 0) ? true : false);
        // nothing happens with flags.C and flags.V
    }
    RB_write(rd, RD2.entire);

    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void MLS(int rd, int ra, int rm, int rn,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    //Special cases
    if( rd == PC || ra == PC || rm == PC || rn == PC) {
        printf("Unpredictable MLS instruction result\n");
        return;
    }

    uint32_t aux = (unsigned) ( RB.read(ra) - (RB.read(rm) * RB.read(rn)) );
    RB_write(rd, aux);
}

//------------------------------------------------------
inline void MVN(int rd, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    dprintf("Instruction: MVN\n");
    RB_write(rd,~dpi_shiftop.entire);

    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable MVN instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(~dpi_shiftop.entire,31);
            flags.Z = ((~dpi_shiftop.entire == 0) ? true : false);
            flags.C = dpi_shiftopcarry;
            // nothing happens with flags.V
        }
    }

    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, ~dpi_shiftop.entire, ~dpi_shiftop.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void PKH(int rd, int rn, int rm, bool tb,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    uint32_t dest = 0;
    //Special case.
    if(rd == PC || rn == PC || rm == PC){
        printf("Unpredictable PKH instruction result\n");
        return;
    }

    //Write to register
    if(tb == true){
        dest |= (dpi_shiftop.entire & 0xFFFF); // High
        dest |= (RB.read(rn) & 0xFFFF0000);    // Low
    }
    else {
        dest |= (RB.read(rn) & 0xFFFF);            // High
        dest |= (dpi_shiftop.entire & 0xFFFF0000);  //Low
    }

    RB_write(rd, dest);
}

//------------------------------------------------------
inline void ORR(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2;

    dprintf("Instruction: ORR\n");
    RN2.entire = RB_read(rn);
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n", RN2.entire,dpi_shiftop.entire);
    RD2.entire = RN2.entire | dpi_shiftop.entire;
    RB_write(rd,RD2.entire);

    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable ORR instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire,31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = dpi_shiftopcarry;
            // nothing happens with flags.V
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void RSB(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2, neg_RN2;
    r64bit_t result;

    dprintf("Instruction: RSB\n");
    RN2.entire = RB_read(rn);
    if(rn == PC) RN2.entire += 4;
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n", RN2.entire,dpi_shiftop.entire);
    neg_RN2.entire = - RN2.entire;
    result.uhilo = (uint64_t)(uint32_t)dpi_shiftop.uentire + (uint64_t)(uint32_t)neg_RN2.uentire;
    RD2.entire = result.reg[0];
    RB_write(rd, RD2.entire);
    if ((s == 1) && (rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable RSB instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire,31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = !(((uint32_t) RN2.uentire > (uint32_t) dpi_shiftop.uentire) ? true : false);
            flags.V = (((getBit(neg_RN2.entire,31) && getBit(dpi_shiftop.entire,31) && (!getBit(RD2.entire,31))) ||
                        ((!getBit(neg_RN2.entire,31)) && (!getBit(dpi_shiftop.entire,31)) && getBit(RD2.entire,31))) ? true : false);
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void RSC(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2, neg_RN2;
    r64bit_t result;

    dprintf("Instruction: RSC\n");
    RN2.entire = RB_read(rn);
    if(rn == PC) RN2.entire += 4;
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n  Carry = %d\n", RN2.entire,dpi_shiftop.entire, flags.C);
    neg_RN2.entire = - RN2.entire;
    if (!flags.C) neg_RN2.entire--;
    result.uhilo = (uint64_t)(uint32_t)dpi_shiftop.uentire + (uint64_t)(uint32_t)neg_RN2.uentire;
    RD2.entire = result.reg[0];

    RB_write(rd, RD2.entire);
    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable RSC instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire,31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = !(((uint32_t) RN2.uentire > (uint32_t) dpi_shiftop.uentire) ? true : false);
            flags.V = (((getBit(neg_RN2.entire,31) && getBit(dpi_shiftop.entire,31) && (!getBit(RD2.entire,31))) ||
                        ((!getBit(neg_RN2.entire,31)) && (!getBit(dpi_shiftop.entire,31)) && getBit(RD2.entire,31))) ? true : false);
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void SBC(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2, neg_shiftop;
    r64bit_t result;

    dprintf("Instruction: SBC\n");
    RN2.entire = RB_read(rn);
    if(rn == PC) RN2.entire += 4;
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n  Carry = %d\n", RN2.entire,dpi_shiftop.entire, flags.C);
    neg_shiftop.entire = - dpi_shiftop.entire; 
    if (!flags.C) neg_shiftop.entire--;
    result.uhilo = (uint64_t)(uint32_t)RN2.uentire + (uint64_t)(uint32_t)neg_shiftop.uentire;
    RD2.entire = result.reg[0];
    RB_write(rd, RD2.entire);
    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable SBC instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire,31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = !(((uint32_t) dpi_shiftop.uentire > (uint32_t) RN2.uentire) ? true : false);
            flags.V = (((getBit(RN2.entire,31) && getBit(neg_shiftop.entire,31) && (!getBit(RD2.entire,31))) ||
                        ((!getBit(RN2.entire,31)) && (!getBit(neg_shiftop.entire,31)) && getBit(RD2.entire,31))) ? true : false);
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void SMLAL(int rdhi, int rdlo, int rm, int rs, bool s,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc) {

    r64bit_t result, acc;
    reg_t RM2, RS2;

    RM2.entire = RB_read(rm);
    RS2.entire = RB_read(rs);
    acc.reg[0] = RB_read(rdlo);
    acc.reg[1] = RB_read(rdhi);

    dprintf("Instruction: SMLAL\n");
    dprintf("Operands:\n  rm=0x%X, contains 0x%lX\n  rs=0x%X, contains 0x%lX\n  Add multiply result to %lld\nDestination(Hi): Rdhi=0x%X, Rdlo=0x%X\n", rm,RM2.entire,rs,RS2.entire,acc.hilo,rdhi,rdlo);

    // Special cases
    if((rdhi == PC)||(rdlo == PC)||(rm == PC)||(rs == PC)||(rdhi == rdlo)||(rdhi == rm)||(rdlo == rm)) {
        printf("Unpredictable SMLAL instruction result\n");
        return;
    }

    result.hilo = (int64_t)RM2.entire * (int64_t)RS2.entire + acc.hilo;
    RB_write(rdhi,result.reg[1]);
    RB_write(rdlo,result.reg[0]);
    if(s == 1){
        flags.N = getBit(result.reg[1],31);
        flags.Z = ((result.hilo == 0) ? true : false);
        // nothing happens with flags.C and flags.V
    }
    dprintf(" *  R%d(high) R%d(low) <= 0x%08X%08X (%d)\n", rdhi, rdlo, result.reg[1], result.reg[0], result.reg[0]);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void SMULL(int rdhi, int rdlo, int rm, int rs, bool s,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc) {

    r64bit_t result;
    reg_t RM2, RS2;

    RM2.entire = RB_read(rm);
    RS2.entire = RB_read(rs);

    dprintf("Instruction: SMULL\n");
    dprintf("Operands:\n  rm=0x%X, contains 0x%lX\n  rs=0x%X, contains 0x%lX\n  Destination(Hi): Rdhi=0x%X, Rdlo=0x%X\n", rm,RM2.entire,rs,RS2.entire,rdhi,rdlo);

    //  Since ARMv6, the behavior of SMULL instruction when rdhi==rm and rdlo=rm is well
    // defined.
    //    if((rdhi == PC)||(rdlo == PC)||(rm == PC)||(rs == PC)||(rdhi == rdlo)||(rdhi == rm)||(rdlo == rm)) {
    if((rdhi == PC)||(rdlo == PC)||(rm == PC)||(rs == PC)||(rdhi == rdlo)) {
        printf("Unpredictable SMULL instruction result\n");
        return;
    }

    result.hilo = (int64_t)RM2.entire * (int64_t)RS2.entire;
    RB_write(rdhi,result.reg[1]);
    RB_write(rdlo,result.reg[0]);
    if(s == 1){
        flags.N = getBit(result.reg[1],31);
        flags.Z = ((result.hilo == 0) ? true : false);
        // nothing happens with flags.C and flags.V
    }
    dprintf(" *  R%d(high) R%d(low) <= 0x%08X%08X (%d)\n", rdhi, rdlo, result.reg[1], result.reg[0], result.reg[0]);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void STC(){
    dprintf("Instruction: STC\n");
    fprintf(stderr,"Warning: STC instruction is not implemented in this model.\n");
}

//------------------------------------------------------
inline void STM(int rlist,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc, ac_memory& MEM, unsigned r) {

    // todo special cases

    int i;

    if (r == 0) { // STM(1)
        dprintf("Instruction: STM\n");
        ls_address = lsm_startaddress;
        for(i=0;i<16;i++){
            if(isBitSet(rlist,i)) {
                MEM_write(ls_address.entire,RB_read(i));
                ls_address.entire += 4;
                dprintf(" *  Stored register: 0x%X; value: 0x%X; address: 0x%lX\n",i,RB_read(i),ls_address.entire-4);
            }
        }
    } else { // STM(2)
        if (arm_proc_mode.getPrivilegeLevel() == arm_impl::PL0) {
            fprintf(stderr, "STM(2) unpredictable in user mode");
            abort();
        }
        dprintf("Instruction: STM(2)\n");
        ls_address = lsm_startaddress;
        for(i=0;i<16;i++){
            if(isBitSet(rlist,i)) {
                MEM_write(ls_address.entire,RB .read(i));
                ls_address.entire += 4;
                dprintf(" *  Stored register: 0x%X; value: 0x%X; address: 0x%lX\n",i,RB_read(i),ls_address.entire-4);
            }
        }
    }

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void STR(int rd, int rn,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    dprintf("Instruction: STR\n");

    // Special cases
    // verify coprocessor alignment

    MEM_write(ls_address.entire, RB_read(rd));

    dprintf(" *  MEM[0x%08X] <= 0x%08X\n", ls_address.entire, RB_read(rd));

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void STRB(int rd, int rn,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    reg_t RD2;

    dprintf("Instruction: STRB\n");

    // Special cases

    RD2.entire = RB_read(rd);
    MEM_write_byte(ls_address.entire, RD2.byte[0]);

    dprintf(" *  MEM[0x%08X] <= 0x%02X\n", ls_address.entire, RD2.byte[0]);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void STRBT(int rd, int rn,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    reg_t RD2;

    dprintf("Instruction: STRBT\n");

    // Special cases

    RD2.entire = RB_read(rd);
    MEM_write_byte(ls_address.entire, RD2.byte[0]);

    dprintf(" *  MEM[0x%08X] <= 0x%02X\n", ls_address.entire, RD2.byte[0]);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void STRD(int rd, int rn,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    dprintf("Instruction: STRD\n");

    // Special cases
    // Destination register must be even
    if(isBitSet(rd,0)){
        printf("Undefined STRD instruction result (Rd must be even)\n");
        return;
    }
    // Check doubleword alignment
    if((rd == LR)||(ls_address.entire & 0x00000007)){
        printf("Unpredictable STRD instruction result (Address is not doubleword aligned)\n");
        return;
    }

    //FIXME: Check if writeback receives +4 from second address
    MEM_write(ls_address.entire,RB_read(rd));
    MEM_write(ls_address.entire+4,RB_read(rd+1));

    dprintf(" *  MEM[0x%08X], MEM[0x%08X] <= 0x%08X %08X\n", ls_address.entire, ls_address.entire+4, RB_read(rd+1), RB_read(rd));

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void STRH(int rd, int rn,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    int16_t data;

    dprintf("Instruction: STRH\n");

#ifdef UNALIGNED_ACCESS_SUPPORT
    // Special cases
    // verify coprocessor alignment
    // verify halfword alignment
    if(isBitSet(ls_address.entire,0)){
        printf("Unpredictable STRH instruction result (Address is not halfword aligned)\n");
        return;
    }
#endif

    data = (int16_t) (RB_read(rd) & 0x0000FFFF);
    MEM_write_half(ls_address.entire, data);

    dprintf(" *  MEM[0x%08X] <= 0x%04X\n", ls_address.entire, data);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void STRT(int rd, int rn,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    dprintf("Instruction: STRT\n");

    // Special cases
    // verificar caso do coprocessador (alinhamento)

    MEM_write(ls_address.entire, RB_read(rd));

    dprintf(" *  MEM[0x%08X] <= 0x%08X\n", ls_address.entire, RB_read(rd));

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void SUB(int rd, int rn, bool s,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2, neg_shiftop;
    r64bit_t result;

    dprintf("Instruction: SUB\n");
    RN2.entire = RB_read(rn);
    if(rn == PC) RN2.entire += 4;

    dprintf("Operands:\n", RN2.entire,dpi_shiftop.entire);
    dprintf("A = 0x%lX\n", RN2.entire);
    dprintf("B = 0x%lX\n", dpi_shiftop.entire);

    neg_shiftop.entire = - dpi_shiftop.entire;
    result.uhilo = (uint64_t)(uint32_t)RN2.uentire + (uint64_t)(uint32_t)neg_shiftop.uentire;
    RD2.entire = result.reg[0];
    RB_write(rd, RD2.entire);
    if ((s == 1)&&(rd == PC)) {
        if (arm_proc_mode.mode == arm_impl::processor_mode::USER_MODE ||
            arm_proc_mode.mode == arm_impl::processor_mode::SYSTEM_MODE) {
            printf("Unpredictable SUB instruction result\n");
            return;
        }
        SPSRtoCPSR();
    } else {
        if (s == 1) {
            flags.N = getBit(RD2.entire,31);
            flags.Z = ((RD2.entire == 0) ? true : false);
            flags.C = !(((uint32_t) dpi_shiftop.uentire > (uint32_t) RN2.uentire) ? true : false);
            flags.V = (((getBit(RN2.entire,31) && getBit(neg_shiftop.entire,31) && (!getBit(RD2.entire,31))) ||
                        ((!getBit(RN2.entire,31)) && (!getBit(neg_shiftop.entire,31)) && getBit(RD2.entire,31))) ? true : false);
        }
    }
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);

}

//------------------------------------------------------
inline void SWP(int rd, int rn, int rm,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    reg_t RN2, RM2, rtmp;
    int32_t tmp;
    int rn10;

    dprintf("Instruction: SWP\n");

    // Special cases
    // verify coprocessor alignment
    if((rd == PC)||(rm == PC)||(rn == PC)||(rm == rn)||(rn == rd)){
        printf("Unpredictable SWP instruction result\n");
        return;
    }

    RN2.entire = RB_read(rn);
    RM2.entire = RB_read(rm);
    rn10 = RN2.entire & 0x00000003;

    switch(rn10) {
    case 0:
        tmp = MEM_read(RN2.entire);
        break;
    case 1:
        rtmp.entire = MEM_read(RN2.entire);
        tmp = (RotateRight(8,rtmp)).entire;
        break;
    case 2:
        rtmp.entire = MEM_read(RN2.entire);
        tmp = (RotateRight(16,rtmp)).entire;
        break;
    default:
        rtmp.entire = MEM_read(RN2.entire);
        tmp = (RotateRight(24,rtmp)).entire;
    }

    MEM_write(RN2.entire,RM2.entire);
    RB_write(rd,tmp);

    dprintf(" *  MEM[0x%08X] <= 0x%08X (%d)\n", RN2.entire, RM2.entire, RM2.entire);
    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, tmp, tmp);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void SWPB(int rd, int rn, int rm,
                 ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                 ac_reg<unsigned>& ac_pc, ac_memory& MEM) {

    uint32_t tmp;
    reg_t RM2, RN2;

    dprintf("Instruction: SWPB\n");

    // Special cases
    if((rd == PC)||(rm == PC)||(rn == PC)||(rm == rn)||(rn == rd)){
        printf("Unpredictable SWPB instruction result\n");
        return;
    }

    RM2.entire = RB_read(rm);
    RN2.entire = RB_read(rn);

    tmp = (uint32_t) MEM_read_byte(RN2.entire);
    MEM_write_byte(RN2.entire,RM2.byte[0]);
    RB_write(rd,tmp);

    dprintf(" *  MEM[0x%08X] <= 0x%02X (%d)\n", RN2.entire, RM2.byte[0], RM2.byte[0]);
    dprintf(" *  R%d <= 0x%02X (%d)\n", rd, tmp, tmp);

    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void TEQ(int rn,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RN2, alu_out;

    dprintf("Instruction: TEQ\n");
    RN2.entire = RB_read(rn);
    dprintf("Operands:\n  A = 0x%lX\n  B = 0x%lX\n", RN2.entire,dpi_shiftop.entire);
    alu_out.entire = RN2.entire ^ dpi_shiftop.entire;

    flags.N = getBit(alu_out.entire,31);
    flags.Z = ((alu_out.entire == 0) ? true : false);
    flags.C = dpi_shiftopcarry;
    // nothing happens with flags.V

    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void TST(int rn,
                ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                ac_reg<unsigned>& ac_pc) {

    reg_t RN2, alu_out;

    dprintf("Instruction: TST\n");
    RN2.entire = RB_read(rn);
    dprintf("Operands:\n");
    dprintf("A = 0x%lX\n", RN2.entire);
    dprintf("B = 0x%lX\n", dpi_shiftop.entire);
    alu_out.entire = RN2.entire & dpi_shiftop.entire;

    flags.N = getBit(alu_out.entire, 31);
    flags.Z = ((alu_out.entire == 0) ? true : false);
    flags.C = dpi_shiftopcarry;
    // nothing happens with flags.V

    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void UMLAL(int rdhi, int rdlo, int rm, int rs, bool s,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc) {

    r64bit_t result, acc;
    reg_t RM2, RS2;

    RM2.entire = RB_read(rm);
    RS2.entire = RB_read(rs);
    acc.reg[0] = RB_read(rdlo);
    acc.reg[1] = RB_read(rdhi);

    dprintf("Instruction: UMLAL\n");
    dprintf("Operands:\n  rm=0x%X, contains 0x%lX\n  rs=0x%X, contains 0x%lX\n  Add multiply result to %lld\nDestination(Hi): Rdhi=0x%X, Rdlo=0x%X\n", rm,RM2.entire,rs,RS2.entire,acc.hilo,rdhi,rdlo);

    // Special cases
    if((rdhi == PC)||(rdlo == PC)||(rm == PC)||(rs == PC)||(rdhi == rdlo)||(rdhi == rm)||(rdlo == rm)) {
        printf("Unpredictable UMLAL instruction result\n");
        return;
    }

    result.uhilo = (uint64_t)(uint32_t)RM2.uentire * (uint64_t)(uint32_t)RS2.uentire
        + (uint64_t)acc.uhilo;
    RB_write(rdhi,result.reg[1]);
    RB_write(rdlo,result.reg[0]);
    if(s == 1){
        flags.N = getBit(result.reg[1],31);
        flags.Z = ((result.hilo == 0) ? true : false);
        // nothing happens with flags.C and flags.V
    }

    dprintf(" *  R%d(high) R%d(low) <= 0x%08X%08X (%d)\n", rdhi, rdlo, result.reg[1], result.reg[0], result.reg[0]);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void UMULL(int rdhi, int rdlo, int rm, int rs, bool s,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc) {


    r64bit_t result;
    reg_t RM2, RS2;

    RM2.entire = RB_read(rm);
    RS2.entire = RB_read(rs);

    dprintf("Instruction: UMULL\n");
    dprintf("Operands:\n  rm=0x%X, contains 0x%lX\n  rs=0x%X, contains 0x%lX\n  Destination(Hi): Rdhi=0x%X, Rdlo=0x%X\n", rm,RM2.entire,rs,RS2.entire,rdhi,rdlo);

    //  Since ARMv6, the behavior of UMULL instruction when rdhi==rm and rdlo=rm is well
    // defined.
    //    if((rdhi == PC)||(rdlo == PC)||(rm == PC)||(rs == PC)||(rdhi == rdlo)||(rdhi == rm)||(rdlo == rm)) {
    if((rdhi == PC)||(rdlo == PC)||(rm == PC)||(rs == PC)||(rdhi == rdlo)) {
        printf("Unpredictable UMULL instruction result\n");
        return;
    }

    result.uhilo = (uint64_t)(uint32_t)RM2.uentire * (uint64_t)(uint32_t)RS2.uentire;
    RB_write(rdhi,result.reg[1]);
    RB_write(rdlo,result.reg[0]);
    if(s == 1){
        flags.N = getBit(result.reg[1],31);
        flags.Z = ((result.hilo == 0) ? true : false);
        // nothing happens with flags.C and flags.V
    }
    dprintf(" *  R%d(high) R%d(low) <= 0x%08X%08X (%d)\n", rdhi, rdlo, result.reg[1], result.reg[0], result.reg[0]);
    dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n",flags.N,flags.Z,flags.C,flags.V);
    ac_pc = RB_read(PC);
}

//------------------------------------------------------
inline void DSMLA(int rd, int rn,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc) {

    reg_t RD2, RN2;

    RN2.entire = RB_read(rn);

    dprintf("Instruction: SMLA<y><x>\n");
    dprintf("Operands:\n  rn=0x%X, contains 0x%lX\n  first operand contains 0x%lX\n  second operand contains 0x%lX\n  rd=0x%X, contains 0x%lX\n", rn, RN2.entire, OP1.entire, OP2.entire, rd, RD2.entire);

    RD2.entire = (OP1.entire * OP2.entire) + RN2.entire;

    RB_write(rd, RD2.entire);

    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);

    // SET Q FLAG
}

//------------------------------------------------------
inline void DSMUL(int rd,
                  ac_regbank<31, arm_parms::ac_word, arm_parms::ac_Dword>& RB,
                  ac_reg<unsigned>& ac_pc) {

    reg_t RD2;

    dprintf("Instruction: SMUL<y><x>\n");
    dprintf("Operands:\n  first operand contains 0x%lX\n  second operand contains 0x%lX\n  rd=0x%X, contains 0x%lX\n", OP1.entire, OP2.entire, rd, RD2.entire);

    RD2.entire = OP1.entire * OP2.entire;

    RB_write(rd, RD2.entire);

    dprintf(" *  R%d <= 0x%08X (%d)\n", rd, RD2.entire, RD2.entire);

    // SET Q FLAG
}

//------------------------------------------------------


//!Instruction and1 behavior method.
void ac_behavior( and1 ){ AND(rd, rn, s, RB, ac_pc);}

//!Instruction eor1 behavior method.
void ac_behavior( eor1 ){ EOR(rd, rn, s, RB, ac_pc);}

//!Instruction sub1 behavior method.
void ac_behavior( sub1 ){ SUB(rd, rn, s, RB, ac_pc);}

//!Instruction rsb1 behavior method.
void ac_behavior( rsb1 ){ RSB(rd, rn, s, RB, ac_pc);}

//!Instruction add1 behavior method.
void ac_behavior( add1 ){ ADD(rd, rn, s, RB, ac_pc);}

//!Instruction adc1 behavior method.
void ac_behavior( adc1 ){ ADC(rd, rn, s, RB, ac_pc);}

//!Instruction sbc1 behavior method.
void ac_behavior( sbc1 ){ SBC(rd, rn, s, RB, ac_pc);}

//!Instruction rsc1 behavior method.
void ac_behavior( rsc1 ){ RSC(rd, rn, s, RB, ac_pc);}

//!Instruction tst1 behavior method.
void ac_behavior( tst1 ){ TST(rn, RB, ac_pc);}

//!Instruction teq1 behavior method.
void ac_behavior( teq1 ){ TEQ(rn, RB, ac_pc);}

//!Instruction cmp1 behavior method.
void ac_behavior( cmp1 ){ CMP(rn, RB, ac_pc);}

//!Instruction cmn1 behavior method.
void ac_behavior( cmn1 ){ CMN(rn, RB, ac_pc);}

//!Instruction orr1 behavior method.
void ac_behavior( orr1 ){ ORR(rd, rn, s, RB, ac_pc);}

//!Instruction mov1 behavior method.
void ac_behavior( mov1 ){ MOV(rd, s, RB, ac_pc);}

//!Instruction bic1 behavior method.
void ac_behavior( bic1 ){ BIC(rd, rn, s, RB, ac_pc);}

//!Instruction mvn1 behavior method.
void ac_behavior( mvn1 ){ MVN(rd, s, RB, ac_pc);}

//!Instruction and2 behavior method.
void ac_behavior( and2 ){ AND(rd, rn, s, RB, ac_pc);}

//!Instruction eor2 behavior method.
void ac_behavior( eor2 ){ EOR(rd, rn, s, RB, ac_pc);}

//!Instruction sub2 behavior method.
void ac_behavior( sub2 ){ SUB(rd, rn, s, RB, ac_pc);}

//!Instruction rsb2 behavior method.
void ac_behavior( rsb2 ){ RSB(rd, rn, s, RB, ac_pc);}

//!Instruction add2 behavior method.
void ac_behavior( add2 ){ ADD(rd, rn, s, RB, ac_pc);}

//!Instruction adc2 behavior method.
void ac_behavior( adc2 ){ ADC(rd, rn, s, RB, ac_pc);}

//!Instruction sbc2 behavior method.
void ac_behavior( sbc2 ){ SBC(rd, rn, s, RB, ac_pc);}

//!Instruction rsc2 behavior method.
void ac_behavior( rsc2 ){ RSC(rd, rn, s, RB, ac_pc);}

//!Instruction tst2 behavior method.
void ac_behavior( tst2 ){ TST(rn, RB, ac_pc);}

//!Instruction teq2 behavior method.
void ac_behavior( teq2 ){ TEQ(rn, RB, ac_pc);}

//!Instruction cmp2 behavior method.
void ac_behavior( cmp2 ){ CMP(rn, RB, ac_pc);}

//!Instruction cmn2 behavior method.
void ac_behavior( cmn2 ){ CMN(rn, RB, ac_pc);}

//!Instruction orr2 behavior method.
void ac_behavior( orr2 ){ ORR(rd, rn, s, RB, ac_pc);}

//!Instruction mov2 behavior method.
void ac_behavior( mov2 ){ MOV(rd, s, RB, ac_pc);}

//!Instruction bic2 behavior method.
void ac_behavior( bic2 ){ BIC(rd, rn, s, RB, ac_pc);}

//!Instruction mvn2 behavior method.
void ac_behavior( mvn2 ){ MVN(rd, s, RB, ac_pc);}

//!Instruction and3 behavior method.
void ac_behavior( and3 ){ AND(rd, rn, s, RB, ac_pc);}

//!Instruction eor3 behavior method.
void ac_behavior( eor3 ){ EOR(rd, rn, s, RB, ac_pc);}

//!Instruction sub3 behavior method.
void ac_behavior( sub3 ){ SUB(rd, rn, s, RB, ac_pc);}

//!Instruction rsb3 behavior method.
void ac_behavior( rsb3 ){ RSB(rd, rn, s, RB, ac_pc);}

//!Instruction add3 behavior method.
void ac_behavior( add3 ){ ADD(rd, rn, s, RB, ac_pc);}

//!Instruction adc3 behavior method.
void ac_behavior( adc3 ){ ADC(rd, rn, s, RB, ac_pc);}

//!Instruction sbc3 behavior method.
void ac_behavior( sbc3 ){ SBC(rd, rn, s, RB, ac_pc);}

//!Instruction rsc3 behavior method.
void ac_behavior( rsc3 ){ RSC(rd, rn, s, RB, ac_pc);}

//!Instruction tst3 behavior method.
void ac_behavior( tst3 ){ TST(rn, RB, ac_pc);}

//!Instruction teq3 behavior method.
void ac_behavior( teq3 ){ TEQ(rn, RB, ac_pc);}

//!Instruction cmp3 behavior method.
void ac_behavior( cmp3 ){ CMP(rn, RB, ac_pc);}

//!Instruction cmn3 behavior method.
void ac_behavior( cmn3 ){ CMN(rn, RB, ac_pc);}

//!Instruction orr3 behavior method.
void ac_behavior( orr3 ){ ORR(rd, rn, s, RB, ac_pc);}

//!Instruction mov3 behavior method.
void ac_behavior( mov3 ){ MOV(rd, s, RB, ac_pc);}

//Instruction mov4 behavior method.
void ac_behavior( mov4 ){ MOV(rd, s, RB, ac_pc);}

//Instruction movt behaviour method.
void ac_behavior( movt ) { MOVT(rd,s,RB,ac_pc);}

//!Instruction bic3 behavior method.
void ac_behavior( bic3 ){ BIC(rd, rn, s, RB, ac_pc);}

//!Instruction mvn3 behavior method.
void ac_behavior( mvn3 ){ MVN(rd, s, RB, ac_pc);}

//!Instruction b behavior method.
void ac_behavior( b ){ B(h, offset, RB, ac_pc);}

//!Instruction blx1 behavior method.
void ac_behavior( blx1 ){
    fprintf(stderr,"Warning: BLX instruction is not implemented in this model. PC=%X\n", ac_pc.read());
}

//!Instruction bx behavior method.
void ac_behavior( bx ){ BX(rm, RB, ac_pc); }

//!Instruction blx2 behavior method.
void ac_behavior( blx2 ){
    reg_t dest;
    dest.entire = RB_read(rm);
    dprintf("Instruction: BLX2\n");
    dprintf("Branch to contents of reg: 0x%X\n", rm);
    dprintf("Contents of register: 0x%lX\n", dest.entire);
    // Note that PC is already incremented by 4, i.e., pointing to the next instruction
    if(isBitSet(dest.entire,0)) {
        fprintf(stderr,"Change to thumb not implemented in this model. PC=%X\n", ac_pc.read());
        exit(EXIT_FAILURE);
    }

    RB_write(LR, RB_read(PC));
    dprintf("Branch return address: 0x%lX\n", RB_read(LR));

    flags.T = isBitSet(rm, 0);
    RB_write(PC, dest.entire & 0xFFFFFFFE);
    ac_pc = RB_read(PC);

    dprintf("Calculated branch destination: 0x%lX\n", RB_read(PC));
}

//!Instruction swp behavior method.
void ac_behavior( swp ){ SWP(rd, rn, rm, RB, ac_pc, MEM); }

//!Instruction swpb behavior method.
void ac_behavior( swpb ){ SWPB(rd, rn, rm, RB, ac_pc, MEM); }

//!Instruction mla behavior method.
void ac_behavior( mla ){ MLA(rn, rd, rm, rs, s, RB, ac_pc);}
// OBS: inversao dos parametros proposital ("fields with the same name...")

//!Instruction mul behavior method.
void ac_behavior( mul ){ MUL(rn, rm, rs, s, RB, ac_pc);}
// OBS: inversao dos parametros proposital ("fields with the same name...")

void ac_behavior ( mls ) { MLS(rn, rd, rs,rm, RB,ac_pc); }
// OBS: inversao dos parametros proposital ("fields with the same name...")

//!Instruction smlal behavior method.
void ac_behavior( smlal ){ SMLAL(rdhi, rdlo, rm, rs, s, RB, ac_pc);}

//!Instruction smull behavior method.
void ac_behavior( smull ){ SMULL(rdhi, rdlo, rm, rs, s, RB, ac_pc);}

//!Instruction umlal behavior method.
void ac_behavior( umlal ){ UMLAL(rdhi, rdlo, rm, rs, s, RB, ac_pc);}

//!Instruction umull behavior method.
void ac_behavior( umull ){ UMULL(rdhi, rdlo, rm, rs, s, RB, ac_pc);}

//!Instruction ldr1 behavior method.
void ac_behavior( ldr1 ){ LDR(rd, rn, RB, ac_pc, MEM);  }

//!Instruction ldrt1 behavior method.
void ac_behavior( ldrt1 ){ LDRT(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldrb1 behavior method.
void ac_behavior( ldrb1 ){ LDRB(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldrbt1 behavior method.
void ac_behavior( ldrbt1 ){ LDRBT(rd, rn, RB, ac_pc, MEM); }

//!Instruction str1 behavior method.
void ac_behavior( str1 ){ STR(rd, rn, RB, ac_pc, MEM); }

//!Instruction strt1 behavior method.
void ac_behavior( strt1 ){ STRT(rd, rn, RB, ac_pc, MEM); }

//!Instruction strb1 behavior method.
void ac_behavior( strb1 ){ STRB(rd, rn, RB, ac_pc, MEM); }

//!Instruction strbt1 behavior method.
void ac_behavior( strbt1 ){ STRBT(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldr2 behavior method.
void ac_behavior( ldr2 ){ LDR(rd, rn, RB, ac_pc, MEM); }

void ac_behavior( ldrex ){
  ls_address.entire = RB_read(rn);
  LDR(rd, rn, RB, ac_pc, MEM);
}

//!Instruction ldrt2 behavior method.
void ac_behavior( ldrt2 ){ LDRT(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldrb2 behavior method.
void ac_behavior( ldrb2 ){ LDRB(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldrbt2 behavior method.
void ac_behavior( ldrbt2 ){ LDRBT(rd, rn, RB, ac_pc, MEM); }

//!Instruction str2 behavior method.
void ac_behavior( str2 ){ STR(rd, rn, RB, ac_pc, MEM); }

void ac_behavior( strex ){
  ls_address.entire = RB_read(rn);
  STR(rt, rn, RB, ac_pc, MEM);
  RB_write(rd, 0);
  fprintf(stderr, "arm: Using STREX dummy instruction. Might have sync issues.\n");
}

//!Instruction strt2 behavior method.
void ac_behavior( strt2 ){ STRT(rd, rn, RB, ac_pc, MEM); }

//!Instruction strb2 behavior method.
void ac_behavior( strb2 ){ STRB(rd, rn, RB, ac_pc, MEM); }

//!Instruction strbt2 behavior method.
void ac_behavior( strbt2 ){ STRBT(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldrh behavior method.
void ac_behavior( ldrh ){ LDRH(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldrsb behavior method.
void ac_behavior( ldrsb ){ LDRSB(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldrsh behavior method.
void ac_behavior( ldrsh ){ LDRSH(rd, rn, RB, ac_pc, MEM); }

//!Instruction strh behavior method.
void ac_behavior( strh ){ STRH(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldm behavior method.
void ac_behavior( ldm ){ LDM(rlist,r, RB, ac_pc, MEM); }

//!Instruction stm behavior method.
void ac_behavior( stm ){ STM(rlist, RB, ac_pc, MEM, r); }

//!Instruction cdp behavior method.
void ac_behavior( cdp ){ CDP();}

//!Instruction ldc behavior method.
void ac_behavior( ldc ){ LDC();}

//!Instruction stc behavior method.
void ac_behavior( stc ){ STC();}

//!Instruction bkpt behavior method.
void ac_behavior( bkpt ){
    fprintf(stderr,"Warning: BKPT instruction is not implemented in this model. PC=%X\n", ac_pc.read());
}

//!Instruction swi behavior method.
void ac_behavior( swi ){
#ifdef SYSTEM_MODEL
    service_interrupt(*this, arm_impl::EXCEPTION_SWI);
#else
    dprintf("Instruction: SWI\n");
    if (swinumber == 0) {
        // New ABI (EABI), expected syscall number is in r7
        unsigned sysnum = RB_read(7) + 0x900000;
        dprintf("EABI Syscall number: 0x%X\t(%d)\n", RB_read(7), RB_read(7));
        if (syscall.process_syscall(sysnum) == -1) {
            fprintf(stderr, "Warning: A syscall not implemented in this model was called.\n\tCaller address: 0x%X\n\tSyscall number: 0x%X\t%d\n", (unsigned int)ac_pc, sysnum, sysnum);
        }
        // Old ABI (syscall encoded in instruction)
    } else {
        dprintf("Syscall number: 0x%X\t%d\n", swinumber, swinumber);
        if (syscall.process_syscall(swinumber) == -1) {
            fprintf(stderr, "Warning: A syscall not implemented in this model was called.\n\tCaller address: 0x%X\n\tSWI number: 0x%X\t(%d)\n", (unsigned int)ac_pc, swinumber, swinumber);
        }
    }
#endif
}

//!Instruction clz behavior method.
void ac_behavior( clz ){ CLZ(rd, rm, RB, ac_pc);}

//!Instruction mrs behavior method.
void ac_behavior( mrs ){ MRS(rd,r,zero3,subop2,func2,subop1,rm,fieldmask, RB, ac_pc);}

//!Instruction msr1 behavior method.
void ac_behavior( msr1 ){
    unsigned in = RB_read(rm);
    unsigned res;

    dprintf("Instruction: MSR\n");
    // Write to CPSR
    if (r == 0)  {
        res = readCPSR();
        if (arm_proc_mode.getPrivilegeLevel() == arm_impl::PL1) {
            if (fieldmask & 1) {
                res &= ~0xFF;
                res |= (in & 0xFF);
            }
            if (fieldmask & (1 << 1)) {
                res &= ~0xFF00;
                res |= (in & 0xFF00);
            }
            if (fieldmask & (1 << 2)) {
                res &= ~0xFF0000;
                res |= (in & 0xFF0000);
            }
        }
        if (fieldmask & (1 << 3)) {
            res &= ~0xFF000000;
            res |= (in & 0xFF000000);
        }
        writeCPSR(res);
        dprintf(" *  CPSR <= 0x%08X\n", res);
        dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n     FIQ disable"
                "=0x%X, IRQ disable=0x%X, Thumb=0x%X\n",flags.N,flags.Z,
                flags.C,flags.V, arm_proc_mode.fiq, arm_proc_mode.irq,
                arm_proc_mode.thumb);
        dprintf(" *  Processor mode <= %s MODE\n", arm_proc_mode.currentMode_str());
    } else { // r == 1, write to SPSR
        res = readSPSR();
        if (fieldmask & 1) {
            res &= ~0xFF;
            res |= (in & 0xFF);
        }
        if (fieldmask & (1 << 1)) {
            res &= ~0xFF00;
            res |= (in & 0xFF00);
        }
        if (fieldmask & (1 << 2)) {
            res &= ~0xFF0000;
            res |= (in & 0xFF0000);
        }
        if (fieldmask & (1 << 3)) {
            res &= ~0xFF000000;
            res |= (in & 0xFF000000);
        }
        writeSPSR(res);
        dprintf(" *  SPSR <= 0x%08X\n", res);
    }
}

//!Instruction msr2 behavior method.
void ac_behavior( msr2 ){
    reg_t temp;
    temp.entire = imm8;
    unsigned in = RotateRight(rotate*2, temp).entire;
    unsigned res;
    dprintf("Instruction: MSR\n");
    // Write to CPSR
    if (r == 0)  {
        res = readCPSR();
        if (arm_proc_mode.getPrivilegeLevel() == arm_impl::PL1) {
            if (fieldmask & 1) {
                res &= ~0xFF;
                res |= (in & 0xFF);
            }
            if (fieldmask & (1 << 1)) {
                res &= ~0xFF00;
                res |= (in & 0xFF00);
            }
            if (fieldmask & (1 << 2)) {
                res &= ~0xFF0000;
                res |= (in & 0xFF0000);
            }
        }
        if (fieldmask & (1 << 3)) {
            res &= ~0xFF000000;
            res |= (in & 0xFF000000);
        }
        writeCPSR(res);
        dprintf(" *  CPSR <= 0x%08X\n", res);
        dprintf(" *  Flags <= N=0x%X, Z=0x%X, C=0x%X, V=0x%X\n     FIQ disable"
                "=0x%X, IRQ disable=0x%X, Thumb=0x%X\n",flags.N,flags.Z,
                flags.C,flags.V, arm_proc_mode.fiq, arm_proc_mode.irq,
                arm_proc_mode.thumb);
        dprintf(" *  Processor mode <= %s MODE\n", arm_proc_mode.currentMode_str());
    } else { // r == 1, write to SPSR
        res = readSPSR();
        if (fieldmask & 1) {
            res &= ~0xFF;
            res |= (in & 0xFF);
        }
        if (fieldmask & (1 << 1)) {
            res &= ~0xFF00;
            res |= (in & 0xFF00);
        }
        if (fieldmask & (1 << 2)) {
            res &= ~0xFF0000;
            res |= (in & 0xFF0000);
        }
        if (fieldmask & (1 << 3)) {
            res &= ~0xFF000000;
            res |= (in & 0xFF000000);
        }
        writeSPSR(res);
        dprintf(" *  SPSR <= 0x%08X\n", res);
    }
}

//!Instruction ldrd2 behavior method.
void ac_behavior( ldrd ){ LDRD(rd, rn, RB, ac_pc, MEM); }

//!Instruction ldrd2 behavior method.
void ac_behavior( strd ){ STRD(rd, rn, RB, ac_pc, MEM); }

//!Instruction dsmla behavior method.
void ac_behavior( dsmla ){ DSMLA(drd, drn, RB, ac_pc); }

//!Instruction dsmlal behavior method.
void ac_behavior( dsmlal ){
    fprintf(stderr,"Warning: SMLAL<y><x> instruction is not implemented in this model. PC=%X\n", ac_pc.read());
}

//!Instruction dsmul behavior method.
void ac_behavior( dsmul ){ DSMUL(drd, RB, ac_pc); }

//!Instruction dsmlaw behavior method.
void ac_behavior( dsmlaw ){
    fprintf(stderr,"Warning: SMLAW<y><x> instruction is not implemented in this model. PC=%X\n", ac_pc.read());
}

//!Instruction dsmulw behavior method.
void ac_behavior( dsmulw ){
    fprintf(stderr,"Warning: SMULW<y><x> instruction is not implemented in this model. PC=%X\n", ac_pc.read());
}

//!Instruction bfi/bfc behavior method.
void ac_behavior( bfi ){ BFI(rd, rn, lsb, msb, RB, ac_pc); }

//! Instruction nop behavior method
void ac_behavior( nop ){/*Nothing to do here.*/}

//! instruction PKH
void ac_behavior( pkh ) { PKH(rd, rn, rm, tb, RB, ac_pc ); }

void ac_behavior( end ) { }

//Coprocessor Generic CPxx implementation
//! instruction MCR
void ac_behavior( mcr ) { MCR(cp_num, funcc2, funcc3, crn,crm, rd,RB,ac_pc); }

//! instruction MRC
void ac_behavior( mrc ) { MRC(cp_num, funcc2, funcc3, crn,crm, rd,RB,ac_pc); }

//! instruction SMC
void ac_behavior( smc ) {
  fprintf(stderr, "Warning: SMC instruction in not implemented in this model.  PC=%X\n", ac_pc.read());
}

//! instruction UBFX
void ac_behavior( ubfx ) { UBFX(rd, rn, lsb, widthm1+1, RB, ac_pc); }

//! instruction SBFX
void ac_behavior( sbfx ) { SBFX(rd, rn, lsb, widthm1+1, RB, ac_pc); }


//! instruction UXTB
void ac_behavior( uxtb ) { UXTB(rd, rm, RB, ac_pc); }

//! instruction UXTH
void ac_behavior( uxth ) { UXTH(rd, rm, RB, ac_pc); }

//! instruction SXTH
void ac_behavior( sxth ) { SXTH(rd, rm, RB, ac_pc); }

//! instruction REV
void ac_behavior( rev ) { REV(rd, rm, RB, ac_pc); }

// -----------------------------------------------------------------
