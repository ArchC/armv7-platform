#include "cp15.h"

extern bool DEBUG_CP15;

#define dprintf(args...) if(DEBUG_CP15){fprintf(stderr,args);}

#define NO_ACCESS    0b00
#define CL_READ      0b01
#define CL_WRITE     0b10

cp15::cp15()
{
    reset();
}

cp15::~cp15()
{
}

bool readPermission(cp15_reg & reg, arm_impl::PrivilegeLevel pl){
    return (reg.permissions[pl] & CL_READ)? true:false;
}
bool writePermission(cp15_reg & reg, arm_impl::PrivilegeLevel pl){
    return (reg.permissions[pl] & CL_WRITE)? true:false;
}

void cp15::reset()
{
    dprintf("CP15 reseted to default values\n");

    //SCC
    //Control
    SCC_RB[CTR].value                                       = 0x00C50078; //Depends on external
    SCC_RB[CTR].permissions[arm_impl::PL0]                  = (NO_ACCESS);
    SCC_RB[CTR].permissions[arm_impl::PL1]                  = (CL_READ|CL_WRITE);
    //Auxiliar Control
    SCC_RB[AUX_CTR].value                                   = 0x00000002;
    SCC_RB[AUX_CTR].permissions[arm_impl::PL0]              = (NO_ACCESS);
    SCC_RB[AUX_CTR].permissions[arm_impl::PL1]              = (CL_READ|CL_WRITE);

    //Secure Configuration
    SCC_RB[SEC_CONF].value                                  = 0x00000000;
    SCC_RB[SEC_CONF].permissions[arm_impl::PL0]             = (NO_ACCESS);
    SCC_RB[SEC_CONF].permissions[arm_impl::PL1]             = (CL_READ|CL_WRITE);

    //Secure Debug Enable
    SCC_RB[SEC_DBG_ENABLE].value                            = 0x00000000;
    SCC_RB[SEC_DBG_ENABLE].permissions[arm_impl::PL0]       = (NO_ACCESS);
    SCC_RB[SEC_DBG_ENABLE].permissions[arm_impl::PL1]       = (CL_READ|CL_WRITE);

    //NonSecure Access Control
    SCC_RB[NONSEC_ACC_CTR].value                            = 0x00000000;
    SCC_RB[NONSEC_ACC_CTR].permissions[arm_impl::PL0]       = (NO_ACCESS);
    SCC_RB[NONSEC_ACC_CTR].permissions[arm_impl::PL1]       = (CL_READ|CL_WRITE);

    //Coprocessor access control
    SCC_RB[COPROC_ACC_CTR].value                            = 0x00000000;
    SCC_RB[COPROC_ACC_CTR].permissions[arm_impl::PL0]       = (NO_ACCESS);
    SCC_RB[COPROC_ACC_CTR].permissions[arm_impl::PL1]       = (CL_READ|CL_WRITE);

    //Secure & nonsecure vector base
    SCC_RB[SEC_NONSEC_VEC_BASE].value                       = 0x00000000;
    SCC_RB[SEC_NONSEC_VEC_BASE].permissions[arm_impl::PL0]  = (NO_ACCESS);
    SCC_RB[SEC_NONSEC_VEC_BASE].permissions[arm_impl::PL1]  = (CL_READ|CL_WRITE);

    //Monitor vector base address
    SCC_RB[MON_VEC_BASE_ADD].value                          = 0x00000000;
    SCC_RB[MON_VEC_BASE_ADD].permissions[arm_impl::PL0]     = (NO_ACCESS);
    SCC_RB[MON_VEC_BASE_ADD].permissions[arm_impl::PL1]     = (CL_READ|CL_WRITE);

    //Main ID
    SCC_RB[MAIN_ID].value                                   = 0x413FC082;
    SCC_RB[MAIN_ID].permissions[arm_impl::PL0]              = (NO_ACCESS);
    SCC_RB[MAIN_ID].permissions[arm_impl::PL1]              = (CL_READ);

    //Silicon ID
    SCC_RB[SILICON_ID].value                                = 0x00000000; //Depends on external
    SCC_RB[SILICON_ID].permissions[arm_impl::PL0]           = (NO_ACCESS);
    SCC_RB[SILICON_ID].permissions[arm_impl::PL1]           = (CL_READ);

    //Memory model feature
    SCC_RB[MEM_MODEL_FEAT_0].value                          = 0x01100003;
    SCC_RB[MEM_MODEL_FEAT_0].permissions[arm_impl::PL0]     = (NO_ACCESS);
    SCC_RB[MEM_MODEL_FEAT_0].permissions[arm_impl::PL1]     = (CL_READ);

    //Instruction Set Attribute 0-7
    SCC_RB[IS_ATTR_0].value                                 = 0x00101111;
    SCC_RB[IS_ATTR_0].permissions[arm_impl::PL0]            = (NO_ACCESS);
    SCC_RB[IS_ATTR_0].permissions[arm_impl::PL1]            = (CL_READ);

    SCC_RB[IS_ATTR_1].value                                 = 0x13112111;
    SCC_RB[IS_ATTR_1].permissions[arm_impl::PL0]            = (NO_ACCESS);
    SCC_RB[IS_ATTR_1].permissions[arm_impl::PL1]            = (CL_READ);

    SCC_RB[IS_ATTR_2].value                                 = 0x21232031;
    SCC_RB[IS_ATTR_2].permissions[arm_impl::PL0]            = (NO_ACCESS);
    SCC_RB[IS_ATTR_2].permissions[arm_impl::PL1]            = (CL_READ);

    SCC_RB[IS_ATTR_3].value                                 = 0x11112131;
    SCC_RB[IS_ATTR_3].permissions[arm_impl::PL0]            = (NO_ACCESS);
    SCC_RB[IS_ATTR_3].permissions[arm_impl::PL1]            = (CL_READ);

    SCC_RB[IS_ATTR_4].value                                 = 0x00011142;
    SCC_RB[IS_ATTR_4].permissions[arm_impl::PL0]            = (NO_ACCESS);
    SCC_RB[IS_ATTR_4].permissions[arm_impl::PL1]            = (CL_READ);

    SCC_RB[IS_ATTR_5].value                                 = 0x00000000;
    SCC_RB[IS_ATTR_5].permissions[arm_impl::PL0]            = (NO_ACCESS);
    SCC_RB[IS_ATTR_5].permissions[arm_impl::PL1]            = (CL_READ);

    SCC_RB[IS_ATTR_6].value                                 = 0x00000000;
    SCC_RB[IS_ATTR_6].permissions[arm_impl::PL0]            = (NO_ACCESS);
    SCC_RB[IS_ATTR_6].permissions[arm_impl::PL1]            = (CL_READ);

    SCC_RB[IS_ATTR_7].value                                 = 0x00000000;
    SCC_RB[IS_ATTR_7].permissions[arm_impl::PL0]            = (NO_ACCESS);
    SCC_RB[IS_ATTR_7].permissions[arm_impl::PL1]            = (CL_READ);

    //MMU
    //TLB type
    MMU_RB[TLB_TYPE].value                                  = 0x00202001;
    MMU_RB[TLB_TYPE].permissions[arm_impl::PL0]             = (NO_ACCESS);
    MMU_RB[TLB_TYPE].permissions[arm_impl::PL1]             = (CL_READ);

    //Translation table 0 base
    MMU_RB[TTB_0].value                                     = 0x00000000; //Unpredictable
    MMU_RB[TTB_0].permissions[arm_impl::PL0]                = (NO_ACCESS);
    MMU_RB[TTB_0].permissions[arm_impl::PL1]                = (CL_READ|CL_WRITE);

    //translation table 1 base
    MMU_RB[TTB_1].value                                     = 0x00000000; //Unpredictable
    MMU_RB[TTB_1].permissions[arm_impl::PL0]                = (NO_ACCESS);
    MMU_RB[TTB_1].permissions[arm_impl::PL1]                = (CL_READ|CL_WRITE);

    //Translation table control
    MMU_RB[TTB_CTR].value                                   = 0x00000000; //Unpredictable
    MMU_RB[TTB_CTR].permissions[arm_impl::PL0]              = (NO_ACCESS);
    MMU_RB[TTB_CTR].permissions[arm_impl::PL1]              = (CL_READ|CL_WRITE);

    //Domain access control
    MMU_RB[DOMAIN_ACC_CTR].value                            = 0x00000000; //Unpredictable
    MMU_RB[DOMAIN_ACC_CTR].permissions[arm_impl::PL0]       = (NO_ACCESS);
    MMU_RB[DOMAIN_ACC_CTR].permissions[arm_impl::PL1]       = (CL_READ|CL_WRITE);

    //Data Fault Status Register
    MMU_RB[DFSR].value                                      = 0x00000000; //Unpredictable
    MMU_RB[DFSR].permissions[arm_impl::PL0]                 = (NO_ACCESS);
    MMU_RB[DFSR].permissions[arm_impl::PL1]                 = (CL_READ|CL_WRITE);

    //Data Fault Status Auxiliar Register
    MMU_RB[DFSR_AUX].value                                  = 0x00000000; //Unpredictable
    MMU_RB[DFSR_AUX].permissions[arm_impl::PL0]             = (NO_ACCESS);
    MMU_RB[DFSR_AUX].permissions[arm_impl::PL1]             = (CL_READ|CL_WRITE);

    //Instruction Fault Status Register
    MMU_RB[IFSR].value                                      = 0x00000000; //Unpredictable
    MMU_RB[IFSR].permissions[arm_impl::PL0]                 = (NO_ACCESS);
    MMU_RB[IFSR].permissions[arm_impl::PL1]                 = (CL_READ|CL_WRITE);

    //Instruction Fault Status Auxiliar Register
    MMU_RB[IFSR_AUX].value                                  = 0x00000000; //Unpredictable
    MMU_RB[IFSR_AUX].permissions[arm_impl::PL0]             = (NO_ACCESS);
    MMU_RB[IFSR_AUX].permissions[arm_impl::PL1]             = (CL_READ|CL_WRITE);

    //Instruction Fault Address Register
    MMU_RB[IFAR].value                                      = 0x00000000; //Unpredictable
    MMU_RB[IFAR].permissions[arm_impl::PL0]                 = (NO_ACCESS);
    MMU_RB[IFAR].permissions[arm_impl::PL1]                 = (CL_READ|CL_WRITE);

    //Data fault address register
    MMU_RB[DFAR].value                                      = 0x00000000; //Unpredictable
    MMU_RB[DFAR].permissions[arm_impl::PL0]                 = (NO_ACCESS);
    MMU_RB[DFAR].permissions[arm_impl::PL1]                 = (CL_READ|CL_WRITE);

    //Primary region remap
    MMU_RB[PRIMARY_REGION_REMAP].value                      = 0x00098AA4;
    MMU_RB[PRIMARY_REGION_REMAP].permissions[arm_impl::PL0] = (NO_ACCESS);
    MMU_RB[PRIMARY_REGION_REMAP].permissions[arm_impl::PL1] = (CL_READ|CL_WRITE);

    //Normal Region Remap
    MMU_RB[NORMAL_REGION_REMAP].value  = 0x44E048E0;
    MMU_RB[NORMAL_REGION_REMAP].permissions[arm_impl::PL0]  = (NO_ACCESS);
    MMU_RB[NORMAL_REGION_REMAP].permissions[arm_impl::PL1]  = (CL_READ|CL_WRITE);

    //Context ID
    MMU_RB[CONTEXT_ID].value                                = 0x00000000; //Unpredictable
    MMU_RB[CONTEXT_ID].permissions[arm_impl::PL0]           = (NO_ACCESS);
    MMU_RB[CONTEXT_ID].permissions[arm_impl::PL1]           = (CL_READ|CL_WRITE);

    //FCSE PID
    MMU_RB[FCSE_PID].value                                  = 0x00000000;
    MMU_RB[FCSE_PID].permissions[arm_impl::PL0]             = (NO_ACCESS);
    MMU_RB[FCSE_PID].permissions[arm_impl::PL1]             = (CL_READ|CL_WRITE);

    //Thread and process ID registers
    MMU_RB[TPID_RW].value                                   = 0x00000000; //Unpredictable
    MMU_RB[TPID_RW].permissions[arm_impl::PL0]              = (CL_READ|CL_WRITE);
    MMU_RB[TPID_RW].permissions[arm_impl::PL1]              = (CL_READ|CL_WRITE);

    MMU_RB[TPID_RO].value                                   = 0x00000000; //Unpredictable
    MMU_RB[TPID_RO].permissions[arm_impl::PL0]              = (CL_READ);
    MMU_RB[TPID_RO].permissions[arm_impl::PL1]              = (CL_READ);

    MMU_RB[TPID_PO].value                                   = 0x00000000; //Unpredictable
    MMU_RB[TPID_PO].permissions[arm_impl::PL0]              = (NO_ACCESS);
    MMU_RB[TPID_PO].permissions[arm_impl::PL1]              = (CL_READ|CL_WRITE);
    //
}

cp15_reg *cp15::getRegister(unsigned opc1, unsigned opc2, unsigned crn,
                            unsigned crm)
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


void cp15::MCR(arm_arch_ref *core, arm_impl::PrivilegeLevel pl, unsigned opc1,
               unsigned opc2, unsigned crn, unsigned crm, unsigned rt_value){
    dprintf("\nCP15 operation MCR: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X, RegVal=0x%X\n", opc1, opc2, crn, crm, rt_value);

    if(crn == 8){
        //Trap TLB instructions
        TLB_operations(opc1, opc2, crn,crm, rt_value);
        return;
    }

    cp15_reg *dest = getRegister(opc1, opc2, crn,crm);
    if(dest == NULL){
        fprintf(stderr,"WARNING: access to non-implemented cp15 register. Operation: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X\n", opc1, opc2, crn, crm);
        return;
    }
    if(!writePermission(*dest, pl)){
        //caller has no permission to write to this register
        //Generate Unidentified interruption
        dprintf("Write not allowed. Low privilege level!");
        service_interrupt(*core,arm_impl::EXCEPTION_UNDEFINED_INSTR);

    }

        (*dest).value = rt_value;
    return;
}

uint32_t cp15::MRC(arm_arch_ref *core, arm_impl::PrivilegeLevel pl,
                   unsigned opc1, unsigned opc2, unsigned crn, unsigned crm)
{
    dprintf("CP15 operation MRC: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X\n", opc1, opc2, crn, crm);
    cp15_reg *dest = getRegister(opc1, opc2, crn,crm);

    if(dest == NULL){
        fprintf(stderr,"WARNING: access to non-implemented cp15 register.\n operation: opc1=0x%X, opc2=0x%X, crn=0X%X, crm=0x%X", opc1, opc2, crn, crm);

        return 0;
    }

    if(!readPermission(*dest, pl)){
        //caller has no permission to read this register
        //Generate Unidentified interruption
        dprintf("Read not allowed. Low privilege level!");
        service_interrupt(*core,arm_impl::EXCEPTION_UNDEFINED_INSTR);
    }
    return ((*dest).value);
}


void cp15::TLB_operations(unsigned opc1, unsigned opc2, unsigned crn,
                          unsigned crm, unsigned rt_value)
{
    dprintf("TLB Operations not implemented in this model (yet). \n opc1=0x%X, opc2=0x%X, crn=0x%X, crm=0x%X\n", opc1, opc2, crn, crm);
}


