// "ram.cpp" - RAM memory model
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

#include "ram.h"
#include "arm_interrupts.h"
#include "defines.H"

#include <string>
extern bool DEBUG_RAM;
#define dprintf(args...) if(DEBUG_RAM){fprintf(stderr,args);}

ram_module::ram_module (const sc_module_name name_, tzic_module & tzic_,
			const uint32_t blockNumber_):
sc_module (name_),
tzic (tzic_),
blockNumber (blockNumber_)
{
  /* Allocate memory space.  */
  memory = new unsigned[blockNumber / 4];
}

ram_module::~ram_module ()
{

  delete[]memory;
}

unsigned
ram_module::fast_read (unsigned address)
{
  unsigned int data = *((unsigned *)(((char*)memory) + (address)));

  dprintf ("READ from %s local address: 0x%X Content: 0x%X\n",
	   this->name (), address, data);
  return data;
}

void
ram_module::fast_write (unsigned address, unsigned datum, unsigned offset)
{
  dprintf ("WRITE to %s local address: 0x%X (offset: 0x%X) Content: 0x%X\n",
	   this->name (), address, offset, datum);

#ifdef UNALIGNED_ACCESS_SUPPORT
  *((unsigned *) (((char*)memory) + address)) = datum;
#else
  unsigned old_data = 0;

  if (offset)
    {
      /* Missaligned read.  */
      old_data = fast_read (address);
      old_data &= (0xFFFFFFFF << (32 - offset)) >> (32 - offset);
      old_data |= ((datum << offset) >> offset) << offset;
      *(memory + address / 4) = old_data;
    }
  else
    {
      *(memory + address / 4) = datum;
    }
#endif

}

int
ram_module::populate (char *file, unsigned start_address)
{
  FILE *fd;
  struct stat st;

  printf ("ArchC: Populating device %s with %s. Starting at: 0x%X\n",
	  this->name (), file, start_address);

  fd = fopen (file, "rb");
  if (fd < 0)
    {
      fprintf (stderr, "ArchC: File: %s: %s", file, strerror (errno));
      exit (1);
    }

  if (stat (file, &st) < 0)
    {
      fprintf (stderr, "ArchC: File: %s, %s", file, strerror (errno));
      exit (1);
    }

  if (fread ((memory + (start_address / 4)), st.st_size, 1, fd) < 1)
    {
      fprintf (stderr, "ArchC: File: %s, %s", file, strerror (errno));
      exit (1);
    }
  fclose (fd);
  return 0;
}
