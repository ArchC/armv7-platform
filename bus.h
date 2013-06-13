// imx53_bus.h
// -
// This represents the imx53 Bus used in the ARM SoC by
// Freescale iMX35.
//
// Authors : Rafael Auler,
//           Gabriel K. Bertazi.
//-------------------------------------------------------------------------

#ifndef BUS_H
#define BUS_H

#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "arm.H"
#include "gpt.h"
#include "tzic.h"
#include "ram.h"
#include "uart.h"
#include <stdarg.h>
#include <deque>

/* Custom functional model fast simulation for imx53 bus.  This class
   has pointer to instances of all models plugged into the soc bus and
   callback the appropriate method for each module by checking the
   transaction address.  */

class imx53_bus: public sc_module, public ac_tlm_transport_if
{
private:

    /* Representation of a device attached to bus. It holds information of
       the bus connection to the peripheral device.  */
    struct mapped_device
    {
        /* Pointer to module instance.  */
        peripheral *device;

        /* Begin of device's address space.  */
        uint32_t start_address;

        /* End of device's address space.  */
        uint32_t end_address;
    };

    /* Data structure to hold every device attached to bus.  */
    std::deque<struct mapped_device> devices;

public:

    /* signal data_abort/fetch_abort directly to the processor core.  */
    sc_port<ac_tlm_transport_if> proc_port;

    imx53_bus(sc_module_name name_): sc_module(name_){};
    ~imx53_bus();

    void connect_device(peripheral *device, uint32_t start_address,
                        uint32_t end_address);

    ac_tlm_rsp transport(const ac_tlm_req& req);
};

#endif /* !BUS_H.  */

