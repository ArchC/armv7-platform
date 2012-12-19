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
};



sd_card::~sd_card()
{
}

sd_response sd_card::exec_cmd(short cmd_index, short cmd_type, uint32_t arg)
{
    //Routes each command to its handler
    switch(cmd_index){
    case 16:
        return cmd16_handler(arg);
        break;
    default:
        printf("SD CARD: CMD%d not supported/implemented in this model");
        exit(0);
    }
}



// CMD16 ==>  SET_BLOCKLEN
sd_response sd_card::cmd16_handler(uint32_t arg)
{
    blocklen = arg;

    sd_response resp;
    resp.response[0] = 0;
    resp.response[1] = 0;
    resp.response[2] = 0;
    return resp ;   // No error.

}
