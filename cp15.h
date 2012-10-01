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


class cp15: public coprocessor{

private:
    uint32_t SCC_RB[19];
    uint32_t MMU_RB[25];

    void reset();
public:
    uint32_t *getRegister(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm);
    void MCR(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm, unsigned rt_value);

    cp15();
    ~cp15();
};

#endif
