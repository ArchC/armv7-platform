// ccm.h
// -
// This is a dumb CCM reacting as described by iMX53 QSB iMXRM manual.
// It does not actually performs any action, since our model is not clock accurated.
// We just respond to Core as if the command was executed.
//
//
// Author : Gabriel Krisman Bertazi   Date: Fev 08,  2013
#ifndef CCM_H
#define CCM_H

#include "peripheral.h"

#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "tzic.h"


class ccm_module : public sc_module,  public peripheral {
private:
    tzic_module &tzic;

    static const uint32_t CCR     = 0x0;
    static const uint32_t CCDR    = 0x4;
    static const uint32_t CSR     = 0x8;
    static const uint32_t CCSR    = 0xC;
    static const uint32_t CACRR   = 0x10;
    static const uint32_t CBCDR   = 0x14;
    static const uint32_t CBCMR   = 0x18;
    static const uint32_t CSCMR1  = 0x1C;
    static const uint32_t CSCMR2  = 0x20;
    static const uint32_t CSCDR1  = 0x24;
    static const uint32_t CS1CDR  = 0x28;
    static const uint32_t CS2CDR  = 0x2C;
    static const uint32_t CDCDR   = 0x30;
    static const uint32_t CHSCCDR = 0x34;
    static const uint32_t CSCDR2  = 0x38;
    static const uint32_t CSCDR3  = 0x3C;
    static const uint32_t CSCDR4  = 0x40;
    static const uint32_t CDHIPR  = 0x48;
    static const uint32_t LAST_ADDR = 0x4C;
    uint32_t regs[LAST_ADDR/4];

    void reset();

  // Fast read/write don't implement error checking. The bus (or other caller)
  // must ensure the address is valid.
  // Invalid read/writes are treated as no-ops.
  // Unaligned addresses have undefined behavior
    unsigned fast_read(unsigned address);
    void fast_write(unsigned address, unsigned datum, unsigned offset);

public:
    ccm_module (sc_module_name name_, tzic_module &tzic_);
    ~ccm_module();

    unsigned read_signal(unsigned address, unsigned offset) {
        return fast_read(address);
    }

    void write_signal(unsigned address, unsigned datum, unsigned offset) {
        fast_write(address, datum, offset);
    }

};

#endif
