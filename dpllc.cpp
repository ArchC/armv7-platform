#include "dpllc.h"
#include "arm_interrupts.h"


extern bool DEBUG_DPLLC;

#include <stdarg.h>

static inline int dprintf(const char *format, ...) {
  int ret;
  if (DEBUG_DPLLC) {
    va_list args;
    va_start(args, format);
    ret = vfprintf(ac_err, format, args);
    va_end(args);
  }
  return ret;
}


void dpllc_module::reset()
{
    regs[CTL/4]    = 0x00AA0223;
    regs[CONFIG/4] = 0x00AA0006;
}



dpllc_module::dpllc_module (sc_module_name name_, tzic_module &tzic_,uint32_t start_add,
                            uint32_t end_add):
    sc_module(name_), peripheral(start_add, end_add),tzic(tzic_)

{
    reset();
}


dpllc_module::~dpllc_module()
{

}

unsigned dpllc_module::fast_read(unsigned address)
{
    dprintf("DPLLC READ from  %s local address: 0x%X\n", this->name(), address);
    return regs[address/4];
}
void dpllc_module::fast_write(unsigned address, unsigned datum, unsigned offset)
{
    dprintf("DPLLC WRITE to %s local address: 0x%X (offset: 0x%X) Content: 0x%X\n",
            this->name(), address, offset, datum);

    switch(address)
    {
    case CTL:
        regs[CTL/4] = datum | 0x1;   //Already locks CTL. Mimic an executed
                                         //action behaviour.
        break;
    default:
        regs[address/4] = datum;
        break;
    }



}

