// imx53_bus.h
// -
// This represents the Imx53 Bus used in the ARM SoC by
// Freescale iMX35.
//
// Author : Rafael Auler,
//          Gabriel K. Bertazi.
//-------------------------------------------------------------------------

#ifndef __BUS_H__
#define __BUS_H__

#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "armv5e.H"
#include "gpt.h"
#include "tzic.h"
#include "ram.h"
#include "uart.h"
#include <stdarg.h>
#include <deque>

// Custom functional model / fast simulation for imx53 bus.
// This class has instances for all the models plugged into the soc bus and
// callback the appropriate method for each module by checking the transaction
// address. It does the comparison via fast hardwired IFs tailored for this
// platform.
class imx53_bus: public sc_module, public ac_tlm_transport_if {
private:
    deque<peripheral*> devices;

public:

    // signal data_abort/fetch_abort directly to the processor core
    sc_port<ac_tlm_transport_if> proc_port;

    imx53_bus(sc_module_name name_): sc_module(name_){};
    ~imx53_bus();

    void connectDevice(peripheral* device);
    ac_tlm_rsp transport(const ac_tlm_req& req);
};

#endif
