// "rom.cpp" - ROM memory model
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

#include "rom.h"
#include "arm_interrupts.h"
#include "sys/mman.h"

extern bool DEBUG_ROM;
#define dprintf(args...) if(DEBUG_ROM){fprintf(stderr,args);}

rom_module::~rom_module ()
{
//    delete [] memory;
}

unsigned
rom_module::fast_read (unsigned address)
{
  unsigned datum = *((unsigned *) (((char *) data) + (address)));
  dprintf ("READ from %s local address: 0x%X Content: 0x%X\n",
	   this->name (), address, datum);
  return datum;
}

void
rom_module::fast_write (unsigned address, unsigned datum, unsigned offset)
{
  dprintf
    ("WARNING: Atempt to write to ROM device | %s physical address: 0x%X (offset: 0x%X) Content: 0x%X\n",
     this->name (), address, offset, datum);
}
