/**
 * CP15.h - System Control Coprocessor
 *
 * This file implements a general SC coprocessor,
 * named CP15.
 * For more information, please refer to
 * Arm Cortex-A8 r3p2 manual on Section. 3.2
 *
 * This model does not implement TLB instructions
 * and Secure access control
 *
 * Author: Gabriel Krisman Bertazi
 * Date:   05/10/2012
 **/

#ifndef __CP15_H__
#define __CP15_H__
#include "coprocessor.h"
#include<stdint.h>
#include <stdarg.h>
#include <systemc.h>
#include "ac_stats_base.H"


typedef enum {CTR=0,AUX_CTR,SEC_CONF, SEC_DBG_ENABLE, NONSEC_ACC_CTR,
              COPROC_ACC_CTR, SEC_NONSEC_VEC_BASE, MON_VEC_BASE_ADD,
              MAIN_ID, SILICON_ID, MEM_MODEL_FEAT_0, IS_ATTR_0, IS_ATTR_1,
              IS_ATTR_2, IS_ATTR_3, IS_ATTR_4, IS_ATTR_5, IS_ATTR_6,
              IS_ATTR_7} SCC_regNAME;


typedef enum {TLB_TYPE,TTB_0, TTB_1, TTB_CTR,DOMAIN_ACC_CTR, DFSR,
              DFSR_AUX, IFSR,IFSR_AUX, IFAR, DFAR,
              CONTEXT_ID, FCSE_PID, TPID_RW,TPID_RO, TPID_PO,
              PRIMARY_REGION_REMAP,NORMAL_REGION_REMAP} MMU_regNAME;

class cp15: public coprocessor{

    friend class MMU;

private:
    uint32_t SCC_RB[19];
    uint32_t MMU_RB[18];

    void TLB_operations(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm, unsigned rt_value);
    void reset();
public:
    uint32_t *getRegister(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm);
    void MCR(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm, unsigned rt_value);
    uint32_t MRC(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm);

    cp15();
    ~cp15();
};

#endif
