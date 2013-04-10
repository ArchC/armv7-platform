#include "ram.h"
#include "arm_interrupts.h"


extern bool DEBUG_RAM;

#include <stdarg.h>
static inline int dprintf(const char *format, ...) {
  int ret;
  if (DEBUG_RAM) {
    va_list args;
    va_start(args, format);
    ret = vfprintf(ac_err, format, args);
    va_end(args);
  }
  return ret;
}

ram_module::~ram_module()
{
    delete [] memory;
}

unsigned ram_module::fast_read(unsigned address)
{
    unsigned data = *(memory + (address/4) );
    dprintf("READ from %s local address: 0x%X Content: 0x%X\n",
            this->name(), GetMemoryBegin(),address, data);
    return data;

}
void ram_module::fast_write(unsigned address, unsigned datum, unsigned offset)
{
    dprintf("WRITE to %s local address: 0x%X (offset: 0x%X) Content: 0x%X\n",
            this->name(), address, offset, datum);

    unsigned old_data = 0;
    if(offset){
        old_data = fast_read(address);
        old_data &= (0xFFFFFFFF << (32 - offset)) >> (32 - offset);
        old_data |= ((datum << offset) >> offset) << offset;
        *(memory + address/4) = old_data;
    } else {
        *(memory + address/4) = datum;
    }
}

int ram_module::populate(char *data_filepath, unsigned start_address)
{
    printf("ArchC: Populating device %s with %s starting at: 0x%X: %s\n",
           this->name(), data_filepath, start_address);

        int dataFile = open(data_filepath, O_RDONLY);
        if(dataFile == -1){
            printf("Unable to load data file %s", data_filepath);
            exit(1);
        }
        struct stat st;
        stat(data_filepath, &st);
        size_t size = st.st_size;

        read(dataFile,memory+(start_address/4), st.st_size);
        close(dataFile);

}
