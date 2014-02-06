
@ Position where stack starts. any position beetween beggining of iRAM
@ and this is reserved for stack.

        .set TEMP_STACK, 0xF801F000

@ Position where ESDHC backups response structure. 16 bytes

        .set ESDHC_RESPONSE_STRUCT, 0xF801F010

@ Position where SD CID is stored. 16 bytes
        .set SD_CID, 0xF801F020

@ Position where SD RCA is stored. 4 bytes
        .set SD_RCA, 0xF801F030

@ Initial Load Buffer. 2K bytes (0x800 bytes) (in Iram)
        .set INITIAL_LOAD_BUFFER, 0xF801F100

@ Initial load ivt position 
        .set IVT_POSITION,  INITIAL_LOAD_BUFFER+0x400

