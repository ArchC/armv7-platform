// "gpt.cpp" - General Purpose Timer model
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

#include "gpt.h"
#include "arm_interrupts.h"
#include <time.h>

extern bool DEBUG_GPT;

#include <stdarg.h>
static inline int
dprintf (const char *format, ...)
{
  int ret;
  if (DEBUG_GPT)
    {
      va_list args;
      va_start (args, format);
      ret = vfprintf (ac_err, format, args);
      va_end (args);
    }
  return ret;
}

void
gpt_module::set_output_pin (bool & output, oc_operation_mode_t mode)
{
  switch (mode)
    {
    case OC_TOGGLE:
      output = !output;
      break;
    case OC_CLEAR:
      output = false;
      break;
    case OC_SET:
      output = true;
      break;
      // FIXME: low pulse is not functional in this model
    case OC_LOWPULSE:
      output = false;
      break;
    default:
      break;
    }
}

void
gpt_module::prc_gpt ()
{
  do
    {
      wait (1, SC_NS);

      if (!enabled)
	continue;

      if (clock_src == CLK_OFF || clock_src == EXTERNAL_CLK)
	continue;

      dprintf ("-------------------- GPT --------------------- \n");
      dprintf ("Prescaler counter value: 0x%X\n", prescaler_counter);
      bool prescaler_tick = false;
      if (prescaler_counter == prescaler)
	{
	  prescaler_tick = true;
	  prescaler_counter = 0;
	  dprintf ("Prescaler ticked.\n");
	}
      else if (prescaler_counter >= 4096)
	{
	  prescaler_counter = 0;
	}
      else
	{
	  ++prescaler_counter;
	}

      if (!prescaler_tick)
	continue;

      dprintf ("Counter value: 0x%X\n", counter);
      // Compare counter logic
      if (counter == 0xFFFFFFFF)
	{
	  *(regs + GPT_SR / 4) |= (1 << 5);
	  dprintf ("Counter rolled over.\n");
	}
      if (counter == *(regs + GPT_OCR1 / 4))
	{
	  *(regs + GPT_SR / 4) |= (1 << 0);
	  set_output_pin (do_cmpout1, om1);
	  dprintf ("Counter output_compare_1 event generated.\n");
	}
      else if (counter == *(regs + GPT_OCR2 / 4))
	{
	  *(regs + GPT_SR / 4) |= (1 << 1);
	  set_output_pin (do_cmpout2, om2);
	  dprintf ("Counter output_compare_2 event generated.\n");
	}
      else if (counter == *(regs + GPT_OCR3 / 4))
	{
	  *(regs + GPT_SR / 4) |= (1 << 2);
	  set_output_pin (do_cmpout3, om3);
	  dprintf ("Counter output_compare_3 event generated.\n");
	}
      // Update counter logic
      if (counter_mode == M_RESTART && counter == *(regs + GPT_OCR1 / 4))
	{
	  counter = 0;
	}
      else
	{
	  if (counter == 0xFFFFFFFF)
	    counter = 0;
	  else
	    ++counter;
	}
      // Generate interrupts
      bool gen_interrupt = false;
      if (*(regs + GPT_IR / 4) & *(regs + GPT_SR / 4))
	gen_interrupt = true;
      if (gen_interrupt)
	{
	  tzic.interrupt (GPT_IRQNUM, /*deassert= */ false);
	  dprintf ("Asserted interrupt line in TZIC.\n");
	}
      else
	{
	  tzic.interrupt (GPT_IRQNUM, /*deassert= */ true);
	}
    }
  while (1);
}

unsigned
gpt_module::fast_read (unsigned address)
{
  unsigned res = 0;
  switch (address)
    {
    case GPT_CNT:
      return counter;
      break;
    default:
      return *(regs + address / 4);
    }
  return res;
}

void
gpt_module::fast_write (unsigned address, unsigned datum)
{
  switch (address)
    {
    case GPT_CR:
      // Not written to the control register
      if (datum & (1 << 31))
	{			// force output compare channel 3
	  set_output_pin (do_cmpout3, om3);
	}
      if (datum & (1 << 30))
	{			// force output compare channel 2
	  set_output_pin (do_cmpout2, om2);
	}
      if (datum & (1 << 29))
	{			// force output compare channel 1
	  set_output_pin (do_cmpout1, om1);
	}
      // Written to the control register
      om3 = static_cast < oc_operation_mode_t > ((datum & 0x1C000000) >> 26);
      om2 = static_cast < oc_operation_mode_t > ((datum & 0x03800000) >> 23);
      om1 = static_cast < oc_operation_mode_t > ((datum & 0x00700000) >> 20);
      im2 = static_cast < input_capture_t > ((datum & 0x000C0000) >> 18);
      im1 = static_cast < input_capture_t > ((datum & 0x00030000) >> 16);
      counter_mode =
	static_cast < counter_mode_t > ((datum & 0x00000200) >> 9);
      clock_src =
	static_cast < selected_clock_t > ((datum & 0x000001C0) >> 6);
      stop_en = (datum & 0x00000020) >> 5;
      wait_en = (datum & 0x00000008) >> 3;
      dbg_en = (datum & 0x00000004) >> 2;
      en_mode = (datum & 0x00000002) >> 1;
      // are we re-enabling module?
      if (!enabled & (datum & 0x1))
	{
	  if (en_mode)
	    {
	      counter = prescaler_counter = 0;
	    }
	}
      enabled = (datum & 0x1);

      // Ignore writes to FO3, FO2, FO1, SWR (software reset)
      *(regs + address / 4) = datum & ~(0xE0008000);

      if (datum & 0x8000)
	do_reset ( /*hard_reset= */ false);
      break;
    case GPT_SR:
      // Clear bits indicated
      *(regs + address / 4) &= ~datum;
      break;
    case GPT_ICR1:
    case GPT_ICR2:
    case GPT_CNT:
      // FIXME: Should generate bus exception, these are read-only
      // registers
      break;
    case GPT_OCR1:
      if (counter_mode == M_RESTART)
	counter = 0;
      *(regs + address / 4) = datum;
      break;
    case GPT_PR:
      prescaler = datum;
      prescaler_counter = 0;
      // fall through
    default:
      *(regs + address / 4) = datum;
    }

}


void
gpt_module::ind_capin1 (bool deassert)
{
  if (((im1 == IC_RISINGEDGE || im1 == IC_BOTH)
       && !deassert)
      || ((im1 == IC_FALLINGEDGE || im1 == IC_BOTH) && deassert))
    {
      *(regs + GPT_ICR1 / 4) = counter;
      *(regs + GPT_SR / 4) |= (1 << 3);
    }
}

void
gpt_module::ind_capin2 (bool deassert)
{
  if (((im2 == IC_RISINGEDGE || im2 == IC_BOTH)
       && !deassert)
      || ((im2 == IC_FALLINGEDGE || im2 == IC_BOTH) && deassert))
    {
      *(regs + GPT_ICR2 / 4) = counter;
      *(regs + GPT_SR / 4) |= (1 << 4);
    }
}
