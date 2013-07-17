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

extern bool DEBUG_BUS;
#define dprintf(args...)\
    if(DEBUG_BUS) \
        fprintf(stderr,args);

//Include a new device to array.
void
imx53_bus::connect_device (peripheral * device,
			   const uint32_t start_address,
			   const uint32_t end_address)
{
  struct imx53_bus::mapped_device new_connection;

  new_connection.device = device;
  new_connection.start_address = start_address;
  new_connection.end_address = end_address;

  devices.push_back (new_connection);
}


// Route a request to an specific device.
ac_tlm_rsp
imx53_bus::transport (const ac_tlm_req & req)
{

  std::deque < struct mapped_device >::iterator it = devices.begin ();
  struct imx53_bus::mapped_device * cur;

  ac_tlm_rsp ans;
  unsigned addr = req.addr;
  unsigned offset = (addr % 4) * 8;
  addr = (addr >> 2) << 2;
  ans.status = SUCCESS;
  if (offset)
    {
      dprintf (" ! Next bus access is misaligned. Byte offset: %d\n",
	       offset / 8);
    }

  while (it != devices.end ())
    {
      // dereference operator is overloaded so we need this trick.
      cur = &(*it);
      it++;
      if (addr >= cur->start_address && addr <= cur->end_address)
	{
	  if (req.type == READ)
	    {
	      dprintf (" <--> BUS TRANSACTION: [READ] 0x%X\n", addr);

	      ans.data =
		cur->device->read_signal ((addr - cur->start_address),
					  offset);
	      if (offset)
		{
		  ans.data = ans.data >> offset;
		}
	      return ans;
	    }
	  else if (req.type == WRITE)
	    {
	      dprintf (" <--> BUS TRANSACTION: [WRITE] 0x%X @0x%X \n",
		       req.data, addr);

	      cur->device->write_signal ((addr - cur->start_address),
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
