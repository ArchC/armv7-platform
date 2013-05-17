// ram.h
// -
// This represents a generic RAM device used in the ARM SoC by
// Freescale iMX35.
//
// Author : Gabriel Krisman Bertazi   Date: Sep 16, 2012
//
#ifndef RAM_H
#define RAM_H

#include "peripheral.h"
#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "tzic.h"

class ram_module : public sc_module,  public peripheral {
private:
    tzic_module &tzic;
    unsigned  *memory;
    uint32_t blockNumber; //In Bytes

    unsigned fast_read(unsigned address);
    void fast_write(unsigned address, unsigned datum, unsigned offset);

public:
    ram_module (const sc_module_name name_, tzic_module &tzic_,
                const uint32_t blockNumber_);
    ~ram_module();

    int populate(char *file, unsigned start_address);

    /* Wrapper read to implement peripheral interface with correct
       parameters.  */
    unsigned read_signal(unsigned address, unsigned offset){
        return fast_read(address);
    }

    /* Wrapper to implement write peripheral interface with correct
       parameters.  */
    void write_signal(unsigned address, unsigned datum, unsigned offset) {
        fast_write(address, datum, offset);
    }
};


#endif /* !RAM_H.  */
