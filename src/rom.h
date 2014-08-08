// 'rom.h' - ROM memory model
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
// Author : Gabriel Krisman Bertazi, 15/07/2012
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef ROM_H
#define ROM_H

#include "peripheral.h"
#include <sys/stat.h>
#include <systemc.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <ac_tlm_protocol.H>
#include "tzic.h"
#include <string>
//In this model, we mimic the behavior of an internal RoM device.
//
// This is the backend of storing instructions. This device is only responsible
// for recording and reading information. It doesn't handle Virtual Memory nor
// access control. All of that must be handled before this point.

// More info about this module:

// Please refer to iMX53 Reference Manual 487
//

using namespace std;

class rom_module:public sc_module, public peripheral
{
private:
  tzic_module & tzic;
  void *data;

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

  rom_module (sc_module_name name_, tzic_module & tzic_,
	      const char *dataPath):sc_module (name_), tzic (tzic_)
  {

    if (!dataPath)
      {
        printf ("ArchC: You must provide a boot code.\n");
        exit (1);
      }

    printf ("ArchC: Reading flat binary file: %s\n", dataPath);

    int dataFile = open (dataPath, O_RDONLY);
    if (dataFile == -1)
      {
	printf ("Unable to load boot file %s\n", dataPath);
	exit (1);
      }
    struct stat st;
    stat (dataPath, &st);
    size_t size = st.st_size;
    data = mmap (NULL, size, PROT_READ, MAP_PRIVATE, dataFile, 0);
    if (data == MAP_FAILED)
      {
	printf ("Unable to map bootfile  %s", dataPath);
	exit (1);
      }
    close (dataFile);
  };

  ~rom_module ();


};

#endif // !ROM_H
