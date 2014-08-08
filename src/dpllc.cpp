// "dpllc.cpp" - Dummy DPLLC model
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
// Author : Gabriel Krisman Bertazi
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#include "dpllc.h"
#include "arm_interrupts.h"
#include <stdarg.h>

extern bool DEBUG_DPLLC;
#define dprintf(args...)                        \
    if(DEBUG_DPLLC)                             \
        fprintf(stderr,args);

void
dpllc_module::reset ()
{
  regs[CTL / 4] = 0x00AA0223;
  regs[CONFIG / 4] = 0x00AA0006;
}

dpllc_module::dpllc_module (sc_module_name name_, tzic_module & tzic_):
sc_module (name_), tzic (tzic_)
{
  reset ();
}

unsigned
dpllc_module::fast_read (unsigned address)
{
  dprintf ("%s: READ Address: 0x%X\n", this->name (), address);

  return regs[address / 4];
}

void
dpllc_module::fast_write (unsigned address, unsigned datum, unsigned offset)
{
  dprintf ("%s: WRITE address: 0x%X (offset: 0x%X) Content: 0x%X\n",
	   this->name (), address, offset, datum);

  switch (address)
    {
    case CTL:
      //Already locks CTL. Mimic an executed
      //action behaviour.
      regs[CTL / 4] = datum | 0x1;
      break;

    default:
      regs[address / 4] = datum;
      break;
    }
}
