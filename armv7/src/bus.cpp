// 'bus.cpp' - IP bus model
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
// Author : Rafael Auler,            10/08/2011
//          Gabriel Krisman Bertazi, 10/10/2012
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#include "bus.h"
#include "defines.H"

extern bool DEBUG_BUS;
#define dprintf(args...)                        \
  if(DEBUG_BUS)                                 \
    fprintf(stderr,args);

//Include a new device to array.
void
imx53_bus::connect_device (peripheral * device,
			   const uint32_t start_address,
			   const uint32_t end_address)
{
  devices[n_of_devices].device = device;
  devices[n_of_devices].start_address = start_address;
  devices[n_of_devices].end_address = end_address;
  n_of_devices++;
}


// Route a request to an specific device.
ac_tlm_rsp
imx53_bus::transport (const ac_tlm_req & req)
{
  ac_tlm_rsp ans;
  unsigned addr = req.addr;
  unsigned offset = (addr % 4) * 8;

#ifndef UNALIGNED_ACCESS_SUPPORT
  addr = (addr >> 2) << 2;
#endif

  ans.status = SUCCESS;

  if (offset)
    {
      dprintf (" ! Next bus access is misaligned. Byte offset: %d\n",
	       offset / 8);
    }

  for(int i = 0; i < n_of_devices; i++)
    {
      // dereference operator is overloaded so we need this trick.
      struct imx53_bus::mapped_device *cur = &(devices[i]);
      if (addr >= cur->start_address && addr <= cur->end_address)
	{
	  if (req.type == READ)
	    {
	      dprintf (" <--> BUS TRANSACTION: [READ] 0x%X\n", addr);

	      ans.data =
		devices[i].device->read_signal ((addr - devices[i].start_address),
                                                offset);

#ifndef UNALIGNED_ACCESS_SUPPORT
              if (offset)
                ans.data = ans.data >> offset;
#endif
              return ans;
	    }
	  else if (req.type == WRITE)
	    {
	      dprintf (" <--> BUS TRANSACTION: [WRITE] 0x%X @0x%X \n",
		       req.data, addr);

	      devices[i].device->write_signal ((addr - devices[i].start_address),
                                               req.data, offset);
	      return ans;
	    }
	}
    }

  // Fail - warn core about failure.
  ac_tlm_req abrt_req;
  abrt_req.type = READ;
  abrt_req.dev_id = 0;
  abrt_req.addr = 0;
  abrt_req.data = arm_impl::EXCEPTION_DATA_ABORT;
  proc_port->transport (abrt_req);
  ans.status = ERROR;

  return ans;
}
