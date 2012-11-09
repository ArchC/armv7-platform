#include "rom.h"
#include "arm_interrupts.h"
#include "sys/mman.h"

extern bool DEBUG_ROM;

#include <stdarg.h>
static inline int dprintf(const char *format, ...) {
  int ret;
  if (DEBUG_ROM) {
    va_list args;
    va_start(args, format);
    ret = vfprintf(ac_err, format, args);
    va_end(args);
  }
  return ret;
}

rom_module::~rom_module()
{
//    delete [] memory;
}

unsigned rom_module::fast_read(unsigned address)
{
    unsigned datum = *((unsigned*)(((char*)data) + (address)));
    dprintf("READ from %s local address: 0x%X Content: 0x%X\n",
            this->name(), GetMemoryBegin()+address, datum);
    return datum;
}

void rom_module::fast_write(unsigned address, unsigned datum, unsigned offset)
{
    dprintf("WARNING: Atempt to write to ROM device | %s physical address: 0x%X (offset: 0x%X) Content: 0x%X\n",
            this->name(), address, offset, datum);
}

