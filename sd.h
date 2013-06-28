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
// This is the backend of storing instructions. This device is only
// responsible for recording and reading information.
// It doesn't handle Virtual Memory nor access control. All of that must
// be handled before this point.  using namespace std;

enum response_type{ R1=1, R1b, R1bCMD12, R2, R3, R4, R5, R5b, R6, R7};

struct sd_response
{
    enum response_type type;
    unsigned char response[17];
};

class sd_card : public sc_module
{
    enum sd_state
    {
        SD_IDLE,   // Idle State
        SD_READY,  // Ready State
        SD_IDENT,  // Identification State
        SD_STBY,   // Standby State
        SD_TRAN,   // Transfer State
        SD_DATA,   // Sending Data
        SD_RCV,    // Receive Data
        SD_PRG,    // Programming State
        SD_DIS,    // Disconnect State
        SD_INA     // Inactive State
    };

private:
    // FSM state pointer
    enum sd_state current_state;

    // Whether performing a single block operation.
    bool single_block_p;

    // Flag for treating next  command as application specific.
    bool application_specific_p;

    // Main pointer to SD memory on RAM
    void *data;

    // Size of SD card bin file
    size_t data_size;

    // Block Length defined by cmd16
    int blocklen;

    // Current accessed address
    uint32_t current_address;

    unsigned char data_line[4096]; // Data line buffer

    // CSD of a generic 4Gb card.
    static const char CSD[];
    // CID of a generic 4Gb card.
    static const char CID[];
    // SCR of a generic 4Gb card.
    static const char SCR[];

    // RCA - Relative card address
    uint16_t RCA;

    bool card_selected_p;

    void send_block_to_dataline(uint32_t offset);


    // -- State Handler --
    void exec_state_data();

    // -- Command Handlers --
    struct sd_response cmd0_handler (uint32_t arg);  // Set card to Idle.
    struct sd_response cmd2_handler (uint32_t arg);  // Send CID
    struct sd_response cmd3_handler (uint32_t arg);  // Publish new RCA
    struct sd_response cmd7_handler (uint32_t arg);  // Select/deselect card
    struct sd_response cmd8_handler (uint32_t arg);  // Send if_cond.
    struct sd_response cmd9_handler (uint32_t arg);  // Send CSD.
    struct sd_response cmd12_handler(uint32_t arg);  // Suspend transfer.
    struct sd_response cmd16_handler(uint32_t arg);  // Set_BlockLen.
    struct sd_response cmd17_handler(uint32_t arg);  // Read Single Block.
    struct sd_response cmd18_handler(uint32_t arg);  // Read Multiple Block.
    struct sd_response cmd55_handler(uint32_t arg);  // Next cmd is application
                                                     // specific.
    // -- Application command handler --
    struct sd_response acmd41_handler(uint32_t arg); //Get OCR
    struct sd_response acmd51_handler(uint32_t arg); //Send SCR
    // --

    // Set up backend memory by loading an binary image file from disk.
    int load_image_from_file(const char *file);
    
public:
    // -- External signals
    SC_HAS_PROCESS ( sd_card );
    void prc_sdcard ();

    sd_card (sc_module_name name_, const char* dataPath);
    ~sd_card();

    struct sd_response exec_cmd(short cmd_index, short cmd_type, uint32_t arg);

// This function is used by external controllers to read the sd card IO buffer
// It doesn't check any data integrity.
    bool read_dataline(std::queue<unsigned char> & buffer, uint32_t len);
    bool data_line_busy;           // Semaphor for data_line
};

#endif /* !SD_H.  */
