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

// SCR of a generic 4Gb card. Extracted from imx53 boot SD card.
const char sd_card::SCR[] = {0x02, 0x35, 0x80, 0x00,
                             0x01, 0x00, 0x00, 0x00};

sd_card::sd_card (sc_module_name name_, const char* dataPath): sc_module(name_)
{
    // Set current state to idle.
    this->current_state = SD_IDLE;

    // Set dataline empty.
    this->data_line_busy = false;

    // Set next command as non-application specific.
    this->application_specific_p = false;

    // Every RCA is initialized with zeroes.
    this->RCA = 0x0;

    this->card_selected_p = false;

    // A SystemC thread never finishes execution, but transfers control
    // back to SystemC kernel via wait() calls.
    SC_THREAD(prc_sdcard);

};

sd_card::~sd_card()
{
    //munmap: free data allocated by mmap
    if(munmap(data,data_size) != 0)
    {
        printf("Unable to free SD mmapped memory (Run to the hills!)");
    }
}

// Load a given binary image from disk to SD_card memory backend.  It
// attemps to mmap file passed as argument to data pointer.  On success
// defines data_size and returns 0. On failure returns -1.
// data pointes must be unmapped when no longer needed.
int sd_card::load_image_from_file(const char *file)
{
    int dataFile;
    struct stat st;

    // Check if image was provided.
    if(file == NULL)
        return -1;

    fprintf(stderr, "ArchC: Loading SD card file: %s\n", file);

    dataFile = open(file, O_LARGEFILE);
    if(dataFile == -1)
    {
        printf("Unable to load SD card file %s. Error: ",
               file, strerror(errno));
        return 1;
    }

    stat(file, &st);
    this->data_size = st.st_size;

    this->data = mmap(NULL, data_size, PROT_READ|PROT_WRITE,
                      MAP_PRIVATE, dataFile, 0);

    if(data == MAP_FAILED)
    {
        fprintf(stderr,"Unable to map SD card file  %s. Error: %s\n",
                file, strerror(errno));
        return 1;
    }

    close(dataFile);
    return 0;
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
    do {
        wait(1, SC_NS);
        dprintf("-------------------- SD CARD -------------------- \n");

        // Main FSM dispatcher.
        switch(current_state)
        {
        case SD_IDLE:
            break;

        case SD_DATA:
            exec_state_data();
            break;

        default:
            printf("%s: FSM State %d was not implemented in this model\n",
                   this->name(), current_state);
            exit(1);
        }

    } while (1);
}

// -------------------------------------------------------------------------------
//  Current state procedure
// -------------------------------------------------------------------------------
void sd_card::exec_state_data()
{
    dprintf("%s: Block Read: Sending data from address 0x%x to bus.\n",
            this->name(), current_address);

    if(data_line_busy)
        return; //Avoid overwritting unread data on dataline

    send_block_to_dataline(current_address);
    current_address += blocklen; //iterate to next block

    if(single_block_p == true) //If single read, stop it
        current_state = SD_IDLE;
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
    if(!application_specific_p)
    {
        //Routes each command to its handler
        switch(cmd_index)
        {
        case 0:
            return cmd0_handler(arg);
        case 2:
            return cmd2_handler(arg);
        case 3:
            return cmd3_handler(arg);
        case 7:
            return cmd7_handler(arg);
        case 8:
            return cmd8_handler(arg);
        case 9:
            return cmd9_handler(arg);
        case 12:
            return cmd12_handler(arg);
        case 16:
            return cmd16_handler(arg);
        case 17:
            return cmd17_handler(arg);
        case 18:
            return cmd18_handler(arg);
        case 55:
            return cmd55_handler(arg);
        default:
            printf("SD CARD: CMD%d not supported/implemented in this model\n",
                   cmd_index);
            exit(1);
        }
    }
    else
    {
        application_specific_p = false;
        //Routes each command to its handler
        switch(cmd_index)
        {
        case 41:
            return acmd41_handler(arg);
        case 51:
            return acmd51_handler(arg);
        default:
            printf("SD CARD: ACMD%d not supported/implemented in this model\n",
                   cmd_index);
            exit(1);
        }
    }
}

// CMD0 ==>  SET_CARD_TO_IDLE
struct sd_response sd_card::cmd0_handler(uint32_t arg)
{
    struct sd_response resp;

    dprintf("SD_CARD CMD0: arg=NOARG\n");
    current_state = SD_IDLE;
    data_line_busy = false;

    // No response. Just return anything.
    resp.type = R1;
    return resp;
}

// CMD2 ==>  SEND_CID
struct sd_response sd_card::cmd2_handler(uint32_t arg)
{
    struct sd_response resp;

    dprintf("SD_CARD CMD2: arg=NOARG\n");

    resp.type = R2;
    resp.response[0] = 0x3F;
    memcpy(&(resp.response[1]), CID, 16);
    return resp;
}

// CMD3 ==>  GENERATE_NEW_RCA
struct sd_response sd_card::cmd3_handler(uint32_t arg)
{
    struct sd_response resp;

    dprintf("SD_CARD CMD3: arg=NOARG\n");

    RCA = 1;

    resp.type = R6;
    resp.response[0] = 0x03;
    resp.response[1] = (RCA & 0xFF00)>>8;
    resp.response[2] = (RCA & 0x00FF);
    resp.response[3] = 0x0; //Dummy values for card status
    resp.response[3] = 0x0; // ^
    resp.response[4] = 0x1; // Fake CRC and end transmission

    return resp;
}

// CMD7 ==>  SELECT/DESELECT CARD
struct sd_response sd_card::cmd7_handler(uint32_t arg)
{
    struct sd_response resp;

    dprintf("SD_CARD CMD7: arg=arg (RCA)\n");

    if(arg == RCA && arg != 0x0)
        card_selected_p = true;
    else
        card_selected_p = false;

    resp.type = R1b;
    resp.response[0] = 0xC7;
    resp.response[1] = 0x00;
    resp.response[2] = 0x00;
    resp.response[3] = 0x00; //Dummy values for card status
    resp.response[3] = 0x00; // ^
    resp.response[4] = 0x01; // Fake CRC and end transmission

    return resp;
}


// CMD8 ==>  SEND_IF_COND
struct sd_response sd_card::cmd8_handler(uint32_t arg)
{
    struct sd_response resp;
    unsigned char check_pattern;
    unsigned char voltage;

    dprintf("SD_CARD CMD8: arg=0x%X\n", arg);

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

    dprintf("SD_CARD CMD9: arg=NOARG\n");

    resp.type = R2;
    resp.response[0] = 0x3F;
    memcpy(&(resp.response[1]), CSD, 16);
    return resp;
}

// CMD12 ==>  STOP_MULTIBLOCK READ
struct sd_response sd_card::cmd12_handler(uint32_t arg)
{
    dprintf("SD_CARD CMD12: arg=NOARG\n");
    current_state = SD_IDLE;
    struct sd_response aux;
    return aux;
}

// CMD16 ==>  SET_BLOCKLEN
struct sd_response sd_card::cmd16_handler(uint32_t arg)
{
    dprintf("SD_CARD CMD16: arg=0x%X\n", arg);
    if(arg > 4098) {
        fprintf(stderr, " SDCARD: BLOCKLEN too high. "
                "Overflow internal Buffer. aborting\n");
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
    current_state = SD_DATA;
    single_block_p = true;

        dprintf("SD_CARD CMD17: arg=0x%X\n", arg);
    struct sd_response resp;
    return resp;

}

// CMD18 ==>  READ_MULTIPLE_BLOCK
struct sd_response sd_card::cmd18_handler(uint32_t arg)
{
    dprintf("SD_CARD CMD18: arg=0x%X\n", arg);

    current_address = arg;
    current_state = SD_DATA;

    struct sd_response resp;
    return resp;

}

// CMD55 ==> APP_CMD
struct sd_response sd_card::cmd55_handler(uint32_t arg)
{
    struct sd_response resp;
    dprintf("SD_CARD CMD55: arg=0x%X. "
            "Next command must be application specific\n", arg);

    application_specific_p = true;

    resp.type = R1;
    return resp;
}
// ----------------------------------------------------------------------------
// -----------------------Application command handler--------------------------
// ----------------------------------------------------------------------------

struct sd_response sd_card::acmd41_handler(uint32_t arg)
{
    struct sd_response resp;

    dprintf("SD_CARD ACMD41: arg=%d\n", arg);

    resp.type = R3;
    resp.response[0] = 0x3F; //Begin of trasmission
    resp.response[1] = 0xFF; // Fake OCR
    resp.response[2] = 0xFF; //
    resp.response[3] = 0xFF; //
    resp.response[4] = 0xFF; // Last bit indicates we're not busy.
    resp.response[5] = 0xFF; //End of transmission
    return resp;
}

struct sd_response sd_card::acmd51_handler(uint32_t arg)
{
    struct sd_response resp;

    dprintf("SD_CARD ACMD51: arg=%d\n", arg);

    resp.type = R3;
    resp.response[0] = 0x3F; //Begin of trasmission
    resp.response[1] = 0xFF; // Fake OCR
    resp.response[2] = 0xFF; //
    resp.response[3] = 0xFF; //
    resp.response[4] = 0xFF; // Last bit indicates we're not busy.
    resp.response[5] = 0xFF; //End of transmission
    return resp;
}

