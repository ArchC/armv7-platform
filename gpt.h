// 'gpt.h' - Generic Purpose timer model
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
// Author : Rafael Auler, 13/09/2011
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef GPT_H
#define GPT_H

#include "peripheral.h"
#include "tzic.h"

#include <systemc.h>
#include <ac_tlm_protocol.H>

// In this model, we mimic the behavior of the GPT IP for generating
// timer triggered interrupts, communicating directly with the TZIC IC via
// one of its input interrupt ports.
//
// GPT originally is clocked by SoC peripheral clock. This is ommitted
// in this functional simulation.
//
// More info about this module:
// Please refer to iMX53 Reference Manual page 1735
//
class gpt_module:public sc_module, public peripheral
{
private:

  static const unsigned GPT_CR = 0x0;	// GPT control register
  static const unsigned GPT_PR = 0x4;	// GPT prescaler register
  static const unsigned GPT_SR = 0x8;	// GPT status register
  static const unsigned GPT_IR = 0xC;	// GPT interrupt register
  static const unsigned GPT_OCR1 = 0x10;	// GPT output compare register 1
  static const unsigned GPT_OCR2 = 0x14;	// GPT output compare register 2
  static const unsigned GPT_OCR3 = 0x18;	// GPT output compare register 3
  static const unsigned GPT_ICR1 = 0x1C;	// GPT input capture register 1
  static const unsigned GPT_ICR2 = 0x20;	// GPT input capture register 2
  static const unsigned GPT_CNT = 0x24;	// GPT counter register
  static const unsigned GPT_LASTADDR = 0x28;

  // According to iMX53 SoC
  static const unsigned GPT_IRQNUM = 39;

  unsigned regs[GPT_LASTADDR / 4];

  // 32-bit counter
  unsigned counter;

  // 12-bit prescaler
  unsigned prescaler, prescaler_counter;

  enum selected_clock_t
  { CLK_OFF = 0, PERIPHERAL_CLK = 1, HI_FREQ = 2, EXTERNAL_CLK = 3,
    LOW_FREQ = 4
  };

  enum oc_operation_mode_t
  { OC_DISCONNECTED = 0, OC_TOGGLE = 1, OC_CLEAR = 2,
    OC_SET = 3, OC_LOWPULSE = 4
  };

  enum input_capture_t
  { IC_DISABLED = 0, IC_RISINGEDGE = 1, IC_FALLINGEDGE = 2,
    IC_BOTH = 3
  };

  enum counter_mode_t
  { M_RESTART = 0, M_FREERUN = 1 };

  selected_clock_t clock_src;
  oc_operation_mode_t om1, om2, om3;
  input_capture_t im1, im2;
  counter_mode_t counter_mode;
  bool stop_en;			// true if gpt is enabled in stop mode
  bool wait_en;			// true if gpt is enabled in wait mode
  bool dbg_en;			// true if gpt is enabled in dbg mode
  bool en_mode;			// true if gpt resets when re-enabled
  bool enabled;			// freezes counters if false

  void do_reset (bool hard_reset = true)
  {
    // Initial values
    counter = 0;
    prescaler = 0;
    prescaler_counter = 0;
    clock_src = CLK_OFF;
    om1 = om2 = om3 = OC_DISCONNECTED;
    im1 = im1 = IC_DISABLED;
    counter_mode = M_RESTART;
    if (hard_reset)
      {
	stop_en = false;
	wait_en = false;
	dbg_en = false;
	en_mode = false;
	enabled = false;
      }

    do_cmpout1 = false;
    do_cmpout2 = false;
    do_cmpout3 = false;
    *(regs + GPT_CR / 4) = 0;
    *(regs + GPT_PR / 4) = 0;
    *(regs + GPT_SR / 4) = 0;
    *(regs + GPT_IR / 4) = 0;
    *(regs + GPT_OCR1 / 4) = 0xFFFFFFFF;
    *(regs + GPT_OCR2 / 4) = 0xFFFFFFFF;
    *(regs + GPT_OCR3 / 4) = 0xFFFFFFFF;
    *(regs + GPT_ICR1 / 4) = 0;
    *(regs + GPT_ICR2 / 4) = 0;
    *(regs + GPT_CNT / 4) = 0;
  }

  // This port is used to send interrupts to the processor
  tzic_module & tzic;

  void set_output_pin (bool & output, oc_operation_mode_t mode);

  // Fast read/write don't implement error checking. The bus (or other caller)
  // must ensure the address is valid.
  // Invalid read/writes are treated as no-ops.
  // Unaligned addresses have undefined behavior
  unsigned fast_read (unsigned address);
  void fast_write (unsigned address, unsigned datum);


public:

  //Wrappers to call fast_read/write with correct parameters
  unsigned read_signal (unsigned address, unsigned offset)
  {
    return fast_read (address);
  }
  void write_signal (unsigned address, unsigned datum, unsigned offset)
  {
    fast_write (address, datum);
  }

  // This is the main process to simulate the IP behavior
  void prc_gpt ();

  // -- External signals
  // Two input capture with programmable edge trigger
  void ind_capin1 (bool deassert = false);
  void ind_capin2 (bool deassert = false);
  // Input external clk
  void ind_clkin (bool deassert = false)
  {
  }
  // Outpus
  bool do_cmpout1;
  bool do_cmpout2;
  bool do_cmpout3;

  SC_HAS_PROCESS (gpt_module);

gpt_module (sc_module_name name_, tzic_module & tzic_):
  sc_module (name_), tzic (tzic_)
  {

    // A SystemC thread never finishes execution, but transfers control back
    // to SystemC kernel via wait() calls.
    SC_THREAD (prc_gpt);

    do_reset ();
  }


};


#endif
