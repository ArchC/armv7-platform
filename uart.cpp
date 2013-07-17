// "uart.cpp" - UART model
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

#include "uart.h"
#include "arm_interrupts.h"
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

extern bool DEBUG_UART;

#include <stdarg.h>
static inline int
dprintf (const char *format, ...)
{
  int ret;
  if (DEBUG_UART)
    {
      va_list args;
      va_start (args, format);
      ret = vfprintf (ac_err, format, args);
      va_end (args);
    }
  return ret;
}

static struct termios orig_termios;
static void
reset_terminal_mode ()
{
  tcsetattr (0, TCSANOW, &orig_termios);
}

static void
set_raw_terminal_mode ()
{
  struct termios new_termios;

  /* take two copies - one for now, one for later */
  tcgetattr (0, &orig_termios);
  memcpy (&new_termios, &orig_termios, sizeof (new_termios));

  /* set the new terminal mode */
  // FIXME: this is scrambling terminal output!
  atexit (reset_terminal_mode);
  //cfmakeraw(&new_termios);
  //tcsetattr(0, TCSANOW, &new_termios);
}

uart_module::uart_module (sc_module_name name_, tzic_module & tzic_):
sc_module (name_), tzic (tzic_)
{
  // A SystemC thread never finishes execution, but transfers control back
  // to SystemC kernel via wait() calls.
  SC_THREAD (prc_uart);

  do_reset ();

  set_raw_terminal_mode ();
}

uart_module::~uart_module ()
{
  reset_terminal_mode ();
}

static bool
check_if_has_input ()
{
  struct timeval tv = { 0L, 0L };
  fd_set rdset;
  FD_ZERO (&rdset);
  FD_SET (0, &rdset);
  if (select (1, &rdset, NULL, NULL, &tv) == 1)
    return true;
  return false;
}

void
uart_module::update_flags ()
{
  if (rxd_pointer > 0)
    {
      rdr = true;		// receiver data ready (at least one character in fifo)
      regs[UART_UTS / 4] &= ~(1 << 5); // Erase RxFIFO from UART_UTS
    }
  else
    {
      rdr = false;
      regs[UART_UTS / 4] |= (1 << 5);
    }
  if (rxd_pointer > rxtl)
    {
      rrdy = true;
    }
  else
    {
      rrdy = false;
    }
  if (txd_pointer == 0)
    {
      txfe = true;
      txdc = true;
    }
  else
    {
      txfe = false;
      txdc = false;
    }
  if (txd_pointer < txtl)
    {
      trdy = true;
    }
  else
    {
      trdy = false;
    }
}

void
uart_module::prc_uart ()
{
  do
    {
      wait (1, SC_NS);

      if (!uart_enabled)
	continue;

      dprintf ("-------------------- UART -------------------- \n");

      // Receiver logic
      if (rxd_enabled)
	{
	  unsigned char receive_count = 0;
	  while (check_if_has_input () && rxd_pointer < 32)
	    {
	      char c;
	      if (read (STDIN_FILENO, &c, 1) <= 0)
		continue;
	      if (cin.good ())
		{
		  if (ws == WS_7BIT)
		    c &= 0x7F;
		  rxd_fifo[rxd_pointer++] = c;
		}
	      else
		break;
	      ++receive_count;
	    }
	  if (receive_count)
	    dprintf ("Received %d characters.\n", receive_count);
	}

      // Transmitter logic
      if (txd_enabled)
	{
	  unsigned char transmit_count = 0;
	  while (txd_pointer > 0)
	    {
	      char c = txd_fifo[0];
	      for (int i = 1; i < txd_pointer; ++i)
		txd_fifo[i - 1] = txd_fifo[i];
	      --txd_pointer;
	      if (ws == WS_7BIT)
		c &= 0x7F;
	      std::cout.put (c);
	      std::cout.flush ();
	      ++transmit_count;
	    }
	  if (transmit_count)
	    dprintf ("Trasnsmitted %d characters.\n", transmit_count);
	}

      update_flags ();

      // Interrupt generation logic
      if ((rrdyen && rrdy)
	  || (txmptyen && txfe)
	  || (trdyen && trdy) || (tcen && txdc) || (dren && rdr))
	{
	  tzic.interrupt (UART_IRQNUM, /*deassert= */ false);
	  dprintf ("Asserted interrupt line in TZIC.\n");
	}
      else
	{
	  tzic.interrupt (UART_IRQNUM, /*deassert= */ true);
	}
      // TODO: DMA generation logic

    }
  while (1);
}

unsigned
uart_module::fast_read (unsigned address)
{
  unsigned res = 0;
  switch (address)
    {
      // Reading character
    case UART_URXD:
      {
	bool charrdy = true;	// Indicates false when fifo underrun (no data to read)
	bool err = false;	// Indicates whether the read character has any error
	bool ovrrun = false;	// Indicates this is the 32nd in the fifo (last pos)
	bool frmerr = false;	// Frame error (missing stop bit)
	bool brk = false;	// BREAK detected
	bool prerr = false;	// Parity bit error
	unsigned data = 0;
	if (!uart_enabled || !rxd_enabled)
	  // Should return error
	  return 0;
	// Underrun?
	if (rxd_pointer == 0)
	  {
	    charrdy = false;
	    return ((data & 0xFF) | (charrdy << 15) | (err << 14) |
		    (ovrrun << 13) | (frmerr << 12) | (brk << 11) | (prerr <<
								     10));
	  }
	data = rxd_fifo[0];
	for (int i = 1; i < rxd_pointer; ++i)
	  rxd_fifo[i - 1] = rxd_fifo[i];
	--rxd_pointer;
	if (rxd_pointer == 32)
	  ovrrun = true;
	update_flags ();
	return ((data & 0xFF) | (charrdy << 15) | (err << 14) | (ovrrun << 13)
		| (frmerr << 12) | (brk << 11) | (prerr << 10));
      }
      break;
    case UART_USR1:
      return (parityerr << 15)
	| (rtss << 14)
	| (trdy << 13)
	| (rtsd << 12)
	| (escf << 11)
	| (framerr << 10)
	| (rrdy << 9)
	| (agtim << 8)
	| (dtrd << 7) | (rxds << 6) | (airint << 5) | (awake << 4);
      break;
    case UART_USR2:
      return (adet << 15) |
	(txfe << 14) |
	(dtrf << 13) |
	(idle << 12) |
	(acst << 11) |
	(ridelt << 10) |
	(riin << 9) |
	(irint << 8) |
	(wake << 7) |
	(dcddelt << 6) |
	(dcdin << 5) |
	(rtsf << 4) | (txdc << 3) | (brcd << 2) | (ore << 1) | rdr;
      break;
    default:
      return *(regs + address / 4);
    }
  return res;
}

void
uart_module::fast_write (unsigned address, unsigned datum)
{
  switch (address)
    {
    case UART_UTXD:
      {
	if (!uart_enabled || !txd_enabled)
	  // Should return error
	  return;
	if (!trdy)
	  // Should not send data when trdy is not high since it can send corrupted
	  // data
	  //return;
	  ;
	if (txd_pointer == 33)
	  // Overrun
	  return;
	if (ws == WS_7BIT)
	  txd_fifo[txd_pointer++] = datum & 0x7F;
	else
	  txd_fifo[txd_pointer++] = datum & 0xFF;
	update_flags ();
      }
      break;
    case UART_UCR1:
      aden = (datum >> 15) & 1;
      adbr = (datum >> 14) & 1;
      trdyen = (datum >> 13) & 1;
      iden = (datum >> 12) & 1;
      icd = static_cast < idle_condition_t > ((datum >> 10) & 3);
      rrdyen = (datum >> 9) & 1;
      rxdmaen = (datum >> 8) & 1;
      iren = (datum >> 7) & 1;
      txmptyen = (datum >> 6) & 1;
      rtsden = (datum >> 5) & 1;
      txdmaen = (datum >> 3) & 1;
      atdmaen = (datum >> 2) & 1;
      uart_enabled = datum & 1;
      *(regs + address / 4) = datum & 0xFFEF;
      if ((datum >> 4) & 1)
	;			// TODO
      break;
    case UART_UCR2:
      if (datum & 1)
	do_reset ( /* hardreset = */ false);
      esci = (datum >> 15) & 1;
      irts = (datum >> 14) & 1;
      ctsc = (datum >> 13) & 1;
      cts = (datum >> 12) & 1;
      escen = (datum >> 11) & 1;
      rtec = static_cast < rts_edge_t > ((datum >> 9) & 3);
      pren = (datum >> 8) & 1;
      proe = (datum >> 7) & 1;
      stpb = (datum >> 6) & 1;
      ws = static_cast < wordsize_t > ((datum >> 5) & 1);
      rtsen = (datum >> 4) & 1;
      aten = (datum >> 3) & 1;
      txd_enabled = (datum >> 2) & 1;
      rxd_enabled = (datum >> 1) & 1;
      *(regs + address / 4) = (datum & 0xFFFE) | 0x1;	//Reset was done but this would overwrite
      //reset status bit
      break;
    case UART_UCR3:
      dpec = static_cast < dtr_edge_t > ((datum >> 14) & 3);
      dtren = (datum >> 13) & 1;
      parerren = (datum >> 12) & 1;
      fraerren = (datum >> 11) & 1;
      dsr = (datum >> 10) & 1;
      dcd = (datum >> 9) & 1;
      ri = (datum >> 8) & 1;
      adnimp = (datum >> 7) & 1;
      rxdsen = (datum >> 6) & 1;
      airinten = (datum >> 5) & 1;
      awaken = (datum >> 4) & 1;
      dtrden = (datum >> 3) & 1;
      rxdmuxsel = (datum >> 2) & 1;
      invt = (datum >> 1) & 1;
      acien = datum & 1;
      *(regs + address / 4) = datum & 0xFFFF;
      break;
    case UART_UCR4:
      ctstl = (datum >> 10) & 0x1F;
      invr = (datum >> 9) & 1;
      eniri = (datum >> 8) & 1;
      wken = (datum >> 7) & 1;
      iddmaen = (datum >> 6) & 1;
      irsc = (datum >> 5) & 1;
      lpbyp = (datum >> 4) & 1;
      tcen = (datum >> 3) & 1;
      bken = (datum >> 2) & 1;
      oren = (datum >> 1) & 1;
      dren = datum & 1;
      *(regs + address / 4) = datum & 0xFFFF;
      break;
    case UART_UFCR:
      txtl = (datum >> 10) & 0x1F;
      rfdiv = static_cast < ref_freq_t > ((datum >> 7) & 7);
      dcedte = (datum >> 7) & 1;
      rxtl = datum & 0x1F;
      *(regs + address / 4) = datum & 0xFFFF;
      break;
    case UART_USR1:
      parityerr &= ~((datum >> 15) & 0x1);
      // rtss writes are ignored
      // trdy writes are ignored
      rtsd &= ~((datum >> 12) & 0x1);
      escf &= ~((datum >> 11) & 0x1);
      framerr &= ~((datum >> 10) & 0x1);
      // rrdy writes are ignored
      agtim &= ~((datum >> 8) & 0x1);
      dtrd &= ~((datum >> 7) & 0x1);
      // rxds writes are ignored
      airint &= ~((datum >> 5) & 0x1);
      awake &= ~((datum >> 4) & 0x1);
      break;
    case UART_USR2:
      adet &= ((datum >> 15) & 0x1);
      // txfe writes are ignored
      dtrf &= ((datum >> 13) & 0x1);
      idle &= ((datum >> 12) & 0x1);
      acst &= ((datum >> 11) & 0x1);
      ridelt &= ((datum >> 10) & 0x1);
      // riin writes are ignored
      irint &= ((datum >> 8) & 0x1);
      wake &= ((datum >> 7) & 0x1);
      dcddelt &= ((datum >> 6) & 0x1);
      // dcdin writes are ignored
      rtsf &= ((datum >> 4) & 0x1);
      // txdc writes are ignored
      brcd &= ((datum >> 2) & 0x1);
      ore &= ((datum >> 1) & 0x1);
      // rdr writes are ignored
      break;
    case UART_UBRC:
      break;			// UBRC is readonly
    default:
      *(regs + address / 4) = datum;
    }
}
