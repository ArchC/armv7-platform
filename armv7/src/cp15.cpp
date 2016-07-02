// 'cp15.cpp' - Coprocessor15 (SCC) model.
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
// Author : Gabriel Krisman Bertazi
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#include "cp15.h"

extern bool DEBUG_CP15;

#define dprintf(args...) if(DEBUG_CP15){fprintf(stderr,args);}

#define NO_ACCESS    0b00
#define CL_READ      0b01
#define CL_WRITE     0b10

cp15::cp15 (sc_module_name name_): sc_module (name_)
{

  memset (registers, 0, 0xFFFF);
  reset ();
}

void
cp15::reset ()
{
  dprintf ("CP15 reseted to default values\n");

  registers[MAIN_ID].value = 0x412fc080;
  registers[CACHE_TYPE].value = 0x80048004;
  registers[TCM_TYPE].value = 0x000000;
  registers[TLB_TYPE].value = 0x00202001;
  registers[MULTIPROCESSOR_ID].value = 0x00000000;

  registers[PROCESSOR_FEATURE_0].value = 0x00001031;
  registers[PROCESSOR_FEATURE_1].value = 0x00000011;
  registers[DEBUG_FEATURE_0].value = 0x00000400;
  registers[AUXILIARY_FEATURE_0].value = 0x00000000;
  registers[MEMORY_MODEL_FEATURE_0].value = 0x31100003;
  registers[MEMORY_MODEL_FEATURE_1].value = 0x20000000;
  registers[MEMORY_MODEL_FEATURE_2].value = 0x01202000;
  registers[MEMORY_MODEL_FEATURE_3].value = 0x00000011;

  registers[INSTRUCTION_SET_ATTRIBUTE_0].value = 0x00101111;
  registers[INSTRUCTION_SET_ATTRIBUTE_1].value = 0x12112111;
  registers[INSTRUCTION_SET_ATTRIBUTE_2].value = 0x21232031;
  registers[INSTRUCTION_SET_ATTRIBUTE_3].value = 0x11112131;
  registers[INSTRUCTION_SET_ATTRIBUTE_4].value = 0x00011142;
  registers[INSTRUCTION_SET_ATTRIBUTE_5].value = 0x00000000;
  registers[INSTRUCTION_SET_ATTRIBUTE_6].value = 0x00000000;
  registers[INSTRUCTION_SET_ATTRIBUTE_7].value = 0x00000000;

  registers[CACHE_LEVEL_ID].value = 0x0a000023;
  registers[SILICON_ID].value = 0x00000000;

  registers[CONTROL].value = 0x00c50078;
  registers[AUXILIARY_CONTROL].value = 0x00000002;
  registers[COPROCESSOR_ACCESS_CONTROL].value = 0x00000000;
  registers[SECURE_CONFIGURATION].value = 0x00000000;
  registers[SECURE_DEBUG_ENABLED].value = 0x00000000;
  registers[NONSECURE_ACCESS_CONTROL].value = 0x00000000;

  registers[PHYSICAL_ADDRESS].value = 0x00000000 ;

  registers[PERFORMANCE_MONITOR_CONTROL].value = 0x41002000 ;
  registers[COUNT_ENABLE_SET].value = 0x00000000;
  registers[COUNT_ENABLE_CLEAR].value = 0x00000000;
  registers[OVERFLOW_FLAG_STATUS].value = 0x00000000;
  registers[SOFTWARE_INCREMENT].value = 0x00000000;
  registers[CYCLE_COUNT].value = 0x00000000;

  registers[PERFORMANCE_MONITOR_COUNT].value = 0x00000000;
  registers[USER_ENABLE].value = 0x00000000;
  registers[INTERRUPT_ENABLE_SET].value = 0x00000000;
  registers[INTERRUPT_ENABLE_CLEAR].value = 0x00000000;

  registers[L2_CACHE_LOCKDOWN].value = 0x00000000;
  registers[L2_CACHE_AUXILIARY_CONTROL].value = 0x00000042;

  registers[DATA_TLB_LOCKDOWN_REGISTER].value = 0x00000000;
  registers[INSTRUCTION_TLB_LOCKDOWN_REGISTER].value = 0x00000000;
  registers[PRIMARY_REGION_REMAP_REGISTER].value = 0x00098aa4;
  registers[NORMAL_MEMORY_REMAP_REGISTER].value = 0x44304830;

  registers[SECURE_OR_NONSECURE_VECTOR_BASE_ADDRESS].value = 0x00000000;
  registers[MONITOR_VECTOR_BASE_ADDRESS].value = 0x00000000;
  registers[INTERRUPT_STATUS].value = 0x00000000;

  registers[FCSE_PID].value = 0x00000000;
}

cp15::cp15_register *
cp15::getRegister (const unsigned opc1, const unsigned opc2,
                   const unsigned crn, const unsigned crm)
{
  unsigned h = (((crn & 0xf) << 12)
                | ((opc1 & 0xf) << 8)
                | ((crm & 0xf) << 4)
                | (opc2 & 0xf));
  return &(registers[h]);
}

void
cp15::MCR (arm_arch_ref * core,
	   const arm_impl::PrivilegeLevel pl, const unsigned opc1,
	   const unsigned opc2, const unsigned crn,
	   const unsigned crm, unsigned rt_value)
{
  dprintf ("%s: MCR: "
	   "opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X, RegVal=0x%X\n",
	   this-> name(), opc1, opc2, crn, crm, rt_value);

  struct cp15_register *reg = getRegister (opc1, opc2, crn, crm);

  // Check write permission
  // if (!(reg->permissions[pl] & CL_WRITE))
  //   {
  //     //caller has no permission to write to this register
  //     //Generate Unidentified interruption
  //     dprintf ("%s: Write not allowed. Low privilege level!", this->name());
  //     service_interrupt (*core, arm_impl::EXCEPTION_UNDEFINED_INSTR);
  //   }

  if (reg->write_callback)
    reg->write_callback (reg, &rt_value);

  // Commit write to register.
  reg->value = rt_value;

  return;
}

unsigned
cp15::MRC (arm_arch_ref * core, const arm_impl::PrivilegeLevel pl,
	   const unsigned opc1, const unsigned opc2,
	   const unsigned crn, const unsigned crm)
{
  dprintf ("%s: MRC: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X\n",
	   this->name(), opc1, opc2, crn, crm);

  struct cp15_register *reg = getRegister (opc1, opc2, crn, crm);

  // Check read permission
  // if (!(reg->permissions[pl] & CL_READ))
  //   {
  //     //caller has no permission to read this register
  //     //Generate Unidentified interruption
  //     dprintf ("%s: Read not allowed. Low privilege level!\n", this->name());
  //     service_interrupt (*core, arm_impl::EXCEPTION_UNDEFINED_INSTR);
  //   }

  if (reg->read_callback)
    reg->read_callback (reg);

  return reg->value;
}
