#ifndef __MMU_H__
#define __MMU_H__

#include<cp15.h>
#include "armv5e_bhv_macros.H"
#include <stdarg.h>
#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "armv5e.H"
#include "bus.h"

// ---------------------------------------------------------------------------
// Structs to help understanding memory systems

// First Level Descriptor formats
typedef enum  {fault, page_table, section, supersection, reserved} L1_entry_type;

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
    L1_entry_type type;
    union{
        Supersection super;
        Section section;
        PageTable page;
    }data;
} L1_entry;



class MMU: public sc_module{
private:
    cp15  & cop;          //System and MMU Control Coprocessor
    ac_memory & mem_port; //output TLM port

    inline void write_to_memory(int, unsigned, unsigned);
    inline uint32_t read_from_memory(int, unsigned);

    uint32_t getTTBAddress(uint32_t va);
    uint32_t translateAddress(uint32_t va);

    L1_entry L1_tableWalk(uint32_t FLA);

public:
    bool isActive();

MMU(sc_module_name name_, cp15 & cop_, ac_memory & mem_port_): sc_module(name_),
        cop(cop_), mem_port(mem_port_){};


    void       write(int size, uint32_t address, uint32_t data);
    uint32_t   read (int size, uint32_t address);


};


#endif
