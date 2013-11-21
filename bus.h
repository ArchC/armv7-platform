// 'bus.h' - IP bus model
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

#ifndef BUS_H
#define BUS_H

#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "peripheral.h"
#include "arm.H"
#include <stdarg.h>
#include <deque>

#define MAX_DEVICES 15

// This represents the imx53 Bus used in the ARM SoC by
// Freescale iMX35.

// Custom functional model fast simulation for imx53 bus.  This class
// has pointer to instances of all models plugged into the soc bus and
// callback the appropriate method for each module by checking the
// transaction address.

class imx53_bus:public sc_module, public ac_tlm_transport_if
{

  // Representation of a device attached to bus. It holds information of
  // the bus connection to the peripheral device.  */
  struct mapped_device
  {
    // Pointer to module instance.
    peripheral *device;

    //Begin of device's address space.
    uint32_t start_address;

    // End of device's address space.
    uint32_t end_address;
  };

  // Data structure to hold every device attached to bus.
  //std::deque < struct mapped_device >devices;
  struct mapped_device devices[MAX_DEVICES];

  int n_of_devices;
 public:

  // signal data_abort/fetch_abort directly to the processor core.
  sc_port < ac_tlm_transport_if > proc_port;

  imx53_bus (sc_module_name name_):sc_module (name_)
  {
    n_of_devices =0;
  };

  void connect_device (peripheral * device, const uint32_t start_address,
		       const uint32_t end_address);

  ac_tlm_rsp transport (const ac_tlm_req & req);
};

#endif // !BUS_H.
