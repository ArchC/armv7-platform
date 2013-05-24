#include "ccm.h"
#include "arm_interrupts.h"
#include <stdarg.h>

extern bool DEBUG_CCM;
#define dprintf(args...) if(DEBUG_CCM){fprintf(stderr,args);}

void ccm_module::reset()
{
    regs[CCR/4]     = 0x000016FF;
    regs[CCDR/4]    = 0x00000000;
    regs[CSR/4]     = 0x00000010;
    regs[CCSR/4]    = 0x00000000;
    regs[CSR/4]     = 0x00000010;
    regs[CACRR/4]   = 0x00000000;
    regs[CBCDR/4]   = 0x00888945;
    regs[CBCMR/4]   = 0x00016154;
    regs[CSCMR1/4]  = 0xA6A2A020;
    regs[CSCMR2/4]  = 0x00B12F02;
    regs[CSCDR1/4]  = 0x00430318;
    regs[CS1CDR/4]  = 0x02860241;
    regs[CS2CDR/4]  = 0x00860041;
    regs[CDCDR/4]   = 0x143701d2;
    regs[CHSCCDR/4] = 0x00000000;
    regs[CSCDR2/4]  = 0x12080844;
    regs[CSCDR3/4]  = 0x00000041;
    regs[CSCDR4/4]  = 0x00000241;
    regs[CDHIPR/4]  = 0x00000000;
}

ccm_module::ccm_module (sc_module_name name_, tzic_module &tzic_):
    sc_module(name_), tzic(tzic_)
{
    reset();
}

ccm_module::~ccm_module()
{
}

unsigned ccm_module::fast_read(unsigned address)
{
    dprintf("READ from  %s local address: 0x%X\n", this->name(), address);
    return regs[address/4];
}
void ccm_module::fast_write(unsigned address, unsigned datum, unsigned offset)
{
    dprintf("WRITE to %s local address: 0x%X (offset: 0x%X) Content: 0x%X\n",
            this->name(), address, offset, datum);

    switch(address)
    {
    case CBCDR:
        regs[CBCDR/4] = datum;
        break;
    default:
        dprintf("Access to non-implemented CCM register: 0x%X\n", address);
    }
}

