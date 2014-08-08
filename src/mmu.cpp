// "mmu.cpp" - Memory Management Unit model
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
// Author : Gabriel Krisman Bertazi
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#include<mmu.h>

// TLB is still not complete. It might present some issues when multiprocessing.
//So, lets keep it down for now.

#undef WITH_TLB

extern bool DEBUG_MMU;

#define dprintf(args...)                        \
  if (DEBUG_MMU)                                \
    fprintf(stderr,args);

#define isBitSet(variable, position)                            \
  (((variable & (1 << (position))) != 0) ? true : false)

#define setBit(variable, position)              \
  variable = variable | (1 << (position))

// Verify whether MMU translation is active.
// Translation is active if bit 0 of SCC[ControlRegister] is high.
inline bool
MMU::translation_active ()
{
  return (cop.registers[cp15::CONTROL].value & 0x1);
}

// This method returns the Translate Table Base address. It
//  chooses between TTBR0 and TTBR1 based upon TTBCR.N bit
// On TTBAdd buffer it returns the TTB address an its return is wich
// TTB was used
int
MMU::ttb_address (uint32_t * TTBAdd, uint32_t va)
{
  const int MSB = 31;
  const int LSB = 14;

  uint32_t ttbcr_n;
  uint32_t mask = 0;
  int used = 0;
  uint32_t ttb = 0;

  // Read TTBCR.N
  ttbcr_n = cop.registers[cp15::TRANSLATION_TABLE_BASE_CONTROL].value & 0b111;

  //Algorithm to choose between TTBR_x. Extracted from ARM Manual  P. B3-1321
  //If N == 0 then use TTBR0.
  //if N > 0 then:
  //   if bits[31:32-N] of the input VA are all zero then use TTBR0
  //   otherwise use TTBR1

  ttb = cop.registers[cp15::TRANSLATION_TABLE_BASE_0].value;
  if (ttbcr_n != 0)
    {
      for (int i = 31; i >= ((MSB + 1) - ttbcr_n); i--)
        {
          if (isBitSet (va, i) == 1)
            {
              ttb = cop.registers[cp15::TRANSLATION_TABLE_BASE_1].value;
              used = 1;
              break;
            }
        }
    }
  //Extract TTB address from TTBRx =>  TTBR[ 31: (14-TTBCR.N) ]
  for (int i = MSB; i >= (LSB - ttbcr_n); i--)
    setBit (mask, i);

  *TTBAdd = (ttb & mask);
  return used;
}

// Bus connection functions
ac_tlm_rsp
MMU::talk_to_bus (const ac_tlm_req & req)
{
  return bus_port->transport (req);
}

ac_tlm_rsp
MMU::talk_to_bus (ac_tlm_req_type type, unsigned address, unsigned datum)
{
  ac_tlm_req req;
  req.type = type;
  req.addr = address;
  req.data = datum;
  return bus_port->transport (req);
}

// MMU Core Inteface Function. Receives a memory access request from
// Core, performs the translation based on information from CP15 and
// redispatch the request with the converted address to bus. If MMU
// translation feature is disable, it simply resends the message to the
// bus TLM port. Everything is invisible to target code, which only
// receives the memory word for the virtual address requested.
ac_tlm_rsp MMU::transport (const ac_tlm_req & req)
{
  uint32_t phy_address;

#ifdef WITH_TLB
  tlb_t* tlb_p;
#endif

  if (translation_active () == false)
    {
      dprintf ("|| MMU Operation: <> MMU is: OFF: "
               "bypassing: Physical Address matches Virtual Address\n");

      return talk_to_bus (req);
    }
  dprintf ("|| MMU Operation: <> MMU is: ON:\n");

#ifdef WITH_TLB

  tlb_p = (req.type == DATA_READ)? &tlb_d:&tlb_i;

  if (tlb_p->fetch_item (req.addr>>12, &phy_address) == false)
    {
      // Perform L1 translation.
      phy_address = L1::translate (*this, req.addr);
      tlb_p->insert_item (req.addr>>12, (phy_address & ~0xFFF));
    }
  phy_address |= (req.addr & 0xFFF);

#else // !WITH_TLB
  // Perform L1 translation.
  phy_address = L1::translate (*this, req.addr);
#endif // WITH_TLB

  // Redispatch with translated physical address.
  return talk_to_bus (req.type, phy_address, req.data);
}

// Performs a first level translate for a given virtual address. This
// method may generate a prefetch/data abort in the event of a table
// entry fault.
uint32_t
MMU::L1::translate (MMU & mmu, uint32_t va)
{
  const uint32_t MSB = 31;
  const uint32_t LSB = 20;

  uint32_t phy_address = 0x0;
  uint32_t ttb_address;
  uint32_t table_entry_index;
  uint32_t first_level_entry_address;
  L1::table_entry first_level_entry;
  uint32_t mask = 0;
  int ttbcr_n = 0;

  dprintf ("%s: translating address: 0x%X\n", mmu.name(), va);

  if (mmu.ttb_address (&ttb_address, va) == 0)
    {
      // Read TTBCR.N
      ttbcr_n = (mmu.cop.registers[cp15::TRANSLATION_TABLE_BASE_CONTROL].value
                 & 0b111);

      dprintf ("%s: TTBR0 used. Address=0x%x. N = TTBCR.N = %d\n",
               mmu.name(), ttb_address, ttbcr_n);
    }
  else
    {
      ttbcr_n = 0;
      dprintf ("%s: TTBR1 used. Address=0x%x. N = %d\n",
               mmu.name(), ttb_address, ttbcr_n);
    }

  //Discover first level table entry address (FLA)
  for (uint32_t i = (MSB - ttbcr_n); i >= LSB; i--)
    setBit (mask, i);

  table_entry_index = (va & mask) >> LSB;

  first_level_entry_address = (ttb_address
                               | (table_entry_index << 2));

  first_level_entry = L1::table_walk (mmu, first_level_entry_address);

  switch (first_level_entry.type)
    {
    case L1::PAGE:
      phy_address =
        L2::translate (mmu, first_level_entry.data.page.base_address, va);
      break;

    case L1::SECTION:
      phy_address = ((first_level_entry.data.section.base_address << 20)
                     | (va & 0xFFFFF));
      break;

    case L1::SUPERSECTION:
      fprintf (stderr,
               "%s: This model does not support SuperSection tables.\n",
               mmu.name());
      exit (0);
      break;

    case  L1::FAULT:
      //Generate Translate Fault
      fprintf (stderr,
               "%s: Need to generate translation fault.\n", mmu.name());
      //exit(0);
      break;

    case L1::RESERVED:
      fprintf (stderr, "%s: Access to Reserved type page.\n", mmu.name());
      exit (0);
      break;
    }

  dprintf ("%s: End of translation. vAdd=0x%X ==>> pAdd = 0x%X\n",
           mmu.name(), va, phy_address);

  return phy_address;
}

// Perform a first level page walk operation. We fetch the address from
// memory and load it into the correct data structure.
MMU::L1::table_entry
MMU::L1::table_walk (MMU & mmu, uint32_t fla)
{
  struct L1::table_entry entry;
  ac_tlm_rsp rsp = mmu.talk_to_bus (READ, fla, 0); //Read First level entry
  uint32_t data = rsp.data;


  dprintf ("%s: Performing First-level table walk: "
           "FLA = 0x%X, FLD=0x%X. FLD is type ", mmu.name(), fla, data);

  switch (data & 0b11) //Extract type bits[1:0]
    {
    case 0:
      //FAULT
      entry.type = L1::FAULT;
      dprintf ("[DATA FAULT]\n");
      break;
    case 1:
      //Page Table
      dprintf ("[PAGE TABLE]\n");
      entry.type = L1::PAGE;
      entry.data.page.base_address = (data & 0xFFFFFC00) >> 10;
      entry.data.page.SBZ = isBitSet (data, 4);
      entry.data.page.NS = isBitSet (data, 3);
      entry.data.page.Domain[0] = isBitSet (data, 5);
      entry.data.page.Domain[1] = isBitSet (data, 6);
      entry.data.page.Domain[2] = isBitSet (data, 7);
      entry.data.page.Domain[3] = isBitSet (data, 8);
      break;
    case 2:
    case 3:
      if (isBitSet (data, 18))
        {
          dprintf ("[SUPERSECTION]\n");
          //Supersection
          entry.type = L1::SUPERSECTION;
          entry.data.super.base_address = (data & 0xFF000000) >> 24;
          entry.data.super.ext_base_address = (data & 0x1E0) >> 5;
          entry.data.super.NS = isBitSet (data, 19);
          entry.data.super.nG = isBitSet (data, 17);
          entry.data.super.AP[0] = isBitSet (data, 10);
          entry.data.super.AP[1] = isBitSet (data, 11);
          entry.data.super.AP[2] = isBitSet (data, 15);
          entry.data.super.TEX[0] = isBitSet (data, 12);
          entry.data.super.TEX[1] = isBitSet (data, 13);
          entry.data.super.TEX[2] = isBitSet (data, 14);
          entry.data.super.XN = isBitSet (data, 4);
          entry.data.super.C = isBitSet (data, 3);
          entry.data.super.B = isBitSet (data, 2);
        }
      else
        {
          //Section
          dprintf ("[SECTION]\n");
          entry.type = L1::SECTION;
          entry.data.section.base_address = (data & 0xFFF00000) >> 20;
          entry.data.section.NS = isBitSet (data, 19);
          entry.data.section.nG = isBitSet (data, 17);
          entry.data.section.AP[0] = isBitSet (data, 10);
          entry.data.section.AP[1] = isBitSet (data, 11);
          entry.data.section.AP[2] = isBitSet (data, 15);
          entry.data.section.TEX[0] = isBitSet (data, 12);
          entry.data.section.TEX[1] = isBitSet (data, 13);
          entry.data.section.TEX[2] = isBitSet (data, 14);
          entry.data.section.domain[0] = isBitSet (data, 5);
          entry.data.section.domain[1] = isBitSet (data, 6);
          entry.data.section.domain[2] = isBitSet (data, 7);
          entry.data.section.domain[3] = isBitSet (data, 8);
          entry.data.section.XN = isBitSet (data, 4);
          entry.data.section.C = isBitSet (data, 3);
          entry.data.section.B = isBitSet (data, 2);
        }
      break;
    }
  return entry;
}

// Second Level translation methods.

// Perform a second level translation for Small/Large pages. Might
// generate prefetch/data abort if access a page fault.
uint32_t
MMU::L2::translate (MMU & mmu, uint32_t base_address, uint32_t va)
{
  const uint32_t mask = 0xFF000;
  L2::table_entry page_entry;
  uint32_t phy_address;
  uint32_t table_index;
  uint32_t page_entry_address;

  table_index = (va & mask) >> 12;
  page_entry_address = ((base_address << 8) | table_index) << 2;

  // Perform page walk
  page_entry = L2::table_walk (mmu, page_entry_address);

  // Recover physical address.
  switch (page_entry.type)
    {
    case L2::SMALL:
      phy_address = ((page_entry.data.small_page.base_address << 12)
                     | (va & 0xFFF));
      break;

    case L2::LARGE:
      phy_address = ((page_entry.data.large_page.base_address << 16)
                     | (va & 0xFFFF));
      break;

    default:
      printf ("%s: Second level page fault was not implemented in this model",
              mmu.name());
      exit(0);
    }

  return phy_address;
}

// Perform a second level page walk operation. We fetch the address
// from memory and load it to the correct data structure.
MMU::L2::table_entry
MMU::L2::table_walk (MMU & mmu, uint32_t SLA)
{
  L2::table_entry entry;
  ac_tlm_rsp rsp = mmu.talk_to_bus (READ, SLA, 0);
  uint32_t data = rsp.data;

  dprintf ("%s: Performing second-level table walk:"
           "SLA = 0x%X, FLD=0x%X. page is type ", mmu.name(), SLA, data);

  switch (data & 0b11)
    {
    case 0:
      entry.type = L2::FAULT;
      dprintf ("[DATA FAULT]\n");
      break;
    case 1:
      dprintf ("[LARGE PAGE]\n");
      entry.type = L2::LARGE;
      entry.data.large_page.base_address = ((data & 0xFFFF0000) >> 16);
      entry.data.large_page.xn = isBitSet (data, 15);
      entry.data.large_page.tex[2] = isBitSet (data, 14);
      entry.data.large_page.tex[1] = isBitSet (data, 13);
      entry.data.large_page.tex[0] = isBitSet (data, 12);
      entry.data.large_page.nG = isBitSet (data, 11);
      entry.data.large_page.s = isBitSet (data, 10);
      entry.data.large_page.ap[2] = isBitSet (data, 9);
      entry.data.large_page.sbz[2] = isBitSet (data, 8);
      entry.data.large_page.sbz[1] = isBitSet (data, 7);
      entry.data.large_page.sbz[0] = isBitSet (data, 6);
      entry.data.large_page.ap[1] = isBitSet (data, 5);
      entry.data.large_page.ap[0] = isBitSet (data, 4);
      entry.data.large_page.c = isBitSet (data, 3);
      entry.data.large_page.b = isBitSet (data, 2);
      break;
    case 2:
    case 3:
      dprintf ("[SMALL PAGE]\n");
      entry.type = L2::SMALL;
      entry.data.small_page.base_address = ((data & 0xFFFFF000) >> 12);
      entry.data.small_page.nG = isBitSet (data, 11);
      entry.data.small_page.s = isBitSet (data, 10);
      entry.data.small_page.ap[2] = isBitSet (data, 9);
      entry.data.small_page.tex[2] = isBitSet (data, 8);
      entry.data.small_page.tex[1] = isBitSet (data, 7);
      entry.data.small_page.tex[1] = isBitSet (data, 6);
      entry.data.small_page.ap[1] = isBitSet (data, 5);
      entry.data.small_page.ap[0] = isBitSet (data, 4);
      entry.data.small_page.c = isBitSet (data, 3);
      entry.data.small_page.b = isBitSet (data, 2);
      entry.data.small_page.xn = isBitSet (data, 0);
      break;
    }
  return entry;
}

