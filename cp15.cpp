#include "cp15.h"


extern bool DEBUG_CP15;

static inline int dprintf(const char *format, ...) {
    int ret;
    if (DEBUG_BUS) {
        va_list args;
        va_start(args, format);
        ret = vfprintf(ac_err, format, args);
        va_end(args);
    }
    return ret;
}



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

    SCC_RB[CONTROL]                          = 0x00C50078;//Depends on external
    SCC_RB[AUXILIARY_CONTROL]                = 0x00000002;
    SCC_RB[SECURE_CONFIGURATION]             = 0x00000000;
    SCC_RB[SECURE_DEBUG_ENABLE]              = 0x00000000;
    SCC_RB[NONSECURE_ACCESS_CONTROL]         = 0x00000000;
    SCC_RB[COPROCESSOR_ACCESS_CONTROL]       = 0x00000000;
    SCC_RB[SECURE_OR_NONSECURE_VECTOR_BASE]  = 0x00000000;
    SCC_RB[MONITOR_VECTOR_BASE_ADDRESS]      = 0x00000000;
    SCC_RB[MAIN_ID]                          = 0x413FC082;
    SCC_RB[SILICON_ID]                       = 0x0;       //Depends on external
    SCC_RB[MEMORY_MODEL_FEATURE_0]           = 0x01100003;

    SCC_RB[IS_ATTRIBUTES_0]                  = 0x00101111;
    SCC_RB[IS_ATTRIBUTES_1]                  = 0x13112111;
    SCC_RB[IS_ATTRIBUTES_2]                  = 0x21232031;
    SCC_RB[IS_ATTRIBUTES_3]                  = 0x11112131;
    SCC_RB[IS_ATTRIBUTES_4]                  = 0x00011142;
    SCC_RB[IS_ATTRIBUTES_5]                  = 0x00000000;
    SCC_RB[IS_ATTRIBUTES_6]                  = 0x00000000;
    SCC_RB[IS_ATTRIBUTES_7]                  = 0x00000000;
}

//TODO: improve this
inline uint32_t *REGDECODER(unsigned opc1, unsigned opc2, unsigned crn, unsigned crm)
{
    //Implements a hardcoded Hash to find CP15 register given opcode.

    if(crn1 == 0){
        if(op1 == 0){
            if(crm == 0){
                switch(opc2){
                case 0: case 4: case 6: case 7:
                    dprintf(" SCC [MAIN_ID] accessed\n");
                    return *(SCC_RB[MAIN_ID]);
                    break;
                }
            }
            if(crm == 1){
                switch(opc2){
                case 4:
                    dprintf(" SCC [MEMORY_MODEL_FEATURE] accessed\n");
                    return *(SCC_RB[MAIN_MEMORY_MODEL_FEATURE_0]);
                    break;
                }
            }
            if(crm == 2){
                switch(opc2){
                case 0:
                    dprintf(" SCC [IS_ATTRIBUTES_0] accessed\n");
                    return *(SCC_RB[IS_ATTRIBUTES_0]);
                    break;
                case 1:
                    dprintf(" SCC [IS_ATTRIBUTES_1] accessed\n");
                    return *(SCC_RB[IS_ATTRIBUTES_1]);
                    break;
                case 2:
                    dprintf(" SCC [IS_ATTRIBUTES_2] accessed\n");
                    return *(SCC_RB[IS_ATTRIBUTES_2]);
                    break;
                case 3:
                    dprintf(" SCC [IS_ATTRIBUTES_3] accessed\n");
                    return *(SCC_RB[IS_ATTRIBUTES_3]);
                    break;
                case 4:
                    dprintf(" SCC [IS_ATTRIBUTES_4] accessed\n");
                    return *(SCC_RB[IS_ATTRIBUTES_4]);
                    break;
                }
            }
    }

}




void cp15::MCR(unsigned opc1, unsigned opc2, unsigned crn,
               unsigned crm, unsigned rt_value)
{
    dprintf("CP15 operation: opc1=%d, opc2=%d, crn=%d, crm=%d\n", opc1, opc2, crn, crm);
}
