#ifndef __CP15_H__
#define __CP15_H__
#include "coprocessor.h"
#include<stdint.h>

//System Control and Configuration Index table
#define CONTROL                             0
#define AUXILIARY_CONTROL                   1
#define SECURE_CONFIGURATION                2
#define SECURE_DEBUG_ENABLE                 3
#define NONSECURE_ACCESS_CONTROL            4
#define COPROCESSOR_ACCESS_CONTROL          5
#define SECURE_OR_NONSECURE_VECTOR_BASE     6
#define MONITOR_VECTOR_BASE_ADDRESS         7
#define MAIN_ID                             8
#define SILICON_ID                          9
#define MEMORY_MODEL_FEATURE_0             10
#define IS_ATTRIBUTES_0                    11
#define IS_ATTRIBUTES_1                    12
#define IS_ATTRIBUTES_2                    13
#define IS_ATTRIBUTES_3                    14
#define IS_ATTRIBUTES_4                    15
#define IS_ATTRIBUTES_5                    16
#define IS_ATTRIBUTES_6                    17
#define IS_ATTRIBUTES_7                    18

//Memory Management Unit Control and Configuration Index table
#define TLB_TYPE                            0
#define TRANSLATION_TABLE_BASE_0            1
#define TRANSLATION_TABLE_BASE_1            2
#define TRANSLATION_TABLE_BASE_CONTROL      3
#define DOMAIN_ACCESS_CONTROL               4
#define DATA_FAULT_STATUS                   5
#define AUXILIARY_FAULT_STATUS              6
#define INSTRUCTION_FAULT_STATUS            7
#define INSTRUCTION_FAULT_ADDRESS           8
#define DATA_FAULT_ADDRESS                  9
#define TLB_OPERATIONS                     10
#define MEMORY_REGION_REMAP                11
#define CONTEXT_ID                         12
#define FCSE_PID                           13
#define THREAD_AND_PROCESS_ID              14

class cp15: public coprocessor{

private:
    uint32_t SCC_RB[18];  //System Control and Configuration register bank
    uint32_t MMU_RB[15];  //Memory Management Unit Control and Configuration register bank

    void reset();
public:
    void MCR(unsigned opc1, unsigned opc2, unsigned crn,
          unsigned crm, unsigned rt_value);

  cp15();
 ~cp15();
};

#endif
