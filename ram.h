// 'ram.h' - RAM memory model
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
// Author : Gabriel Krisman Bertazi, 16/07/2012
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef RAM_H
#define RAM_H

#include "peripheral.h"
#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "tzic.h"

class ram_module:public sc_module, public peripheral
{
private:
  tzic_module & tzic;
  unsigned *memory;
  uint32_t blockNumber;		//In Bytes

  unsigned fast_read (unsigned address);
  void fast_write (unsigned address, unsigned datum, unsigned offset);

public:
    ram_module (const sc_module_name name_, tzic_module & tzic_,
		const uint32_t blockNumber_);
   ~ram_module ();

  int populate (char *file, unsigned start_address);

  // Wrapper read to implement peripheral interface with correct
  // parameters.
  unsigned read_signal (unsigned address, unsigned offset)
  {
    return fast_read (address);
  }

  // Wrapper to implement write peripheral interface with correct
  // parameters.
  void write_signal (unsigned address, unsigned datum, unsigned offset)
  {
    fast_write (address, datum, offset);
  }
};


#endif // !RAM_H.
