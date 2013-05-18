#include "src.h"
#include "arm_interrupts.h"

extern bool DEBUG_SRC;
#define dprintf(args...) if (DEBUG_SRC){fprintf(stderr,args);}

src_module::src_module (sc_module_name name_, tzic_module &tzic_, MODEPINS *pins_):
    sc_module(name_), tzic(tzic_), pins(pins_) {

    reset(true);
}

src_module::~src_module()
{
}

unsigned src_module::fast_read(unsigned address)
{
    if(address > LAST_ADDRESS)
        return 0;   //Index out of range
    switch(address)
        {
        case SRC_SBMR:
            if(!pins){
                printf("SRC: No MODEPIN loaded. Sending default 0x0");
                return 0x00;
            }
            return
                ((pins->TEST_MODE[2] << 29) |
                 (pins->TEST_MODE[1] << 28) |
                 (pins->TEST_MODE[0] << 27) |
                 (pins->BT_FUSE_SEL  << 26) |
                 (pins->BMOD[1]      << 25) |
                 (pins->BMOD[0]      << 24) |
                 (pins->BOOT_CFG[2]  << 16) |
                 (pins->BOOT_CFG[1]  <<  8) |
                 (pins->BOOT_CFG[0]  <<  0));
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
            dprintf("SRC: Atempted to reset GPU/open_vg/IPU/VGU not implemented in this model\n");
        }

        regs[address/4] = datum & 0xFFF ;
        regs[address/4] =  regs[address/4] & (~0b11110); // Clear reset bit for unimplemented stuff.
        break;

    case SRC_SRSR:
        regs[address/4] = (regs[address/4] & ~datum); // Write 1 to clear
        break;

    case SRC_SIMR:
        regs[address/4] = regs[address/4] | (datum & 0x0F);

    default:
        break; //One cannot simply write to SBMR or SISR
    }
}
