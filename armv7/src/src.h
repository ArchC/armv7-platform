// 'src.h' - Dummy SRC  bus model
//
// Copyright (C) 2013 The ArchC team.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------
// Author : Gabriel Krisman Bertazi, 15/04/2013
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef SRC_H
#define SRC_H

#include "peripheral.h"
#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "tzic.h"
#include "pins.h"
#include <stdint.h>		// define types uint32_t, etc

// This represents a generic SRC device used in the ARM SoC by
// Freescale iMX35.

class src_module:public sc_module, public peripheral
{
private:
  tzic_module & tzic;

  static const unsigned SRC_SCR = 0x0;
  static const unsigned SRC_SBMR = 0x4;
  static const unsigned SRC_SRSR = 0x8;
  static const unsigned SRC_SISR = 0x14;
  static const unsigned SRC_SIMR = 0x18;
  static const unsigned LAST_ADDRESS = 0x18;
  uint32_t regs[LAST_ADDRESS / 4];

  void reset (bool hard = false)
  {
    //Have no real diference between hard and soft reset.
    regs[SRC_SCR / 4] = 0x00000D21;
    regs[SRC_SRSR / 4] = 0x00000001;
    regs[SRC_SISR / 4] = 0x00000001;
    regs[SRC_SIMR / 4] = 0x0000000F;
  }

  // Fast read/write don't implement error checking. The bus (or other caller)
  // must ensure the address is valid.
  // Invalid read/writes are treated as no-ops.
  // Unaligned addresses have undefined behavior
  unsigned fast_read (unsigned address);
  void fast_write (unsigned address, unsigned datum);

public:

  src_module (sc_module_name name_, tzic_module & tzic_);
  ~src_module ();

  //Wrappers to call fast_read/write with correct parameters
  unsigned read_signal (unsigned address, unsigned offset)
  {
    return fast_read (address);
  }
  void write_signal (unsigned address, unsigned datum, unsigned offset)
  {
    fast_write (address, datum);
  }
};

#endif
