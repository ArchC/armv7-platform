// 'tzic.h' - TrustZone Interruption Controller model
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
// This represents the UART used in the ARM SoC by Freescale iMX35.
//
// Author : Rafael Auler, 12/09/2011
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef TZIC_H
#define TZIC_H

#include "peripheral.h"
#include <systemc.h>
#include <ac_tlm_protocol.H>

// In this model, we mimic the behavior of the TZIC IP for controlling
// interrupts in the freescale iMX53 SoC. A SystemC thread provides a loop,
// executed once per clock cycle, to update the status of the module.
// In this platform design, the ARM core runs one instruction, possibly
// interacting with external modules via the bus functional model. Then,
// SystemC kernel executes other modules threads (including this one), and it
// must prepare information so the next instruction of the ARM core may
// observe external modules responses.
//
// More info about this module:
// Please refer to iMX53 Reference Manual page 4373, or
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dto0013b/
// CHDFACIE.html
// (AMBA 3 TrustZone Interrupt Controller (SP890) Technical Overview)
//
class tzic_module:public sc_module, public peripheral
{
private:
  static const unsigned TZIC_INTCTRL = 0x0;	// Control register
  static const unsigned TZIC_INTTYPE = 0x4;	// Interrupt controller type register
  static const unsigned TZIC_PRIOMASK = 0xC;	// Priority mask register
  static const unsigned TZIC_SYNCCTRL = 0x10;	// Synchronizer control
  static const unsigned TZIC_DSMINT = 0x14;	// DSM interrupt holdoff
  static const unsigned TZIC_INTSEC0 = 0x80;	// Interrupt security registers
  static const unsigned TZIC_INTSEC1 = 0x84;
  static const unsigned TZIC_INTSEC2 = 0x88;
  static const unsigned TZIC_INTSEC3 = 0x8C;
  static const unsigned TZIC_ENSET0 = 0x100;	// Enable set registers
  static const unsigned TZIC_ENSET1 = 0x104;
  static const unsigned TZIC_ENSET2 = 0x108;
  static const unsigned TZIC_ENSET3 = 0x10C;
  static const unsigned TZIC_ENCLEAR0 = 0x180;	// Enable clear registers
  static const unsigned TZIC_ENCLEAR1 = 0x184;
  static const unsigned TZIC_ENCLEAR2 = 0x188;
  static const unsigned TZIC_ENCLEAR3 = 0x18C;
  static const unsigned TZIC_SRCSET0 = 0x200;	// Source set registers
  static const unsigned TZIC_SRCSET1 = 0x204;
  static const unsigned TZIC_SRCSET2 = 0x208;
  static const unsigned TZIC_SRCSET3 = 0x20C;
  static const unsigned TZIC_SRCCLEAR0 = 0x280;	// Source clear registers
  static const unsigned TZIC_SRCCLEAR1 = 0x284;
  static const unsigned TZIC_SRCCLEAR2 = 0x288;
  static const unsigned TZIC_SRCCLEAR3 = 0x28C;
  static const unsigned TZIC_PRIORITY0 = 0x400;	// Priority registers
  static const unsigned TZIC_PRIORITY1 = 0x404;	// each byte corresponds to
  static const unsigned TZIC_PRIORITY2 = 0x408;	// one of the 128 interrupts
  static const unsigned TZIC_PRIORITY3 = 0x40C;
  static const unsigned TZIC_PRIORITY4 = 0x410;
  static const unsigned TZIC_PRIORITY5 = 0x414;
  static const unsigned TZIC_PRIORITY6 = 0x418;
  static const unsigned TZIC_PRIORITY7 = 0x41C;
  static const unsigned TZIC_PRIORITY8 = 0x420;
  static const unsigned TZIC_PRIORITY9 = 0x424;
  static const unsigned TZIC_PRIORITY10 = 0x428;
  static const unsigned TZIC_PRIORITY11 = 0x42C;
  static const unsigned TZIC_PRIORITY12 = 0x430;
  static const unsigned TZIC_PRIORITY13 = 0x434;
  static const unsigned TZIC_PRIORITY14 = 0x438;
  static const unsigned TZIC_PRIORITY15 = 0x43C;
  static const unsigned TZIC_PRIORITY16 = 0x440;
  static const unsigned TZIC_PRIORITY17 = 0x444;
  static const unsigned TZIC_PRIORITY18 = 0x448;
  static const unsigned TZIC_PRIORITY19 = 0x44C;
  static const unsigned TZIC_PRIORITY20 = 0x450;
  static const unsigned TZIC_PRIORITY21 = 0x454;
  static const unsigned TZIC_PRIORITY22 = 0x458;
  static const unsigned TZIC_PRIORITY23 = 0x45C;
  static const unsigned TZIC_PRIORITY24 = 0x460;
  static const unsigned TZIC_PRIORITY25 = 0x464;
  static const unsigned TZIC_PRIORITY26 = 0x468;
  static const unsigned TZIC_PRIORITY27 = 0x46C;
  static const unsigned TZIC_PRIORITY28 = 0x470;
  static const unsigned TZIC_PRIORITY29 = 0x474;
  static const unsigned TZIC_PRIORITY30 = 0x478;
  static const unsigned TZIC_PRIORITY31 = 0x47C;
  static const unsigned TZIC_PND0 = 0xD00;	// Pending registers
  static const unsigned TZIC_PND1 = 0xD04;
  static const unsigned TZIC_PND2 = 0xD08;
  static const unsigned TZIC_PND3 = 0xD0C;
  static const unsigned TZIC_HIPND0 = 0xD80;	// High priority pending registers
  static const unsigned TZIC_HIPND1 = 0xD84;
  static const unsigned TZIC_HIPND2 = 0xD88;
  static const unsigned TZIC_HIPND3 = 0xD8C;
  static const unsigned TZIC_WAKEUP0 = 0xE00;	// Wakeup config registers
  static const unsigned TZIC_WAKEUP1 = 0xE04;
  static const unsigned TZIC_WAKEUP2 = 0xE08;
  static const unsigned TZIC_WAKEUP3 = 0xE0C;
  static const unsigned TZIC_SWINT = 0xF00;	// Software interrupt trigger
  static const unsigned TZIC_LASTADDR = 0xF04;

  unsigned regs[TZIC_LASTADDR / 4];

  // If not enabled, no interrupts are passed to the processor core
  bool enabled;
  // Indicates if needs process in this cycle
  bool changed;
  // Hardware interrupts input - asserted signals vector
  unsigned int_in[4];

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

  // This port is used to send interrupts to the processor
  sc_port < ac_tlm_transport_if > proc_port;

  bool wakeup_signal;

  // This is the main process to simulate the IP behavior
  void prc_tzic ();

  // Interrupt input. intnumber goes from 0 to 127
  void interrupt (unsigned intnumber, bool deassert = false);

  SC_HAS_PROCESS (tzic_module);

tzic_module (sc_module_name name_):sc_module (name_)
  {
    // A SystemC thread never finishes execution, but transfers control back
    // to SystemC kernel via wait() calls.
    SC_THREAD (prc_tzic);

    // Setting initial TZIC register initial values
    for (int i = 0, e = TZIC_LASTADDR / 4; i != e; ++i)
      {
	regs[i] = 0;
      }
    *(regs + TZIC_INTTYPE / 4) = 0x403;
    enabled = false;
    changed = true;
    int_in[0] = 0;
    int_in[1] = 0;
    int_in[2] = 0;
    int_in[3] = 0;
  }
};

#endif
