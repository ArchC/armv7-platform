// rom.h
// -
// This represents a generic ROM device used in the ARM SoC by
// Freescale iMX35.
//
// Author : Gabriel Krisman Bertazi   Date: Sep 16, 2012
#ifndef __ROM_H__
#define __ROM_H__

#include "peripheral.h"
#include <sys/stat.h>
#include <systemc.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <ac_tlm_protocol.H>
#include "tzic.h"
#include <string>
//In this model, we mimic the behavior of an internal RoM device.
//
// This is the backend of storing instructions. This device is only responsible
// for recording and reading information. It doesn't handle Virtual Memory nor
// access control. All of that must be handled before this point.

// More info about this module:

// Please refer to iMX53 Reference Manual 487
//

using namespace std;

class rom_module : public sc_module,  public peripheral {
private:
    tzic_module &tzic;
    void *data;

  // Fast read/write don't implement error checking. The bus (or other caller)
  // must ensure the address is valid.
  // Invalid read/writes are treated as no-ops.
  // Unaligned addresses have undefined behavior
    unsigned fast_read(unsigned address);
    void fast_write(unsigned address, unsigned datum, unsigned offset);


public:
    //Wrappers to call fast_read/write with correct parameters
    unsigned read_signal(unsigned address, unsigned offset) { return fast_read(address); }
    void write_signal(unsigned address, unsigned datum, unsigned offset) {fast_write(address, datum, offset); }

    rom_module (sc_module_name name_, tzic_module &tzic_, const char* dataPath,
                uint32_t start_add,
                uint32_t end_add): sc_module(name_),
        peripheral(start_add, end_add), tzic(tzic_)
    {

        printf("ArchC: Reading flat binary file: %s\n",dataPath);

        int dataFile = open(dataPath, O_RDONLY);
        if(dataFile == -1){
            printf("Unable to load boot file %s", dataPath);
            exit(1);
        }
        struct stat st;
        stat(dataPath, &st);
        size_t size = st.st_size;
        data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, dataFile, 0);
        if(data == MAP_FAILED){
            printf("Unable to map bootfile  %s", dataPath);
            exit(1);
        }
        close(dataFile);
    };

  ~rom_module();


};


#endif
