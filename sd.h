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
// This model does not simulate SD protocol. That could be a future feature. this
// only acts as a backend flat memory for ESDHCv2 requests.


using namespace std;

typedef struct {
    uint32_t response[3];
}sd_response;



class sd_card : public sc_module{

private:
    void *data;
    int blocklen;

    sd_response cmd16_handler(uint32_t arg);

public:

    sd_response exec_cmd(short cmd_index, short cmd_type, uint32_t arg);
    sd_card (sc_module_name name_, const char* dataPath);
    ~sd_card();
};


#endif
