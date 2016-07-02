// 'uart.h' - UART model
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
// Author : Rafael Auler, 15/09/2011
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef UART_H
#define UART_H

#include "peripheral.h"
#include "tzic.h"
#include <systemc.h>
#include <ac_tlm_protocol.H>

// In this model, we mimic the behavior of the UART IP for providing
// communication with the user and the arm platform+sw. This will be used
// as input/output for software running on iMX35. For this reason, all
// transmitted data from the UART will be directly printed on standard output
// (file descriptor 1) of the simulator, and all data received on standard
// input (file descriptor 0) of the simulator will be received by this UART
// IP simulation.
//
// More info about this module:
// Please refer to iMX53 Reference Manual page 4403
//
class uart_module:public sc_module, public peripheral
{
private:

  static const unsigned UART_URXD = 0x0;	// UART receiver register    (read)
  static const unsigned UART_UTXD = 0x40;	// UART transmitter register (write)
  static const unsigned UART_UCR1 = 0x80;	// UART control register 1
  static const unsigned UART_UCR2 = 0x84;	// UART control register 2
  static const unsigned UART_UCR3 = 0x88;	// UART control register 3
  static const unsigned UART_UCR4 = 0x8C;	// UART control register 4
  static const unsigned UART_UFCR = 0x90;	// UART FIFO control register
  static const unsigned UART_USR1 = 0x94;	// UART status register 1
  static const unsigned UART_USR2 = 0x98;	// UART status register 2
  static const unsigned UART_UESC = 0x9C;	// UART escape character register
  static const unsigned UART_UTIM = 0xA0;	// UART escape timer register
  static const unsigned UART_UBIR = 0xA4;	// UART BRM incremental register
  static const unsigned UART_UBRM = 0xA8;	// UART BRM modulator register
  static const unsigned UART_UBRC = 0xAC;	// UART baud rate counter register
  static const unsigned UART_ONEMS = 0xB0;	// UART one millisecond register
  static const unsigned UART_UTS = 0xB4;	// UART test register
  static const unsigned UART_LASTADDR = 0xB8;

  // According to iMX53 SoC
  static const unsigned UART_IRQNUM = 31;

  unsigned regs[UART_LASTADDR / 4];
  unsigned char rxd_fifo[32];
  unsigned char txd_fifo[32];
  unsigned rxd_pointer;
  unsigned txd_pointer;

  bool uart_enabled;
  bool rxd_enabled;
  bool txd_enabled;

  enum idle_condition_t
  {
    IC_4FRAMES = 0, IC_8FRAMES = 1, IC_16FRAMES = 2, IC_32FRAMES = 3
  };
  enum rts_edge_t
  {
    R_RISING = 0, R_FALLING = 1, R_BOTH = 2
  };
  enum wordsize_t
  { WS_7BIT = 0, WS_8BIT = 1 };
  enum dtr_edge_t
  { D_RISING = 0, D_FALLING_1, D_BOTH = 2 };
  enum ref_freq_t
  { RF_DIVBY6 = 0, RF_DIVBY5 = 1, RF_DIVBY4 = 2, RF_DIVBY3 = 3, RF_DIVBY2 = 4,
    RF_DIVBY1 = 5, RF_DIVBY7 = 6
  };

  // -- Parameters controlled by changing UCR1 --
  bool aden;			// automatic baud rate detection interrupt enable
  bool adbr;			// automatic baud rate detection
  bool trdyen;			// transmitter ready interrupt enable
  bool iden;			// idle connection detected interrupt enable
  idle_condition_t icd;		// idle connection detect
  bool rrdyen;			// receiver ready interrupt enable
  bool rxdmaen;			// receiver ready DMA enable
  bool iren;			// infrared enable
  bool txmptyen;		// transmitter empty interrupt enable
  bool rtsden;			// RTS delta interrupt enable
  bool txdmaen;			// trasmitter ready DMA enable
  bool atdmaen;			// aging dma timer enable
  // -- Parameters controlled by changing UCR2 --
  bool esci;			// escape sequence interrupt enable
  bool irts;			// ignore rts bin
  bool ctsc;			// cts pin control
  bool cts;			// clear to send
  bool escen;			// escape enable
  rts_edge_t rtec;		// request to send edge control
  bool pren;			// parity enable
  bool proe;			// parity odd/even 0 =even
  bool stpb;			// number of stop bits. 0=1, 1=2 ou more.
  wordsize_t ws;		// word size, 7 or 8 bits
  bool rtsen;			// request to send interrupt enable
  bool aten;			// aging timer enable
  // -- Parameters controlled by changing UCR3 --
  dtr_edge_t dpec;		// DTR/DSR interrupt edge control
  bool dtren;			// data terminal ready interrupt enable
  bool parerren;		// parity error interrupt enable
  bool fraerren;		// frame error interrupt enable
  bool dsr;			// data set ready
  bool dcd;			// data carrier detect
  bool ri;			// ring indicator
  bool adnimp;			// autobaud detection not improved
  bool rxdsen;			// receive status interrupt enable
  bool airinten;		// asynchronous IR wake interrupt enable
  bool awaken;			// asynchronous wake interrupt enable
  bool dtrden;			// data terminal ready delta enable
  bool rxdmuxsel;		// RXD muxed input selected - note this bit should be set in iMX35 SoC
  bool invt;			// infrared mode type os transmission
  bool acien;			// autobaud counter interrupt enable
  // -- Parameters controlled by changing UCR4 --
  unsigned char ctstl;		// CTS trigger level: 0, 31 or 32
  bool invr;			// IrDA mode active low or active high detection
  bool eniri;			// serial infrared interrupt enable
  bool wken;			// wake interrupt enable
  bool iddmaen;			// DMA idle condition detected interrupt enable
  bool irsc;			// IR special case
  bool lpbyp;			// Lower power bypass
  bool tcen;			// transmit complete interrupt enable
  bool bken;			// BREAK condition detected interrupt enable
  bool oren;			// receiver overrun interrupt enable
  bool dren;			// receive data ready interrupt enable
  // -- Parameters controlled by changing UFCR -- 
  unsigned char txtl;		// Transmitter trigger level: 2, 31 or 32
  ref_freq_t rfdiv;		// Reference frequency divider 
  bool dcedte;			// DCE/DTE mode select (0 = dce)
  unsigned char rxtl;		// Receiver trigger level: 1, 31 or 32
  // -- Status register 1 flags  -> active interrupts --
  bool parityerr;		// parity error interrupt flag
  bool rtss;			// RTSn pin status
  bool trdy;			// transmitter ready interrupt/DMA flag
  bool rtsd;			// RTS delta
  bool escf;			// escape sequence interrupt flag
  bool framerr;			// frame error interrupt flag
  bool rrdy;			// receiver ready interrupt/DMA flag
  bool agtim;			// ageing timer interrupt flag
  bool dtrd;			// DTR delta
  bool rxds;			// receiver idle interrupt flag
  bool airint;			// asynchronous IR wake interrupt flag
  bool awake;			// asynchronous wake interrupt flag
  // -- Status register 2 flags  -> active interrupts --  
  bool adet;			// automatic baud rate detect complete
  bool txfe;			// transmit buffer fifo empty
  bool dtrf;			// DTR edge triggered interrupt flag
  bool idle;			// IDLE condition
  bool acst;			// autobaud counter stopped
  bool ridelt;			// ring indicator delta
  bool riin;			// ring indicator input
  bool irint;			// serial infrared interrupt flag
  bool wake;			// wake
  bool dcddelt;			// data carrier detect delta
  bool dcdin;			// data carrier detect input
  bool rtsf;			// rts edge triggered interrupt flag
  bool txdc;			// transmitter complete
  bool brcd;			// BREAK condition detected
  bool ore;			// overrun error
  bool rdr;			// receive data ready

  void update_flags ();

  void do_reset (bool hard_reset = true)
  {
    // Initial values]
    if (hard_reset)
      {
	uart_enabled = false;
	rxd_enabled = false;
	txd_enabled = false;
      }
    rxd_pointer = 0;
    txd_pointer = 0;
    aden = false;
    adbr = false;
    trdyen = false;
    iden = false;
    icd = IC_4FRAMES;
    rrdyen = false;
    rxdmaen = false;
    iren = false;
    txmptyen = false;
    rtsden = false;
    txdmaen = false;
    atdmaen = false;
    esci = false;
    irts = false;
    ctsc = false;
    cts = false;
    escen = false;
    rtec = R_RISING;
    pren = false;
    proe = false;
    stpb = false;
    ws = WS_7BIT;
    rtsen = false;
    aten = false;
    dpec = D_RISING;
    dtren = false;
    parerren = false;
    fraerren = false;
    dsr = true;
    dcd = true;
    ri = true;
    adnimp = false;
    rxdsen = false;
    airinten = false;
    awaken = false;
    dtrden = false;
    rxdmuxsel = false;
    invt = false;
    acien = false;
    ctstl = 32;
    invr = false;
    eniri = false;
    wken = false;
    iddmaen = false;
    irsc = false;
    lpbyp = false;
    tcen = false;
    bken = false;
    oren = false;
    dren = false;
    txtl = 2;
    rfdiv = RF_DIVBY6;
    dcedte = false;
    rxtl = 1;
    parityerr = false;
    rtss = false;
    trdy = true;
    rtsd = false;
    escf = false;
    framerr = false;
    rrdy = false;
    agtim = false;
    dtrd = false;
    rxds = true;
    airint = false;
    awake = false;
    adet = false;
    txfe = true;
    dtrf = false;
    idle = false;
    acst = false;
    ridelt = false;
    riin = false;
    irint = false;
    wake = false;
    dcddelt = false;
    dcdin = true;
    rtsf = false;
    txdc = true;
    brcd = false;
    ore = false;
    rdr = false;

    *(regs + UART_URXD / 4) = 0;
    *(regs + UART_UTXD / 4) = 0;
    *(regs + UART_UCR1 / 4) = 0;
    *(regs + UART_UCR2 / 4) = 0x1;
    *(regs + UART_UCR3 / 4) = 0x700;
    *(regs + UART_UCR4 / 4) = 0x8000;
    *(regs + UART_UFCR / 4) = 0x801;
    *(regs + UART_USR1 / 4) = 0x2040;
    *(regs + UART_USR2 / 4) = 0x4028;
    *(regs + UART_UESC / 4) = 0x2B;
    *(regs + UART_UTIM / 4) = 0;
    *(regs + UART_UBIR / 4) = 0;
    *(regs + UART_UBRM / 4) = 0;
    *(regs + UART_UBRC / 4) = 0x4;
    *(regs + UART_ONEMS / 4) = 0;
    *(regs + UART_UTS / 4) = 0x60;
  }

  // This port is used to send interrupts to the processor
  tzic_module & tzic;



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
  void prc_uart ();

  // -- External signals

  SC_HAS_PROCESS (uart_module);

  uart_module (sc_module_name name_, tzic_module & tzic_);
  ~uart_module ();
};


#endif
