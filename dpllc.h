// dpllc.h
// -
// This is a dumb DPLLC reacting as described by iMX53 QSB iMXRM manual.
// It does not actually performs any action, since our model is not clock accurated.
// We just respond to Core as if the command was executed.
//
//
// Author : Gabriel Krisman Bertazi   Date: Fev 06,  2013
#ifndef __DPLLC_H__
#define __DPLLC_H__

#include "peripheral.h"

#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "tzic.h"


class dpllc_module : public sc_module,  public peripheral {
private:
    tzic_module &tzic;

    static const uint32_t CTL = 0x0;
    static const uint32_t CONFIG = 0x4;
    static const uint32_t LAST_ADDR = 0x2C;


    uint32_t regs[LAST_ADDR/4];

    void reset();

  // Fast read/write don't implement error checking. The bus (or other caller)
  // must ensure the address is valid.
  // Invalid read/writes are treated as no-ops.
  // Unaligned addresses have undefined behavior
    unsigned fast_read(unsigned address);
    void fast_write(unsigned address, unsigned datum, unsigned offset);

public:
    //Wrappers to call fast_read/write with correct parameters
    unsigned read_signal(unsigned address, unsigned offset) { return fast_read(address); }
    void write_signal(unsigned address, unsigned datum, unsigned offset) {fast_write(address, datum, offset); }

    dpllc_module (sc_module_name name_, tzic_module &tzic_);

//    sc_module(name_), peripheral(start_add, end_add),tzic(tzic_);
    ~dpllc_module();


};


#endif
