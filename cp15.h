// "CP15.h" - System Control Coprocessor
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
// Author : Gabriel Krisman Bertazi, 05/10/2012
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef CP15_H
#define CP15_H

#include<stdint.h>
#include <stdarg.h>
#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "coprocessor.h"

class cp15: public coprocessor, public sc_module
{
 public:
  static const int MAIN_ID                      = 0x0000;
  static const int CACHE_TYPE                   = 0x0001;
  static const int TCM_TYPE                     = 0x0002;
  static const int TLB_TYPE                     = 0x0003;
  static const int MULTIPROCESSOR_ID            = 0x0005;

  static const int PROCESSOR_FEATURE_0          = 0x0010;
  static const int PROCESSOR_FEATURE_1          = 0x0011;
  static const int DEBUG_FEATURE_0              = 0x0012;
  static const int AUXILIARY_FEATURE_0          = 0x0013;
  static const int MEMORY_MODEL_FEATURE_0       = 0x0014;
  static const int MEMORY_MODEL_FEATURE_1       = 0x0015;
  static const int MEMORY_MODEL_FEATURE_2       = 0x0016;
  static const int MEMORY_MODEL_FEATURE_3       = 0x0017;

  static const int INSTRUCTION_SET_ATTRIBUTE_0  = 0x0020;
  static const int INSTRUCTION_SET_ATTRIBUTE_1  = 0x0021;
  static const int INSTRUCTION_SET_ATTRIBUTE_2  = 0x0022;
  static const int INSTRUCTION_SET_ATTRIBUTE_3  = 0x0023;
  static const int INSTRUCTION_SET_ATTRIBUTE_4  = 0x0024;
  static const int INSTRUCTION_SET_ATTRIBUTE_5  = 0x0025;
  static const int INSTRUCTION_SET_ATTRIBUTE_6  = 0x0026;
  static const int INSTRUCTION_SET_ATTRIBUTE_7  = 0x0027;

  static const int CACHE_SIZE_IDENTIFICATION    = 0x0100;
  static const int CACHE_LEVEL_ID               = 0x0101;
  static const int SILICON_ID                   = 0x0107;
  static const int CACHE_SIZE_SELECTION         = 0x0200;
  static const int CONTROL                      = 0x1000;
  static const int AUXILIARY_CONTROL            = 0x1001;
  static const int COPROCESSOR_ACCESS_CONTROL   = 0x1002;
  static const int SECURE_CONFIGURATION         = 0x1010;
  static const int SECURE_DEBUG_ENABLED         = 0x1011;
  static const int NONSECURE_ACCESS_CONTROL     = 0x1012;

  static const int TRANSLATION_TABLE_BASE_0     = 0x2000;
  static const int TRANSLATION_TABLE_BASE_1     = 0x2001;
  static const int TRANSLATION_TABLE_BASE_CONTROL = 0x2002;

  static const int DOMAIN_ACCESS_CONTROL        = 0x3000;
  static const int DATA_FAULT_STATUS            = 0x5000;
  static const int INSTRUUCTION_FAULT_STATUS    = 0x5001;
  static const int DATA_AUXILIARY_FAULT_STATUS  = 0x5010;
  static const int INSTRUCTION_AUXILIARY_FAULT_STATUS = 0x50011;

  static const int DATA_FAULT_ADDRESS           = 0x6000;
  static const int INSTRUCTION_FAULT_ADDRESS    = 0x6002;

  static const int NOP_WFI                      = 0x7004;
  static const int PHYSICAL_ADDRESS             = 0x7040;
  static const int INVALIDATE_ALL_INSTR_CACHE_TO_UNIFICATION_POINT = 0x7050;
  static const int INVALIDATE_INSTR_CACHE_LINE_UNIFICATION_POINT = 0x7051;
  static const int FLUSH_PREFETCH_BUFFER        = 0x7054;

  static const int NOP_INVALIDATE_ENTIRE_BRANCH_PREDICTOR = 0x7056;
  static const int NOP_INVALIDATE_BRANCH_PREDICTOR_LINE_MVA = 0x7057;

  static const int INVALIDATE_DATA_CACHE_LINE_TO_MVA_POINT = 0x7061;
  static const int INVALIDATE_DATA_CACHE_LINE_BY_SET_AND_WAY = 0x7062;

  static const int VA_TO_PA_TRANSLATION_IN_CURRENT_STATE = 0x7080;
  static const int VA_TO_PA_TRANSLATION_IN_OTHER_STATE = 0x7084;
  static const int CLEAN_DATA_CACHE_LINE_TO_MVA_POINT = 0x70a1;

  static const int CLEAN_DATA_CACHE_LINE_BY_SET_AND_WAY = 0x70a2;

  static const int DATA_SYNCHRONIZATION_BARRIER = 0x70a4;
  static const int DATA_MEMORY_BARRIER = 0x70a5;

  static const int CLEAN_DATA_CACHE_TO_UNIFICATION_POINT = 0x70b1;
  static const int CLEAN_AND_INVALIDATE_DATA_CACHE_TO_MVA_POINT = 0x70e1;
  static const int CLEAN_AND_INVALIDATE_DATA_CACHE_LINE_BY_SET_AND_WAY = 0x70e2;

  static const int INVALIDATE_INSTRUCTION_TLB_UNLOCKED_ENTRIES = 0x8050;
  static const int INVALIDATE_INSTRUCTION_TLB_ENTRY_BY_MVA = 0x8051;
  static const int INVALIDATE_INSTRUCTION_TLB_ENTRY_ON_ASID_MATCH = 0x8052;

  static const int INVALIDATE_DATA_TLB_UNLOCKED_ENTRIES = 0x8060;
  static const int INVALIDATE_DATA_TLB_ENTRY_BY_MVA = 0x8061;
  static const int INVALIDATE_DATA_TLB_ENTRY_ON_ASID_MATCH = 0x8062;

  static const int INVALIDATE_UNIFIED_TLB_UNLOCKED_ENTRIES = 0x8070;
  static const int INVALIDATE_UNIFIED_TLB_ENTRY_BY_MVA = 0x8071;
  static const int INVALIDATE_UNIFIED_TLB_ENTRY_ON_ASID_MATCH = 0x8072;

  static const int PERFORMANCE_MONITOR_CONTROL = 0x90c0;
  static const int COUNT_ENABLE_SET = 0x90c1;
  static const int COUNT_ENABLE_CLEAR = 0x90c2;
  static const int OVERFLOW_FLAG_STATUS = 0x90c3;
  static const int SOFTWARE_INCREMENT = 0x90c4;
  static const int PERFORMANCE_COUNTER_SELECTION = 0x90c5;
  static const int CYCLE_COUNT = 0x90d0;
  static const int EVENT_SELECTION = 0x90d1;
  static const int PERFORMANCE_MONITOR_COUNT = 0x90d2;
  static const int USER_ENABLE = 0x90e0;
  static const int INTERRUPT_ENABLE_SET = 0x90e1;
  static const int INTERRUPT_ENABLE_CLEAR = 0x90e2;

  static const int L2_CACHE_LOCKDOWN = 0x9100;
  static const int L2_CACHE_AUXILIARY_CONTROL = 0x9102;

  static const int DATA_TLB_LOCKDOWN_REGISTER = 0xa000;
  static const int INSTRUCTION_TLB_LOCKDOWN_REGISTER = 0xa001;
  static const int DATA_TLB_PRELOAD = 0xa010;
  static const int INSTRUCTION_TLB_PRELOAD = 0xa011;
  static const int PRIMARY_REGION_REMAP_REGISTER = 0xa020;
  static const int NORMAL_MEMORY_REMAP_REGISTER = 0xa021;

  static const int SECURE_OR_NONSECURE_VECTOR_BASE_ADDRESS = 0xc000;
  static const int MONITOR_VECTOR_BASE_ADDRESS = 0xc001;
  static const int INTERRUPT_STATUS = 0xc010;

  static const int FCSE_PID = 0xd000;
  static const int CONTEXT_ID = 0xd001;

  // Struct that stores a coprocessor 15 register.
 private:
  struct cp15_register
  {
    // Value stored in the register.

    uint32_t value;

    // Permissions for the register.

    unsigned char permissions[2];

    // Callback called during a write to this register. It is executed
    // before writing value.

    void (*write_callback) (struct cp15_register *reg, uint32_t *datum);

    // Callback called during a read to the register. It is executed before
    // reading value.

    void (*read_callback) (struct cp15_register *reg);
  };

  friend class MMU;

  struct cp15_register registers[0xFFFF];

  void reset ();

  struct cp15_register *getRegister (const unsigned opc1,
                                     const unsigned opc2,
                                     const unsigned crn,
                                     const unsigned crm);
public:
  void MCR (arm_arch_ref *core, const arm_impl::PrivilegeLevel pl,
            const unsigned opc1, const unsigned opc2,
            const unsigned crn,  const unsigned crm,
            unsigned rt_value);

  uint32_t MRC (arm_arch_ref *core, const arm_impl::PrivilegeLevel pl,
		const unsigned opc1, const unsigned opc2,
		const unsigned crn, const unsigned crm);

  cp15 (sc_module_name name_);

  uint32_t getRegisterValue (const unsigned hash)
  {return registers[hash].value;};
};

#endif // !CP15_H.

