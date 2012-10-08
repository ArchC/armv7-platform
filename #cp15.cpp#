#include "cp15.h"

extern bool DEBUG_CP15;

#define dprintf(args...) if(DEBUG_CP15){fprintf(stderr,args);}

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

    //SCC
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


    //MMU
    MMU_RB[TLB_TYPE]             = 0x00202001;
    MMU_RB[TTB_0]                = 0x00000000; //Unpredictable
    MMU_RB[TTB_1]                = 0x00000000; //Unpredictable
    MMU_RB[TTB_CTR]              = 0x00000000; //Unpredictable
    MMU_RB[DOMAIN_ACC_CTR]       = 0x00000000; //Unpredictable
    MMU_RB[DFSR]                 = 0x00000000; //Unpredictable
    MMU_RB[DFSR_AUX]             = 0x00000000; //Unpredictable
    MMU_RB[IFSR]                 = 0x00000000; //Unpredictable
    MMU_RB[IFSR_AUX]             = 0x00000000; //Unpredictable
    MMU_RB[IFAR]                 = 0x00000000; //Unpredictable
    MMU_RB[DFAR]                 = 0x00000000; //Unpredictable
    MMU_RB[PRIMARY_REGION_REMAP] = 0x00098AA4;
    MMU_RB[NORMAL_REGION_REMAP]  = 0x44E048E0;
    MMU_RB[CONTEXT_ID]           = 0x00000000; //Unpredictable
    MMU_RB[FCSE_PID]            = 0x00000000;
    MMU_RB[TPID_RW]              = 0x00000000; //Unpredictable
    MMU_RB[TPID_RO]              = 0x00000000; //Unpredictable
    MMU_RB[TPID_PO]              = 0x00000000; //Unpredictable


}

uint32_t * cp15::getRegister(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm)
{
    if(crn == 0){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(SCC_RB[MAIN_ID]);
                if(opc2 == 3) return &(MMU_RB[TLB_TYPE]);
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
    if(crn == 2){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(MMU_RB[TTB_0]);
                if(opc2 == 1) return &(MMU_RB[TTB_1]);
                if(opc2 == 2) return &(MMU_RB[TTB_CTR]);
            }
        }
    }
    if(crn == 3){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(MMU_RB[DOMAIN_ACC_CTR]);
            }
        }
    }
    if(crn == 5){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(MMU_RB[DFSR]);
                if(opc2 == 1) return &(MMU_RB[IFSR]);
            }
            if(crm == 1){
                if(opc2 == 0) return &(MMU_RB[DFSR_AUX]);
                if(opc2 == 1) return &(MMU_RB[IFSR_AUX]);
            }
        }
    }
    if(crn == 6){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(MMU_RB[DFAR]);
                if(opc2 == 2) return &(MMU_RB[IFAR]);
            }
        }
    }
    if(crn == 10){
        if(opc1 == 0){
            if(crm == 2){
                if(opc2 == 0) return &(MMU_RB[PRIMARY_REGION_REMAP]);
                if(opc2 == 1) return &(MMU_RB[NORMAL_REGION_REMAP]);
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
    if(crn == 13){
        if(opc1 == 0){
            if(crm == 0){
                if(opc2 == 0) return &(MMU_RB[FCSE_PID]);
                if(opc2 == 1) return &(MMU_RB[CONTEXT_ID]);
                if(opc2 == 2) return &(MMU_RB[TPID_RW]);
                if(opc2 == 3) return &(MMU_RB[TPID_RO]);
                if(opc2 == 4) return &(MMU_RB[TPID_PO]);
            }
        }
    }
    return NULL;
}


void cp15::MCR(unsigned opc1, unsigned opc2, unsigned crn,
               unsigned crm, unsigned rt_value)
{
    dprintf("CP15 operation MCR: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X, RegVal=0x%X\n", opc1, opc2, crn, crm, rt_value);

    if(crn == 8){
        //Trap  TLB instructions
        TLB_operations(opc1, opc2, crn,crm, rt_value);
        return;
    }

    uint32_t *dest = getRegister(opc1, opc2, crn,crm);
    if(dest == NULL){
        fprintf(stderr,"WARNING: access to non-implemented cp15 register. Operation: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X\n", opc1, opc2, crn, crm);
        return;
    }

     *dest = rt_value;
    return;
}


uint32_t cp15::MRC(unsigned opc1, unsigned opc2, unsigned crn,
               unsigned crm)
{
    dprintf("CP15 operation MRC: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X\n", opc1, opc2, crn, crm);
    uint32_t *dest = getRegister(opc1, opc2, crn,crm);

    if(dest == NULL){
        fprintf(stderr,"WARNING: access to non-implemented cp15 register.\n operation: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X", opc1, opc2, crn, crm);
        return 0;
    }
    return *dest;
}


void cp15::TLB_operations(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm, unsigned rt_value)
{
    fprintf(stderr,"TLB Operations not implemented in this model(yet).");
}


