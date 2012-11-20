#include "sd.h"
#include "arm_interrupts.h"
#include "sys/mman.h"

extern bool DEBUG_SD;
#define dprintf(args...) if(DEBUG_SD){fprintf(stderr,args);}

sd_card::~sd_card()
{
}

unsigned sd_card::fast_read(unsigned address)
{
    unsigned datum = *((unsigned*)(((char*)data) + (address)));
    dprintf("READ from %s local address: 0x%X Content: 0x%X\n",
            this->name(), address, datum);
    return datum;
}

void sd_card::fast_write(unsigned address, unsigned datum, unsigned offset)
{
    dprintf("WRITE to %s local address: 0x%X (offset: 0x%X) Content: 0x%X\n",
            this->name(), address, offset, datum);
    unsigned *aux =  ((unsigned*)(((char*)data) + (address)));
    *aux = datum;
}

