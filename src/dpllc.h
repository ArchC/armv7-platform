// 'dpllc.h' - DPLLC unit model
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
// Author : Gabriel Krisman Bertazi, 06/02/2013
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef DPLLC_H
#define DPLLC_H

#include "peripheral.h"

#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "tzic.h"

// This is a dumb DPLLC reacting as described by iMX53 QSB iMXRM manual.
// It does not actually performs any action, since our model is not clock accurated.
// We just respond to Core as if the command was executed.

class dpllc_module:public sc_module, public peripheral
{
private:
  tzic_module & tzic;

  static const uint32_t CTL = 0x0;
  static const uint32_t CONFIG = 0x4;
  static const uint32_t LAST_ADDR = 0x2C;

  uint32_t regs[LAST_ADDR / 4];

  void reset ();

  // Fast read/write don't implement error checking. The bus (or other caller)
  // must ensure the address is valid.
  // Invalid read/writes are treated as no-ops.
  // Unaligned addresses have undefined behavior
  unsigned fast_read (unsigned address);
  void fast_write (unsigned address, unsigned datum, unsigned offset);

public:
  //Wrappers to call fast_read/write with correct parameters
  unsigned read_signal (unsigned address, unsigned offset)
  {
    return fast_read (address);
  }
  void write_signal (unsigned address, unsigned datum, unsigned offset)
  {
    fast_write (address, datum, offset);
  }

  dpllc_module (sc_module_name name_, tzic_module & tzic_);
};

#endif // !DPLLC_H
