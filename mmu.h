#ifndef __MMU_H__
#define __MMU_H__

#include<cp15.h>
#include <stdarg.h>
#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "armv5e.H"
#include "bus.h"
class MMU: public sc_module, public ac_tlm_transport_if {
    cp15 *cop;
    imx53_bus *mainBus;

public:

    bool isActive();

MMU(sc_module_name name_, cp15 *cop_, imx53_bus *mainBus_): sc_module(name_),
        cop(cop_), mainBus(mainBus_){};
    ac_tlm_rsp transport(const ac_tlm_req& req);

};


#endif
