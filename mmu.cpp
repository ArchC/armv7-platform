#include<mmu.h>

extern bool DEBUG_MMU;

#define dprintf(args...) if(DEBUG_MMU){fprintf(stderr,args);}
#define isBitSet(variable, position) (((variable & (1 << (position))) != 0) ? true : false)
#define setBit(variable, position) variable = variable | (1 << (position))

#define WORD 4
#define HALF 2
#define BYTE 1

// ---------------------------------------------------------------------------
// Structs and functions to easily describe first level translation.
namespace L1
{
    // First Level Descriptor formats
    typedef enum  {fault, page_table, section, supersection, reserved} Type;

    typedef struct{
        uint32_t baseAddress;
        bool SBZ;
        bool NS;
        bool Domain[4];
    }PageTable;

    typedef struct{
        uint32_t baseAddress;
        bool NS;
        bool nG;
        bool AP[3];
        bool TEX[3];
        bool Domain[4];
        bool XN;
        bool C;
        bool B;
    }Section;

    typedef struct{
        char BaseAddress;
        char ExtbaseAddress;
        bool NS;
        bool nG;
        bool AP[3];
        bool XN;
        bool C;
        bool B;
        bool TEX[3];
    }Supersection;

    typedef struct{
        Type type;
        union{
            Supersection super;
            Section section;
            PageTable page;
        }data;
    } tableEntry;


    uint32_t getTableIndex(int n, uint32_t va)
    {
        int msb = 31-n;
        int lsb = 20;
        uint32_t mask = 0;
        for(int i = msb; i >= lsb; i--) setBit(mask, i);
        return (va & mask) >> lsb;
    }


    tableEntry tableWalk(MMU & mmu, uint32_t FLA)
    {
        ac_tlm_rsp rsp = mmu.talk_to_bus(READ,FLA, 0); //Read First level entry
        uint32_t data = rsp.data;
        L1::tableEntry entry;

        dprintf("Performing First-level table walk: FLA = 0x%X, FLD=0x%X. FLD is type ", FLA, data);
        switch(data & 0b11){  //Extract type bits[1:0]
        case 0:
            //FAULT
            entry.type = L1::fault;
            dprintf("[DATA FAULT]\n");
            break;
        case 1:
            //Page Table
            dprintf("[PAGE TABLE]\n");
            entry.type = L1::page_table;
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
                dprintf("[SUPERSECTION]\n");
                //Supersection
                entry.type                      = L1::supersection;
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
                dprintf("[SECTION]\n");
                entry.type = L1::section;
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
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Structs and functions to easily describe second level translation.
namespace L2
{
    typedef enum  {fault, small, large} Type;
    typedef struct{
        uint32_t baseAdd;
        bool nG;
        bool s;
        bool ap[3];
        bool tex[3];
        bool c;
        bool b;
        bool xn;
    }smallPage;

    typedef struct{
        uint32_t baseAdd;
        bool xn;
        bool tex[3];
        bool nG;
        bool s;
        bool ap[3];
        bool sbz[3];
        bool c;
        bool b;
    }largePage;


    typedef struct {
        Type type;
        union {
            smallPage smallpage;
            largePage largepage;
        }data;
    }tableEntry;

    uint32_t getTableIndex(uint32_t va)
    {
        int msb = 31-19;
        int lsb = 12;
        uint32_t mask = 0;
        for(int i = msb; i >= lsb; i--) setBit(mask, i);
        return (va & mask) >> lsb;
    }

    tableEntry tableWalk(MMU & mmu, uint32_t SLA)
    {
        ac_tlm_rsp rsp = mmu.talk_to_bus(READ,SLA, 0); //Read Second level entry
        uint32_t data = rsp.data;

        L2::tableEntry entry;

        dprintf("Performing second-level table walk: SLA = 0x%X, FLD=0x%X. page is type ", SLA, data);
        switch(data & 0b11)
        {
        case 0:
            entry.type = fault;
            dprintf("[DATA FAULT]\n");
            break;
        case 1:
            dprintf("[LARGE PAGE]\n");
            entry.type = large;
            entry.data.largepage.baseAdd = ((data & 0xFFFF0000) >> 16);
            entry.data.largepage.xn     = isBitSet(data, 15);
            entry.data.largepage.tex[2] = isBitSet(data, 14);
            entry.data.largepage.tex[1] = isBitSet(data, 13);
            entry.data.largepage.tex[0] = isBitSet(data, 12);
            entry.data.largepage.nG     = isBitSet(data, 11);
            entry.data.largepage.s      = isBitSet(data, 10);
            entry.data.largepage.ap[2]  = isBitSet(data, 9);
            entry.data.largepage.sbz[2] = isBitSet(data, 8);
            entry.data.largepage.sbz[1] = isBitSet(data, 7);
            entry.data.largepage.sbz[0] = isBitSet(data, 6);
            entry.data.largepage.ap[1]  = isBitSet(data, 5);
            entry.data.largepage.ap[0]  = isBitSet(data, 4);
            entry.data.largepage.c      = isBitSet(data, 3);
            entry.data.largepage.b      = isBitSet(data, 2);
            break;
        case 2: case 3:
            dprintf("[SMALL PAGE]\n");
            entry.type = small;
            entry.data.smallpage.baseAdd = ((data & 0xFFFFF000) >> 12);
            entry.data.smallpage.nG     = isBitSet(data, 11);
            entry.data.smallpage.s      = isBitSet(data, 10);
            entry.data.smallpage.ap[2]  = isBitSet(data, 9);
            entry.data.smallpage.tex[2] = isBitSet(data, 8);
            entry.data.smallpage.tex[1] = isBitSet(data, 7);
            entry.data.smallpage.tex[1] = isBitSet(data, 6);
            entry.data.smallpage.ap[1]  = isBitSet(data, 5);
            entry.data.smallpage.ap[0]  = isBitSet(data, 4);
            entry.data.smallpage.c      = isBitSet(data, 3);
            entry.data.smallpage.b      = isBitSet(data, 2);
            entry.data.smallpage.xn     = isBitSet(data, 0);
            break;
        }
        return entry;
    }
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
inline bool MMU::isActive()
{
    //MMU is active if bit 0 of SCC[ControlRegister] is active
    return (cop.SCC_RB[CTR].value & 0x1);
}

// This function returns the Translation Table Base address. It
//  chooses between TTBR0 and TTBR1 based upon TTBCR.N bit
// On TTBAdd buffer it returns the TTB address an its return is wich
// TTB was used
int MMU::getTTB(uint32_t *TTBAdd, uint32_t va)
{
    int used = 0;
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
                used = 1;
                break;
            }
        }
    }
    //Extract TTB address from TTBRx =>  TTBR[ 31: (14-TTBCR.N) ]
    int msb = 31;
    int lsb = 14 - n;
    uint32_t mask = 0;
    for(int i = msb; i >= lsb; i--) setBit(mask, i);

    *TTBAdd = (ttb & mask);
    return used;
}


//! Bus connection functions
ac_tlm_rsp MMU::talk_to_bus(const ac_tlm_req& req)
{
    return bus_port->transport(req);
}

ac_tlm_rsp MMU::talk_to_bus(ac_tlm_req_type type,unsigned address, unsigned datum )
{
    ac_tlm_req req;
    req.type = type;
    req.addr = address;
    req.data = datum;
    return bus_port->transport(req);
}

// ---------------------------------------------------------------------------
//! MMU Inteface Functions

ac_tlm_rsp MMU::transport(const ac_tlm_req& req)
{
    dprintf("|| MMU Operation: <> MMU is: ");
    if(!isActive()){
        dprintf("OFF (bypassing: Physical Address(PA) matches Virtual Address(VA)\n");
        return talk_to_bus(req); //Ignores MMU, route request directly to bus
    }
    dprintf("ON: VirtualAddress=0x%X\n");

    uint32_t pAdd = translateAddress(req.addr);
    return talk_to_bus(req.type, pAdd, req.data);
}

// ---------------------------------------------------------------------------
//! MMU Translation Functions

uint32_t MMU::translateAddress(uint32_t va)
{
    dprintf("MMU translating address: 0x%X\n", va);
    uint32_t ttb;

    int n = 0;
    if(getTTB(&ttb, va) == 0)   // Else N = 0
    {
        n= cop.MMU_RB[TTB_CTR].value & 0b111;   // Read TTBCR.N
        dprintf("TTB0 used | TTB address=0x%x | N == TTBCR.N == %d\n", ttb, n) ;
    }
    else {
        n = 0;
        dprintf("TTB1 used | TTB address=0x%x | N == %d\n", ttb, n) ;
    }

    //Prepare First Level Address (FLA)
    uint32_t l1_index = L1::getTableIndex(n,va);
    uint32_t FLA = ttb | (l1_index << 2);
    L1::tableEntry l1 = L1::tableWalk(*this, FLA);

    uint32_t pAdd = 0x0;

    if(l1.type == L1::section){
        pAdd = (l1.data.section.baseAddress << 20) | (va & 0xFFFFF);
    }
    else if( l1.type == L1::supersection){
        printf("This model does not support SuperSection tables.\n");
        exit(0);
        return va;
    }
    else if(l1.type == L1::fault){
        //Generate Translation Fault
        return va;
    }
    else if(l1.type == L1::reserved)
    {
        printf("Unexpected page entry type. Reserved.\n");
        return va;
    }
    else if(l1.type == L1::page_table)
    {
        //Prepare Second Level Address (SLA)
        uint32_t l2_index = L2::getTableIndex(va);
        uint32_t SLA = ((l1.data.page.baseAddress << 8) | l2_index) << 2;
        L2::tableEntry l2 = L2::tableWalk(*this,SLA);

        if(l2.type == L2::small)
        {
            pAdd = (l2.data.smallpage.baseAdd << 12) | (va & 0xFFF);
        }
        else if(l2.type == L2::large)
        {
            pAdd = (l2.data.largepage.baseAdd << 16) | (va & 0xFFFF);
        }
        else   /* Second Level page fault*/
        {
            // Exception?
        }
    }

    dprintf("End of translation. vAdd=0x%X ==>> pAdd = 0x%X\n", va, pAdd);
    return pAdd;
}
// ---------------------------------------------------------------------------


