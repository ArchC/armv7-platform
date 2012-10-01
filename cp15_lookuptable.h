#define __CP15_lookuptable__
#define __CP15_lookuptable__


#define CRN_SIZE  16
#define OPC1_SIZE 8
#define CRM_SIZE 16
#define OPC2_SIZE 8

//This lookup table remaps register to real accessing values used on
// MRC and MCR instructions
uint32_t regMap[CRN_SIZE][OPC1_SIZE][CRM_SIZE][OPC2_SIZE]
{
//Cr0
    {
        //OPC1 = 0
        {

    /*CRM=0*/ {MAIN_ID, NULL, NULL, TLB_TYPE, MAIN_ID,              NULL, MAIN_ID,MAIN_ID}
    /*CRM=1*/ {NULL,    NULL, NULL, NULL,     MEMORY_MODEL_FEATURE, NULL, NULL,   NULL   }
            /*CRM=2*/ 
            {
                
        
          
        }

    }

}
#endif
