#include "rom.h"
#include "arm_interrupts.h"
#include "sys/mman.h"

extern bool DEBUG_ROM;
#define dprintf(args...) if(DEBUG_ROM){fprintf(stderr,args);}

rom_module::~rom_module()
{
//    delete [] memory;
}

unsigned rom_module::fast_read(unsigned address)
{
    unsigned datum = *((unsigned*)(((char*)data) + (address)));
    dprintf("READ from %s local address: 0x%X Content: 0x%X\n",
            this->name(), address, datum);
    return datum;
}

void rom_module::fast_write(unsigned address, unsigned datum, unsigned offset)
{
    dprintf("WARNING: Atempt to write to ROM device | %s physical address: 0x%X (offset: 0x%X) Content: 0x%X\n",
            this->name(), address, offset, datum);
}

