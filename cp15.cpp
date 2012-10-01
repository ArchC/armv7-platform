#include "cp15.h"

extern bool DEBUG_CP15;

#define dprintf(args...) if(DEBUG_CP15){fprintf(stderr,args);}



//static inline int dprintf(const char *format, ...) {
//    int ret;
//    if (DEBUG_CP15) {
//        va_list args;
//        va_start(args, format);
//        ret = fprintf(stderr, format, args);
//        va_end(args);
//    }
//    return ret;
//}

cp15::cp15()
{
    reset();
}

cp15::~cp15()
{
}

void cp15::reset()
{
    dprintf("CP15 reseted to default values");

    SCC_RB[CTR]                  = 0x00C50078; //Depends on external
    SCC_RB[AUX_CTR]              = 0x00000002;
    SCC_RB[SEC_CONF]             = 0x00000000;
    SCC_RB[SEC_DBG_ENABLE]       = 0x00000000;
    SCC_RB[NONSEC_ACC_CTR]       = 0x00000000;
    SCC_RB[COPROC_ACC_CTR]       = 0x00000000;
    SCC_RB[SEC_NONSEC_VEC_BASE]  = 0x00000000;
    SCC_RB[MON_VEC_BASE_ADD]     = 0x00000000;
    SCC_RB[MAIN_ID]              = 0x413FC082;
    SCC_RB[SILICON_ID]           = 0x00000000; //Depends on external
    SCC_RB[MEM_MODEL_FEAT_0]     = 0x01100003;

    SCC_RB[IS_ATTR_0]            = 0x00101111;
    SCC_RB[IS_ATTR_1]            = 0x13112111;
    SCC_RB[IS_ATTR_2]            = 0x21232031;
    SCC_RB[IS_ATTR_3]            = 0x11112131;
    SCC_RB[IS_ATTR_4]            = 0x00011142;
    SCC_RB[IS_ATTR_5]            = 0x00000000;
    SCC_RB[IS_ATTR_6]            = 0x00000000;
    SCC_RB[IS_ATTR_7]            = 0x00000000;

}

uint32_t * cp15::getRegister(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm)
{
    if(crn == 0){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(SCC_RB[MAIN_ID]);
                if(opc2 == 4) return &(SCC_RB[MAIN_ID]);
                if(opc2 == 6) return &(SCC_RB[MAIN_ID]);
                if(opc2 == 7) return &(SCC_RB[MAIN_ID]);
            }
            if(crm == 1){
                if(opc2 == 4) return &(SCC_RB[MEM_MODEL_FEAT_0]);
            }
            if(crm == 2){
                if(opc2 == 0) return &(SCC_RB[IS_ATTR_0]);
                if(opc2 == 1) return &(SCC_RB[IS_ATTR_1]);
                if(opc2 == 2) return &(SCC_RB[IS_ATTR_2]);
                if(opc2 == 3) return &(SCC_RB[IS_ATTR_3]);
                if(opc2 == 4) return &(SCC_RB[IS_ATTR_4]);
                if(opc2 == 5) return &(SCC_RB[IS_ATTR_5]);
                if(opc2 == 6) return &(SCC_RB[IS_ATTR_6]);
                if(opc2 == 7) return &(SCC_RB[IS_ATTR_7]);
            }
        }
        if(opc1 == 1){
            if(crm == 0){
                if(opc2 == 7) return &(SCC_RB[SILICON_ID]);
            }
        }
    }

    if(crn == 1){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(SCC_RB[CTR]);
                if(opc2 == 1) return &(SCC_RB[AUX_CTR]);
                if(opc2 == 2) return &(SCC_RB[COPROC_ACC_CTR]);
            }
            if(crm == 1){
                if(opc2 == 0) return &(SCC_RB[SEC_CONF]);
                if(opc2 == 1) return &(SCC_RB[SEC_DBG_ENABLE]);
                if(opc2 == 2) return &(SCC_RB[NONSEC_ACC_CTR]);
            }
        }
    }
    if(crn == 12){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(SCC_RB[SEC_NONSEC_VEC_BASE]);
                if(opc2 == 1) return &(SCC_RB[MON_VEC_BASE_ADD]);
            }
        }
    }
}



void cp15::MCR(unsigned opc1, unsigned opc2, unsigned crn,
               unsigned crm, unsigned rt_value)
{
//    if(DEBUG_CP15)
//    printf("CP15 operation: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X, RegVal=0x%X\n", opc1, opc2, crn, crm, rt_value);
    dprintf("DCP15 operation: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X, RegVal=0x%X\n", opc1, opc2, crn, crm, rt_value);
    uint32_t *dest = getRegister(opc1, opc2, crn,crm);
    *dest = rt_value;
    return;
}


