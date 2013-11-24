// 'mmu.h' - Memory Management Unit model
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
// Author : Gabriel Krisman Bertazi, 10/05/2013
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef MMU_H
#define MMU_H

#include <cp15.h>
#include <stdarg.h>
#include <systemc.h>
#include "arm_bhv_macros.H"
#include "ac_stats_base.H"
#include "arm_interrupts.h"
#include "arm.H"
#include "bus.h"
#include "tlb.h"

#define WITH_TLB

// This code implements a minimal Memory Management Unit as specified by
// the ARMv7 architecture and the Cortex-A8 specification. The intent of
// this module is to provide a simulated ARMv7 Core with a realistic
// interface to handle translation of virtual to physical addresses at
// each memory access performed by the executing code.
// The translation must be transparent to the user code, in a way it
// must not be aware that any address translation is being performed
// during its execution.
//
// ARM specification defines the address translation as a two step
// process for small and large pages and a single step for Sections and
// Subsections pages. Both are handled by nested classes L1 & L2 which
// stands for Level 1 and Level 2 of the translation.
//
// One must notice that this model does not implement any kind of
// instruction/data cache nor any kind of TLB. Those are planned as
// future expansion of this model.

class MMU:public sc_module, public ac_tlm_transport_if
{
#ifdef WITH_TLB
 private:
  typedef tlb<5> tlb_t;

  tlb_t tlb_i;
  tlb_t tlb_d;
#endif // WITH_TLB

public:
  MMU (sc_module_name name_, cp15 & cop_, imx53_bus & bus_)
    : sc_module (name_), cop (cop_), bus_port (bus_) {};

  ac_tlm_rsp transport (const ac_tlm_req & req);
  ac_tlm_rsp talk_to_bus (const ac_tlm_req & req);
  ac_tlm_rsp talk_to_bus (ac_tlm_req_type type,
                          unsigned address, unsigned datum);

 private:
  //System and MMU Control Coprocessor
  cp15 & cop;

  // output TLM port
  sc_port < ac_tlm_transport_if > bus_port;

  // This method returns the Translate Table Base address. It
  // chooses between TTBR0 and TTBR1 based upon TTBCR.N bit
  // On TTBAdd buffer it returns the TTB address an its return is
  // which TTB was used.
  int ttb_address (uint32_t * TTBAdd, uint32_t va);

  // Verify whether MMU translation feature is active.
  // Translation is active if bit 0 of SCC[ControlRegister] is high.
  // If not active, transport method redirects every memory access
  // directly to bus.
  bool translation_active ();

  // This class implements the first translation level for a given
  // virtual address. It includes data structures that will be helpful
  // to easily describe the several kinds of page table entries. The
  // external interface of this class is provided by the function
  // translate, which receives a Virtual Address and returns the
  // pyshical address associated with it, given processor mode and
  // pagination tables.
  class L1
  {
    enum entry_type { FAULT, PAGE, SECTION, SUPERSECTION, RESERVED };

    struct page_table
    {
      uint32_t base_address;
      bool SBZ;
      bool NS;
      bool Domain[4];
    };

    struct section
    {
      uint32_t base_address;
      bool NS;
      bool nG;
      bool AP[3];
      bool TEX[3];
      bool domain[4];
      bool XN;
      bool C;
      bool B;
    };

    struct supersection
    {
      char base_address;
      char ext_base_address;
      bool NS;
      bool nG;
      bool AP[3];
      bool XN;
      bool C;
      bool B;
      bool TEX[3];
    };

    struct table_entry
    {
      enum entry_type type;
      union
      {
        struct supersection super;
        struct section section;
        struct page_table page;
      } data;
    };

    // Perform a first level page walk operation. We fetch the address from
    // memory and load it into the correct data structure.
    static struct table_entry table_walk (MMU & mmu, uint32_t fla);

  public:
    // Performs a first level translate for a given virtual address. This
    // method may generate a prefetch/data abort in the event of a table
    // entry fault.
    static uint32_t translate (MMU & mmu, uint32_t va);
  };

  // This class implements the second level of translation for a given
  // virtual address. It includes data structures that will be helpful
  // to easily describe both small and large page entries. The external
  // interface of this class is provided by the function translate,
  // which receives a Virtual Address and indexes of the first level
  // translation and returns the pyshical address associated with it,
  // given processor mode and pagination tables.
  class L2
  {
    enum entry_type { FAULT, SMALL, LARGE };

    struct small
    {
      uint32_t base_address;
      bool nG;
      bool s;
      bool ap[3];
      bool tex[3];
      bool c;
      bool b;
      bool xn;
    };

    struct large
    {
      uint32_t base_address;
      bool xn;
      bool tex[3];
      bool nG;
      bool s;
      bool ap[3];
      bool sbz[3];
      bool c;
      bool b;
    };

    struct table_entry
    {
      enum entry_type type;
      union
      {
        struct small small_page;
        struct large large_page;
      } data;
    };

    // Perform a second level page walk operation. We fetch the address
    // from memory and load it to the correct data structure.
    static table_entry table_walk (MMU & mmu, uint32_t SLA);

  public:
    // Perform a second level translation for Small/Large pages. Might
    // generate prefetch/data abort if access a page fault.
    static uint32_t translate (MMU & mmu, uint32_t va,
                                 uint32_t base_address);
  };
};

#endif // !MMU_H


