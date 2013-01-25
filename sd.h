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
#include <queue>
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

    typedef enum {
        READ_SINGLE,
        READ_MULTIPLE,
        WRITE_SINGLE,
        WRITE_MULTIPLE,
        IDLE
    } state;

private:

    state current_state;           // Handles a minimal finite state machine
    void *data;                    // Main pointer to SD memory on RAM
    int blocklen;                  // Block Length defined by cmd16
    uint32_t current_address;      // Current accessed address

    unsigned char data_line[4096]; // Data line buffer

    void send_block_to_dataline(uint32_t offset);

    // -- Command Handlers --
    sd_response cmd16_handler(uint32_t arg);  // Set_BlockLen
    sd_response cmd17_handler(uint32_t arg);  // Read Single Block
    // --

public:
    // -- External signals
    SC_HAS_PROCESS( sd_card );
    void prc_sdcard();

    sd_card (sc_module_name name_, const char* dataPath);
    ~sd_card();


    sd_response exec_cmd(short cmd_index, short cmd_type, uint32_t arg);

// This function is used by external controllers to read the sd card IO buffer
// It doesn't check any data integrity.
    bool read_dataline(void *buffer, uint32_t len);
    bool data_line_busy;           // Semaphor for data_line
};

#endif
