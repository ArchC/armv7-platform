#include <systemc.h>
#include <ac_tlm_protocol.H>

SC_MODULE(interrupt_generator) {

  sc_port<ac_tlm_transport_if> proc_port;

  void prc_interrupt_generator();

  SC_CTOR(interrupt_generator) {
    SC_THREAD(prc_interrupt_generator);
  }


};
