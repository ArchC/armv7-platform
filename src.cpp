#include "src.h"
#include "arm_interrupts.h"

extern bool DEBUG_SRC;
#define dprintf(args...) if (DEBUG_SRC){fprintf(stderr,args);}

src_module::src_module (sc_module_name name_, tzic_module &tzic_):
    sc_module(name_), tzic(tzic_) {

    reset(true);
}

src_module::~src_module(){}

unsigned src_module::fast_read(unsigned address)
{
    //Verify index out of range
    if(address > LAST_ADDRESS)
        return 0;

    switch(address)
        {
        case SRC_SBMR:
            return
                ((PINS::TEST_MODE[2] << 29) |
                 (PINS::TEST_MODE[1] << 28) |
                 (PINS::TEST_MODE[0] << 27) |
                 (PINS::BT_FUSE_SEL  << 26) |
                 (PINS::BMOD[1]      << 25) |
                 (PINS::BMOD[0]      << 24) |
                 (PINS::BOOT_CFG[2]  << 16) |
                 (PINS::BOOT_CFG[1]  <<  8) |
                 (PINS::BOOT_CFG[0]  <<  0));
            break;
        default:
            return regs[address/4];
        }
}

void src_module::fast_write(unsigned address, unsigned datum)
{
    switch (address)
    {
    case SRC_SCR:
        if(datum & 0b11110){
            dprintf("SRC: Atempted to reset"
                    "GPU/open_vg/IPU/VGU not implemented in this model\n");
        }

        regs[address/4] = datum & 0xFFF ;

        /* Clear reset bit for unimplemented stuff.  */
        regs[address/4] =  regs[address/4] & (~0b11110);
        break;

    case SRC_SRSR:
        /* Write 1 to clear.  */
        regs[address/4] = (regs[address/4] & ~datum);
        break;

    case SRC_SIMR:
        regs[address/4] = regs[address/4] | (datum & 0x0F);

    default:
        /* One cannot simply write to SBMR or SISR.  */
        break;
    }
}
