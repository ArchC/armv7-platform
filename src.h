// ram.h
// -
// This represents a generic SRC device used in the ARM SoC by
// Freescale iMX35.
//
// Author : Gabriel Krisman Bertazi   Date: Apr, 2013
#ifndef SRC_H
#define SRC_H

#include "peripheral.h"
#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "tzic.h"
#include "pins.h"
#include <stdint.h> // define types uint32_t, etc

class src_module : public sc_module,  public peripheral {
private:
    tzic_module &tzic;
    MODEPINS *pins;

    static const unsigned SRC_SCR  = 0x0;
    static const unsigned SRC_SBMR = 0x4;
    static const unsigned SRC_SRSR = 0x8;
    static const unsigned SRC_SISR = 0x14;
    static const unsigned SRC_SIMR = 0x18;
    static const unsigned LAST_ADDRESS = 0x18;
    uint32_t regs[LAST_ADDRESS/4];

    void reset(bool hard=false)
    {
        //Have no real diference between hard and soft reset.
        regs[SRC_SCR/4]  = 0x00000D21;
        regs[SRC_SRSR/4] = 0x00000001;
        regs[SRC_SISR/4] = 0x00000001;
        regs[SRC_SIMR/4] = 0x0000000F;
    }

  // Fast read/write don't implement error checking. The bus (or other caller)
  // must ensure the address is valid.
  // Invalid read/writes are treated as no-ops.
  // Unaligned addresses have undefined behavior
    unsigned fast_read(unsigned address);
    void fast_write(unsigned address, unsigned datum);

 public:
    //Wrappers to call fast_read/write with correct parameters
    unsigned read_signal (unsigned address, unsigned offset) { return fast_read(address); }
    void write_signal (unsigned address, unsigned datum, unsigned offset) {fast_write(address, datum); }

    src_module (sc_module_name name_, tzic_module &tzic_, MODEPINS *pins_);
    ~src_module();

};

#endif
