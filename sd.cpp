#include "sd.h"
#include "arm_interrupts.h"
#include <errno.h>

extern bool DEBUG_SD;
#define dprintf(args...) if(DEBUG_SD){fprintf(stderr,args);}


// CSD of a generic 4Gb card. Extracted from imx53 boot SD card.
const char sd_card::CSD[] = {0x40, 0x0e, 0x00, 0x32,
                             0x5b, 0x59, 0x00, 0x00,
                             0x1d, 0x6f, 0x7f, 0x80,
                             0x0a, 0x40, 0x00, 0x00};

// CID of a generic 4Gb card. Extracted from imx53 boot SD card.
const char sd_card::CID[] = {0x02, 0x54, 0x4d, 0x53,
                             0x41, 0x30, 0x34, 0x47,
                             0x06, 0x23, 0x65, 0xd7,
                             0xfd, 0x00, 0xb2, 0x00};

sd_card::sd_card (sc_module_name name_, const char* dataPath): sc_module(name_)
{
    if(dataPath == NULL) ///Case no image provided
        return;

    int dataFile = open(dataPath, O_LARGEFILE);
    if(dataFile == -1){
        printf("Unable to load SD card file %s. Error: ",
               dataPath, strerror(errno));
        exit(1);
    }
    fprintf(stderr, "ArchC: Loading SD card file: %s\n", dataPath);

    struct stat st;
    stat(dataPath, &st);
    data_size = st.st_size;

    data = mmap(NULL, data_size, PROT_READ|PROT_WRITE,
                MAP_PRIVATE, dataFile, 0);

    if(data == MAP_FAILED)
    {
        fprintf(stderr,"Unable to map SD card file  %s. Error: %s\n",
                dataPath, strerror(errno) );
        exit(1);
    }
    close(dataFile);

    current_state = IDLE;
    data_line_busy = false;

    // A SystemC thread never finishes execution, but transfers control
    // back to SystemC kernel via wait() calls.
    SC_THREAD(prc_sdcard);
};

sd_card::~sd_card()
{ //munmap: free data allocated by mmap
    if(munmap(data,data_size) != 0)
    {
        printf("Unable to free SD mmapped memory (Run to the hills!)");
    }
}

// This function is used to copy a block from the storage device to data
// line bus
void sd_card::send_block_to_dataline(uint32_t offset)
{
    //Copy data to buffer
    memcpy(data_line, &(((char*)data)[offset]), blocklen);
    data_line_busy = true;
}

void sd_card::prc_sdcard()
{
    do{
        wait(1, SC_NS);
        dprintf("-------------------- SD CARD -------------------- \n");

        if(current_state == IDLE)
            continue;
        else if(current_state == READ || current_state == READ_SINGLE)
        {
            dprintf("SD_card: Block Read: Sending data from address 0x%x to bus.\n", current_address);
            if(data_line_busy)
                continue; //Avoid overwritting unread data on dataline

            send_block_to_dataline(current_address);
            current_address += blocklen; //iterate to next block

            if(current_state == READ_SINGLE) //If single read, stop it
                current_state == IDLE;
        }
        else {
            printf("SDCARD: current_state %d was not implemented in this model", current_state);
        }
    }while(1);
}

// -------------------------------------------------------------------------------
//  External access to dataline
// -------------------------------------------------------------------------------

// This function is used by external controllers to read the sd card IO buffer
// It doesn't check any data integrity.
bool sd_card::read_dataline(std::queue<unsigned char> & buffer, uint32_t len)
{
    if(data_line_busy)
    {
        for(int i=0; i < blocklen; i++)
            buffer.push(data_line[i]);

        data_line_busy = false; //Dataline is cleared for new data.
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
//  Command Handlers
// ----------------------------------------------------------------------------

struct sd_response sd_card::exec_cmd(short cmd_index, short cmd_type,
                                     uint32_t arg)
{
    //Routes each command to its handler
    switch(cmd_index)
    {
        case 0:  return cmd0_handler(arg);
        case 8:  return cmd8_handler(arg);
        case 9:  return cmd9_handler(arg);
        case 12: return cmd12_handler(arg);
        case 16: return cmd16_handler(arg);
        case 17: return cmd17_handler(arg);
        case 18: return cmd18_handler(arg);
        default:
            printf("SD CARD: CMD%d not supported/implemented in this model\n",
                   cmd_index);
            exit(1);
    }
}

// CMD0 ==>  SET_CARD_TO_IDLE
struct sd_response sd_card::cmd0_handler(uint32_t arg)
{
    struct sd_response resp;

    dprintf("SD_CARD CMD0: arg=NOARG");
    current_state = IDLE;
    data_line_busy = false;

    // No response. Just return anything.
    resp.type = R1;
    return resp;
}

// CMD8 ==>  SEND_IF_COND
struct sd_response sd_card::cmd8_handler(uint32_t arg)
{
    struct sd_response resp;
    unsigned char check_pattern;
    unsigned char voltage;

    dprintf("SD_CARD CMD8: arg=0x%X", arg);

    // Decode argument.
    check_pattern = arg & 0xFF;
    voltage = (arg & 0xF00) >> 8;

    // Build response string.
    resp.type = R7;
    resp.response[0] = 0x48;// cmdindx = 8
    resp.response[1] = 0x00; // Reserved
    resp.response[2] = 0x00; // Reserved
    resp.response[3] = voltage; // Voltage supplied
    resp.response[4] = check_pattern; //Check pattern
    resp.response[5] = 0x01; //Fake CRC and end bit.
    return resp;
}


// CMD9 ==>  SEND_CSD
struct sd_response sd_card::cmd9_handler(uint32_t arg)
{
    struct sd_response resp;

    dprintf("SD_CARD CMD9: arg=NOARG");

    resp.type = R2;
    resp.response[0] = 0x3F;
    memcpy(&(resp.response[1]), CSD, 16);
    return resp;
}

// CMD12 ==>  STOP_MULTIBLOCK READ
struct sd_response sd_card::cmd12_handler(uint32_t arg)
{
    dprintf("SD_CARD CMD12: arg=NOARG");
    current_state = IDLE;
    struct sd_response aux;
    return aux;
}


// CMD16 ==>  SET_BLOCKLEN
struct sd_response sd_card::cmd16_handler(uint32_t arg)
{
    dprintf("SD_CARD CMD16: arg=0x%X", arg);
    if(arg > 4098) {
        fprintf(stderr, " SDCARD: BLOCKLEN too high."
                " Overflow internal Buffer. aborting\n");
        exit(1);
    }

    blocklen = arg;

    struct sd_response resp;
    resp.response[0] = 0;
    resp.response[1] = 0;
    resp.response[2] = 0;

    return resp;   // No error.
}

// CMD17 ==>  READ_SINGLEBLK
struct sd_response sd_card::cmd17_handler(uint32_t arg)
{
    current_address = arg;
    current_state = READ_SINGLE;
        dprintf("SD_CARD CMD17: arg=0x%X", arg);
    struct sd_response resp;
    return resp;

}

// CMD18 ==>  READ_MULTIPLE_BLOCK
struct sd_response sd_card::cmd18_handler(uint32_t arg)
{
    current_address = arg;
    current_state = READ;
    dprintf("SD_CARD CMD18: arg=0x%X", arg);
    struct sd_response resp;
    return resp;

}

