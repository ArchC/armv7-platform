#include "sd.h"
#include "arm_interrupts.h"
#include "sys/mman.h"

extern bool DEBUG_SD;
#define dprintf(args...) if(DEBUG_SD){fprintf(stderr,args);}


sd_card::sd_card (sc_module_name name_, const char* dataPath): sc_module(name_)
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

    current_state = IDLE;
    data_line_busy = false;

    // A SystemC thread never finishes execution, but transfers control back
    // to SystemC kernel via wait() calls.
    SC_THREAD(prc_sdcard);
};

sd_card::~sd_card()
{
}

// This function is used to copy a block from the storage device to data line bus
void sd_card::send_block_to_dataline(uint32_t offset)
{
    memcpy(data_line, &(((char*)data)[offset]), blocklen); //Copy data to buffer
    data_line_busy = true;

}

void sd_card::prc_sdcard()
{
    do{
        wait(1, SC_NS);
        dprintf("-------------------- SD CARD -------------------- \n");
        if(current_state == IDLE)
            continue;
        else if(current_state == READ_SINGLE)
        {
            dprintf("SD_card: Single block read: Sending data from address 0x%x to bus.\n", current_address);

            send_block_to_dataline(current_address);
            current_state = IDLE;
        }
        else {
            printf("SDCARD: current_state %d was not implemented in this model", current_state);
        }

    }while(1);
}


// ----------------------------------------------------------------------------------
//  External access to dataline
// ----------------------------------------------------------------------------------

// This function is used by external controllers to read the sd card IO buffer
// It doesn't check any data integrity.
bool sd_card::read_dataline(void *buffer, uint32_t len)
{
    if(data_line_busy)
    {
        memcpy(buffer, data_line, len);
        data_line_busy = false;
        return true;
    }
    return false;
}


// ----------------------------------------------------------------------------------
//  Command Handlers
// ----------------------------------------------------------------------------------


sd_response sd_card::exec_cmd(short cmd_index, short cmd_type, uint32_t arg)
{
    //Routes each command to its handler
    switch(cmd_index){
    case 16:
        return cmd16_handler(arg);
        break;
    case 17:
        return cmd17_handler(arg);
        break;
    default:
        printf("SD CARD: CMD%d not supported/implemented in this model");
        exit(1);
    }
}

// CMD16 ==>  SET_BLOCKLEN
sd_response sd_card::cmd16_handler(uint32_t arg)
{
    if(arg > 4098) {
        printf(" SDCARD: BLOCKLEN too high. Overflow internal Buffer. aborting\n");
        exit(1);
    }

    blocklen = arg;

    sd_response resp;
    resp.response[0] = 0;
    resp.response[1] = 0;
    resp.response[2] = 0;
    printf("CMD16");

    return resp;   // No error.

}

// CMD17 ==>  READ_SINGLEBLK
sd_response sd_card::cmd17_handler(uint32_t arg)
{
    current_address = arg;
    current_state = READ_SINGLE;
    printf("CMD17");
    sd_response resp;
    return resp;

}

