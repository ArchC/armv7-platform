// sd.h
// -
// This represents a generic SD card device to be connected
// to ESDHCV in the ARM SoC by Freescale iMX35.
//
// Author : Gabriel Krisman Bertazi   Date: Sep 16, 2012
#ifndef SD_H
#define SD_H

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
//using namespace std;

enum response_type{ R1=1, R1b, R1bCMD12, R2, R3, R4, R5, R5b, R6, R7};
struct sd_response
{
    enum response_type type;
    unsigned char response[17];
};

class sd_card : public sc_module{

    typedef enum { IDLE, READ, READ_SINGLE, WRITE } state;

private:
    state current_state;           // Handles a minimal finite state machine
    void *data;                    // Main pointer to SD memory on RAM
    size_t data_size;              // Size of SD card bin file
    int blocklen;                  // Block Length defined by cmd16
    uint32_t current_address;      // Current accessed address

    unsigned char data_line[4096]; // Data line buffer

    // CSD of a generic 4Gb card.
    static const char CSD[];
    // CID of a generic 4Gb card.
    static const char CID[];

    void send_block_to_dataline(uint32_t offset);

    // -- Command Handlers --
    struct sd_response cmd0_handler (uint32_t arg);  // Set card to Idle.
    struct sd_response cmd8_handler (uint32_t arg);  // Send if_cond.
    struct sd_response cmd9_handler (uint32_t arg);  // Send CSD.
    struct sd_response cmd12_handler(uint32_t arg);  // Suspend transfer.
    struct sd_response cmd16_handler(uint32_t arg);  // Set_BlockLen.
    struct sd_response cmd17_handler(uint32_t arg);  // Read Single Block.
    struct sd_response cmd18_handler(uint32_t arg);  // Read Multiple Block.
    // --

public:
    // -- External signals
    SC_HAS_PROCESS( sd_card );
    void prc_sdcard();

    sd_card (sc_module_name name_, const char* dataPath);
    ~sd_card();

    struct sd_response exec_cmd(short cmd_index, short cmd_type, uint32_t arg);

// This function is used by external controllers to read the sd card IO buffer
// It doesn't check any data integrity.
    bool read_dataline(std::queue<unsigned char> & buffer, uint32_t len);
    bool data_line_busy;           // Semaphor for data_line
};

#endif /* !SD_H.  */
