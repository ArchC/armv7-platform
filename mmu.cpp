#include<mmu.h>

extern bool DEBUG_MMU;

#define dprintf(args...) if(DEBUG_MMU){fprintf(stderr,args);}
#define isBitSet(variable, position) (((variable & (1 << (position))) != 0) ? true : false)
#define setBit(variable, position) variable = variable | (1 << (position))

#define WORD 4
#define HALF 2
#define BYTE 1

//FD
uint32_t getL1TableIndex(int n, uint32_t va);
// ---------------------------------------------------------------------------

inline bool MMU::isActive()
{
    //MMU is active if bit 0 of SCC[ControlRegister] is active
    return (cop.SCC_RB[CTR].value & 0x1);
}

// This function returns the Translation Table Base address. It
// automatically chooses between TTBR0 and TTBR1 based upon TTBCR.N bit
uint32_t MMU::getTTBAddress(uint32_t va)
{
    uint32_t ttb =0;
    int n = cop.MMU_RB[TTB_CTR].value & 0b111;   // Read TTBCR.N

    //Algorithm to choose between TTBR_x. Extracted from ARM Manual  P. B3-1321
    //If N == 0 then use TTBR0.
    //if N > 0 then:
    //   if bits[31:32-N] of the input VA are all zero then use TTBR0
    //   otherwise use TTBR1
    if(n == 0)
        ttb = cop.MMU_RB[TTB_0].value;
    else {
        ttb = cop.MMU_RB[TTB_0].value;
        for(int i=31; i >= 32-n;i--){
            if(isBitSet(va, i) == 1){
                ttb = cop.MMU_RB[TTB_1].value;
                break;
            }
        }
    }
    //Extract TTB address from TTBRx =>  TTBR[ 31: (14-TTBCR.N) ]
    int msb = 31;
    int lsb = 14 - n;
    uint32_t mask = 0;
    for(int i = msb; i >= lsb; i--) setBit(mask, i);
    return (ttb & mask);
}


// ---------------------------------------------------------------------------
//! Bus connection functions

inline void MMU::write_to_memory(int size, unsigned realAddress, unsigned data)
{
    switch(size){
    case WORD:
        mem_port.write(realAddress, data);
        break;
    case HALF:
        mem_port.write_half(realAddress, data);
        break;
    case BYTE:
        mem_port.write_byte(realAddress, data);
        break;
    }
}
inline uint32_t MMU::read_from_memory(int size, unsigned realAddress)
{
    switch(size){
    case WORD:
        return mem_port.read(realAddress);
        break;
    case HALF:
        return mem_port.read_half(realAddress);
        break;
    case BYTE:
        return mem_port.read_byte(realAddress);
        break;
    }
}

// ---------------------------------------------------------------------------
//! MMU Inteface Functions
void MMU::write(int size, uint32_t address, uint32_t data)
{
    dprintf("|| MMU Write request: <> MMU is: ");
    if(!isActive()){
        dprintf("OFF (bypassing: Physical Address(PA) matches Virtual Address(VA)) || VA=0x%X Data=0x%X\n",address, data);
        write_to_memory(size, address, data);
        return;
    }
    dprintf("ON || VA=0x%X Data=0x%X\n",address, data);
}

uint32_t MMU::read(int size, uint32_t address)
{
    dprintf("|| MMU Read  request: <> MMU is: ");
    if(!isActive()){
        dprintf("OFF (bypassing: Physical Address(PA) matches Virtual Address(VA)) || VA=0x%X\n",address);
        return read_from_memory(size, address);
    }
    dprintf("ON || VA=0x%X Data=0x%X\n",address);
    return 0;
}

// ---------------------------------------------------------------------------
//! MMU Translation Functions

uint32_t MMU::translateAddress(uint32_t va)
{
    int n = cop.MMU_RB[TTB_CTR].value & 0b111;   // Read TTBCR.N
    uint32_t ttb = getTTBAddress(va);

    //Prepare First Level Address (FLA)
    uint32_t l1_index = getL1TableIndex(n,va);
    uint32_t FLA = ttb | (l1_index << 2);
    L1_entry = L1_tableWalk(FLA);

    if(L1_entry.type == section || L1_entry.type == supersection){
        printf("This model does not support SuperSection/Section tables.");
        return va;
    }

    if(L1_entry.type == fault){
        //Generate Translation Fault
    }
    return 0;
}

//! First Level (L1)

// This function, given a Virtual Address, extracts L1
// Table Index
uint32_t getL1TableIndex(int n, uint32_t va)
{
    int msb = 31-n;
    int lsb = 20;
    uint32_t mask = 0;
    for(int i = msb; i >= lsb; i--) setBit(mask, i);
    return (va & mask) >> lsb;
}

L1_entry MMU::L1_tableWalk(uint32_t FLA)
{
    uint32_t data = read_from_memory(WORD, FLA);
    L1_entry entry;
    switch(data & 0b11){  //Extract type bits[1:0]
    case 0:
        //FAULT
        entry.type = fault;
    case 1:
        //Page Table
        entry.type = page_table;
        entry.data.page.baseAddress = (data & 0xFFFFFC00) >> 10;
        entry.data.page.SBZ = isBitSet(data, 4);
        entry.data.page.NS = isBitSet(data,  3);
        entry.data.page.Domain[0] = isBitSet(data, 5);
        entry.data.page.Domain[1] = isBitSet(data, 6);
        entry.data.page.Domain[2] = isBitSet(data, 7);
        entry.data.page.Domain[3] = isBitSet(data, 8);
        break;
    case 2: case 3:
        if(isBitSet(data, 18)) {
            //Supersection
            entry.type                      = supersection;
            entry.data.super.BaseAddress    = (data & 0xFF000000) >> 24;
            entry.data.super.ExtbaseAddress = (data & 0x1E0) >> 5;
            entry.data.super.NS        = isBitSet(data,19);
            entry.data.super.nG        = isBitSet(data,17);
            entry.data.super.AP[0]     = isBitSet(data,10);
            entry.data.super.AP[1]     = isBitSet(data,11);
            entry.data.super.AP[2]     = isBitSet(data,15);
            entry.data.super.TEX[0]    = isBitSet(data,12);
            entry.data.super.TEX[1]    = isBitSet(data,13);
            entry.data.super.TEX[2]    = isBitSet(data,14);
            entry.data.super.XN        = isBitSet(data, 4);
            entry.data.super.C         = isBitSet(data, 3);
            entry.data.super.B         = isBitSet(data, 2);
        }else {
            //Section
            entry.type = section;
            entry.data.section.baseAddress = (data & 0xFFF00000) >> 20;
            entry.data.section.NS          = isBitSet(data,19);
            entry.data.section.nG          = isBitSet(data,17);
            entry.data.section.AP[0]       = isBitSet(data,10);
            entry.data.section.AP[1]       = isBitSet(data,11);
            entry.data.section.AP[2]       = isBitSet(data,15);
            entry.data.section.TEX[0]      = isBitSet(data,12);
            entry.data.section.TEX[1]      = isBitSet(data,13);
            entry.data.section.TEX[2]      = isBitSet(data,14);
            entry.data.section.Domain[0]   = isBitSet(data, 5);
            entry.data.section.Domain[1]   = isBitSet(data, 6);
            entry.data.section.Domain[2]   = isBitSet(data, 7);
            entry.data.section.Domain[3]   = isBitSet(data, 8);
            entry.data.section.XN          = isBitSet(data, 4);
            entry.data.section.C           = isBitSet(data, 3);
            entry.data.section.B           = isBitSet(data, 2);
        }
        break;
    }
    return entry;
}
