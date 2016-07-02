// "tzic.cpp" - TZIC model
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
// Author : Rafael Auler
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#include "tzic.h"
#include "arm_interrupts.h"
#include <time.h>

extern bool DEBUG_TZIC;

#include <stdarg.h>
static inline int
dprintf (const char *format, ...)
{
  int ret;
  if (DEBUG_TZIC)
    {
      va_list args;
      va_start (args, format);
      ret = vfprintf (ac_err, format, args);
      va_end (args);
    }
  return ret;
}

// When an interrupt occurs, checks if priority is higher than TZIC_PRIOMASK
// if not, does not raise the interrupt.
// Check TZIC_ENSETn to see if interrupt is enabled, before registering it.
// Check TZIC_SRCSETn for active interrupts (and with ENSET)
void
tzic_module::prc_tzic ()
{
  do
    {
      wait (1, SC_NS);
      if (!changed)
	continue;
      changed = false;
      wakeup_signal = false;
      bool has_int = false;

      dprintf ("-------------------- TZIC -------------------- \n");
      // updates PENDING based on ENSET & (SRCSET | int_in) and PRIORITY
      // SRCSET stores software caused interrupts
      // int_in stores hardware caused interrupts
      unsigned candidate[4];
      candidate[0] = *(regs + TZIC_ENSET0 / 4) & (*(regs + TZIC_SRCSET0 / 4)
						  | int_in[0]);
      candidate[1] = *(regs + TZIC_ENSET1 / 4) & (*(regs + TZIC_SRCSET1 / 4)
						  | int_in[1]);
      candidate[2] = *(regs + TZIC_ENSET2 / 4) & (*(regs + TZIC_SRCSET2 / 4)
						  | int_in[2]);
      candidate[3] = *(regs + TZIC_ENSET3 / 4) & (*(regs + TZIC_SRCSET3 / 4)
						  | int_in[3]);
      unsigned highest_pri = 0;
      for (unsigned i = 0; i < 128; ++i)
	{
	  bool cur = (*(candidate + (i / 32)) >> (i % 32)) ? true : false;
	  if (!cur)
	    {
	      *(regs + TZIC_PND0 / 4 + (i / 32)) &= ~(1 << (i % 32));
	      continue;
	    }
	  unsigned pri = *(regs + TZIC_PRIORITY0 / 4 + (i / 4));
	  switch (i % 4)
	    {
	    case 0:
	      pri = pri & 0xFF;
	      break;
	    case 1:
	      pri = (pri & 0xFF00) >> 8;
	      break;
	    case 2:
	      pri = (pri & 0xFF0000) >> 16;
	      break;
	    case 3:
	      pri = (pri & 0xFF000000) >> 24;
	      break;
	    }
	  if (pri > *(regs + TZIC_PRIOMASK / 4))
	    {
	      has_int = true;
	      dprintf ("Interrupt IRQ #%d (priority %d) is pending.\n", i,
		       pri);
	      *(regs + TZIC_PND0 / 4 + (i / 32)) |= (1 << (i % 32));
	      if (pri > highest_pri)
		highest_pri = pri;
	      // is this a wakeup interrupt?
	      if (*(regs + TZIC_WAKEUP0 / 4 + (i / 32)) & (1 << (i % 32)))
		{
		  // deassert DSMINT
		  *(regs + TZIC_DSMINT / 4) = 0;
		  wakeup_signal = true;
		  changed = true;
		}
	    }
	}
      // updates HIPENDING based on PENDING and PRIORITY
      for (unsigned i = 0; i < 128; ++i)
	{
	  bool cur = (*(candidate + (i / 32)) >> (i % 32)) ? true : false;
	  if (!cur)
	    {
	      *(regs + TZIC_HIPND0 / 4 + (i / 32)) &= ~(1 << (i % 32));
	      continue;
	    }
	  unsigned pri = *(regs + TZIC_PRIORITY0 / 4 + (i / 4));
	  switch (i % 4)
	    {
	    case 0:
	      pri = pri & 0xFF;
	      break;
	    case 1:
	      pri = (pri & 0xFF00) >> 8;
	      break;
	    case 2:
	      pri = (pri & 0xFF0000) >> 16;
	      break;
	    case 3:
	      pri = (pri & 0xFF000000) >> 24;
	      break;
	    }
	  if (pri == highest_pri)
	    {
	      *(regs + TZIC_HIPND0 / 4 + (i / 32)) |= (1 << (i % 32));
	    }
	  else
	    {
	      *(regs + TZIC_HIPND0 / 4 + (i / 32)) &= ~(1 << (i % 32));
	    }
	}

      if (has_int && enabled)
	{
	  ac_tlm_rsp rsp;
	  ac_tlm_req req;
	  dprintf ("Triggered EXCEPTION_IRQ in ARM core.\n");
	  req.type = WRITE;
	  req.dev_id = 0;
	  req.addr = 0;
	  req.data = arm_impl::EXCEPTION_IRQ;
	  rsp = proc_port->transport (req);
	}
    }
  while (1);
}

void
tzic_module::interrupt (unsigned intnumber, bool deassert)
{
  // Looks for TZIC_DSMINT. If 1, should not change interrupts (hold off).
  if (*(regs + TZIC_DSMINT / 4) & 0x1)
    {
      return;
    }
  if (deassert)
    int_in[intnumber / 32] &= ~(1 << (intnumber % 32));
  else
    int_in[intnumber / 32] |= (1 << (intnumber % 32));
  changed = true;
}

unsigned
tzic_module::fast_read (unsigned address)
{
  unsigned res = 0;
  switch (address)
    {
    case TZIC_INTCTRL:
      if (enabled)
	res = 1;
      else
	res = 0;
      break;
    case TZIC_ENCLEAR0:
    case TZIC_ENCLEAR1:
    case TZIC_ENCLEAR2:
    case TZIC_ENCLEAR3:
      address -= TZIC_ENCLEAR0 - TZIC_ENSET0;
    case TZIC_ENSET0:
    case TZIC_ENSET1:
    case TZIC_ENSET2:
    case TZIC_ENSET3:
      return *(regs + address / 4);
    case TZIC_SRCCLEAR0:
    case TZIC_SRCCLEAR1:
    case TZIC_SRCCLEAR2:
    case TZIC_SRCCLEAR3:
      address -= TZIC_SRCCLEAR0 - TZIC_SRCSET0;
    case TZIC_SRCSET0:
    case TZIC_SRCSET1:
    case TZIC_SRCSET2:
    case TZIC_SRCSET3:
      return *(regs + address / 4) | int_in[(address - TZIC_SRCSET0) / 4];
    default:
      return *(regs + address / 4);

    }
  return res;
}

void
tzic_module::fast_write (unsigned address, unsigned datum)
{
  changed = true;
  switch (address)
    {
    case TZIC_INTCTRL:
      if (datum & 1)
	enabled = true;
      else
	enabled = false;
      break;
    case TZIC_INTTYPE:
      break;
    case TZIC_DSMINT:
      // If an interrupt is occurring, does not change DSMINT state.
      if (int_in[0] | int_in[1] | int_in[2] | int_in[3]
	  | *(regs + TZIC_SRCSET0 / 4) | *(regs + TZIC_SRCSET1 / 4)
	  | *(regs + TZIC_SRCSET2 / 4) | *(regs + TZIC_SRCSET3 / 4))
	return;
      *(regs + address / 4) = datum & 0x1;
      break;
    case TZIC_PND0:
    case TZIC_PND1:
    case TZIC_PND2:
    case TZIC_PND3:
    case TZIC_HIPND0:
    case TZIC_HIPND1:
    case TZIC_HIPND2:
    case TZIC_HIPND3:
      break;
    case TZIC_ENCLEAR0:
    case TZIC_ENCLEAR1:
    case TZIC_ENCLEAR2:
    case TZIC_ENCLEAR3:
      address -= TZIC_ENCLEAR0 - TZIC_ENSET0;
      *(regs + address / 4) = *(regs + address / 4) | ~datum;
      break;
    case TZIC_ENSET0:
    case TZIC_ENSET1:
    case TZIC_ENSET2:
    case TZIC_ENSET3:
      *(regs + address / 4) = *(regs + address / 4) | datum;
      break;
    case TZIC_SRCCLEAR0:
    case TZIC_SRCCLEAR1:
    case TZIC_SRCCLEAR2:
    case TZIC_SRCCLEAR3:
      address -= TZIC_SRCCLEAR0 - TZIC_SRCSET0;
      *(regs + address / 4) = *(regs + address / 4) | ~datum;
      break;
    case TZIC_SRCSET0:
    case TZIC_SRCSET1:
    case TZIC_SRCSET2:
    case TZIC_SRCSET3:
      *(regs + address / 4) = *(regs + address / 4) | datum;
      break;
    case TZIC_SWINT:
      {
	bool negate = (datum & 0x80000000) >> 31;
	unsigned intnum = datum & 0x3FF;
	if (negate)
	  *(regs + TZIC_SRCSET0 / 4 + (intnum / 32)) &= ~(1 << (intnum % 32));
	else
	  *(regs + TZIC_SRCSET0 / 4 + (intnum / 32)) |= 1 << (intnum % 32);
      }
      break;
    default:
      *(regs + address / 4) = datum;
    }
}
