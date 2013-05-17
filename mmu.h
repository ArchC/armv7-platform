#ifndef __MMU_H__
#define __MMU_H__

#include <cp15.h>
#include "armv5e_bhv_macros.H"
#include <stdarg.h>
#include <systemc.h>
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "armv5e.H"
#include "bus.h"

class MMU: public sc_module, public ac_tlm_transport_if {
private:
    cp15 & cop;          //System and MMU Control Coprocessor
    sc_port<ac_tlm_transport_if> bus_port; //output TLM port

    int getTTB(uint32_t *TTBAdd, uint32_t va);
    uint32_t translateAddress(uint32_t va);
    bool isActive();
public:

MMU(sc_module_name name_, cp15 & cop_, imx53_bus & bus_):
    sc_module(name_), cop(cop_) {
      bus_port(bus_);
    };

    ac_tlm_rsp transport(const ac_tlm_req& req);
    ac_tlm_rsp talk_to_bus(ac_tlm_req_type type, unsigned address, unsigned datum);
    ac_tlm_rsp talk_to_bus(const ac_tlm_req& req);
};


#endif
