// ram.h
// -
// This represents a generic RAM device used in the ARM SoC by
// Freescale iMX35.
//
// Author : Gabriel Krisman Bertazi   Date: Sep 16, 2012
#ifndef __RAM_H__
#define __RAM_H__

#include "peripheral.h"

#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "tzic.h"

//In this model, we mimic the behavior of an internal RAM device.
//
// This is the backend of storing instructions. This device is only responsible
// for recording and reading information. It doesn't handle Virtual Memory nor
// access control. All of that must be handled before this point.

// More info about this module:
// Please refer to iMX53 Reference Manual 487
//
class ram_module : public sc_module,  public peripheral {
private:
    tzic_module &tzic;
    unsigned *memory;
    uint32_t blockNumber; //In Bytes

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

  ram_module (sc_module_name name_, tzic_module &tzic_,uint32_t start_add,
              uint32_t end_add, uint32_t blockNumber_):
  sc_module(name_), peripheral(start_add, end_add),tzic(tzic_),
      blockNumber(blockNumber_) {
          //Initialize memory
          memory = new unsigned[blockNumber/4];


  }
  ~ram_module();


};


#endif
