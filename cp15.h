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
  typedef enum
  { CTR = 0, AUX_CTR, SEC_CONF, SEC_DBG_ENABLE, NONSEC_ACC_CTR,
    COPROC_ACC_CTR, SEC_NONSEC_VEC_BASE, MON_VEC_BASE_ADD,
    MAIN_ID, SILICON_ID, MEM_MODEL_FEAT_0, IS_ATTR_0, IS_ATTR_1,
    IS_ATTR_2, IS_ATTR_3, IS_ATTR_4, IS_ATTR_5, IS_ATTR_6,
    IS_ATTR_7
  } SCC_regNAME;

  typedef enum
  { TLB_TYPE, TTB_0, TTB_1, TTB_CTR, DOMAIN_ACC_CTR, DFSR,
    DFSR_AUX, IFSR, IFSR_AUX, IFAR, DFAR,
    CONTEXT_ID, FCSE_PID, TPID_RW, TPID_RO, TPID_PO,
    PRIMARY_REGION_REMAP, NORMAL_REGION_REMAP
  } MMU_regNAME;

  typedef struct
  {
    uint32_t value;
    unsigned char permissions[2];
  } cp15_reg;

  friend class MMU;

private:
  cp15_reg SCC_RB[19];
  cp15_reg MMU_RB[18];

  void reset ();

  void TLB_operations (const unsigned opc1, const unsigned opc2,
		       const unsigned crn, const unsigned crm,
		       const unsigned rt_value);

  cp15_reg *getRegister (const unsigned opc1, const unsigned opc2,
			 const unsigned crn, const unsigned crm);
public:
  void MCR (arm_arch_ref * core,
	    const arm_impl::PrivilegeLevel pl, const unsigned opc1,
	    const unsigned opc2, const unsigned crn,
	    const unsigned crm, const unsigned rt_value);

  uint32_t MRC (arm_arch_ref * core, const arm_impl::PrivilegeLevel pl,
		const unsigned opc1, const unsigned opc2,
		const unsigned crn, const unsigned crm);

  cp15 (sc_module_name name_);
};

#endif // !CP15_H.
