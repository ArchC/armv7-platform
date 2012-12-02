// rom.h
// -
// This represents a generic SD card device to be connected
// to ESDHCV in the ARM SoC by Freescale iMX35.
//
// Author : Gabriel Krisman Bertazi   Date: Sep 16, 2012
#ifndef __SD_H__
#define __SD_H__

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
//In this model, we mimic the behavior of an external SD card device.
//
// This is the backend of storing instructions. This device is only responsible
// for recording and reading information. It doesn't handle Virtual Memory nor
// access control. All of that must be handled before this point.

// More info about this module:
//

using namespace std;

class sd_card : public sc_module{

private:
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

    sd_card (sc_module_name name_, const char* dataPath): sc_module(name_)
    {
        if(dataPath == NULL) ///Case no SD provided
            return;

        int dataFile = open(dataPath, O_LARGEFILE);
        if(dataFile == -1){
            printf("Unable to load SD card file %s", dataPath);
            exit(1);
        }
        printf("ArchC: Loading SD card file: %s\n",dataPath);

        struct stat st;
        stat(dataPath, &st);
        size_t size = st.st_size;
        data = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, dataFile, 0);
        if(data == MAP_FAILED){
            printf("Unable to map SD card file  %s", dataPath);
            exit(1);
        }
        close(dataFile);
    };

    ~sd_card();
};


#endif
