// 'esdhcv2.h' - enhanced SD Host Controller 2 model.
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
// Author : Gabriel Krisman Bertazi, 16/11/2012
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef ESDHC_H
#define ESDHC_H

#include <stdint.h>
#include "peripheral.h"
#include "tzic.h"
#include <systemc.h>
#include <ac_tlm_protocol.H>
#include "sd.h"
#include <queue>

class esdhc_module:public sc_module, public peripheral
{
  enum state
  {
    IDLE,
    HOST_WRITE,
    HOST_READ_TRANSFER,
    HOST_READ_DONE,
    HOST_WRITE_DONE,
  };

  enum irqstat
  {
    IRQ_DMAE = 28,
    IRQ_AC12E = 27,
    IRQ_DEBE = 22,
    IRQ_DCE = 21,
    IRQ_DTOE = 20,
    IRQ_CIE = 19,
    IRQ_CEBE = 18,
    IRQ_CCE = 17,
    IRQ_CTOE = 16,
    IRQ_CINT = 8,
    IRQ_CRM = 7,
    IRQ_CINS_int = 6,
    IRQ_BRR = 5,
    IRQ_BWR = 4,
    IRQ_DINT = 3,
    IRQ_BGE = 2,
    IRQ_TC = 1,
    IRQ_CC = 0
  };

  enum state current_state;

  // Internal Registers
  static const uint32_t DSADR = 0x00;	// DMA System Address
  static const uint32_t BLKATTR = 0x04;	// Block attribute
  static const uint32_t CMDARG = 0x08;	// Command Argument
  static const uint32_t XFERTYP = 0x0C;	// Command Transfer type
  static const uint32_t CMDRSP0 = 0x10;	// Command Response 0
  static const uint32_t CMDRSP1 = 0x14;	// Command Response 1
  static const uint32_t CMDRSP2 = 0x18;	// Command Response 2
  static const uint32_t CMDRSP3 = 0x1C;	// Command Response 3
  static const uint32_t DATPORT = 0x20;	// Data buffer access port
  static const uint32_t PRSSTAT = 0x24;	// Present State
  static const uint32_t PROCTL = 0x28;	// Protocol Control
  static const uint32_t SYSCTL = 0x2C;	// System Control
  static const uint32_t IRQSTAT = 0x30;	// Interrupt Status
  static const uint32_t IRQSTATEN = 0x34;	// Interrupt Status Enable
  static const uint32_t IRQSIGEN = 0x38;	// Interrupt signal Enable
  static const uint32_t AUTOC12ERR = 0x3C;	// Auto CMD12 Error
  static const uint32_t HOSTCAPBLT = 0x40;	// host Capatibilites
  static const uint32_t WML = 0x44;	// Watermark Level
  static const uint32_t FEVT = 0x50;	// Force Event
  static const uint32_t ADMAEST = 0x54;	// ADMA Error Status
  static const uint32_t ADSADDR = 0x58;	// ADMA System Address
  static const uint32_t VENDOR = 0xC0;	// Vendor Specific Register
  static const uint32_t MMCBOOT = 0xC4;	// Fast boot Reg.
  static const uint32_t HOSTVER = 0xFC;	// Host Controller Version
  static const uint32_t ESDHCV_LASTADDR = 0x100;

  unsigned regs[ESDHCV_LASTADDR / 4];

  //XFERTYP
  bool DMAEN;
  bool BCEN;
  bool AC12EN;
  bool DTDSEL;
  bool MSBSEL;
  char RSPTYP;
  bool CCCEN;
  bool CICEN;
  bool DPSEL;
  char CMDTYP;
  char CMDINX;
  //--

  //Present State
  char DLSL;
  bool CLSL;
  bool WPSPL;
  bool CDPL;
  bool CINS;
  bool BREN;
  bool BWEN;
  bool RTA;
  bool WTA;
  bool SDOFF;
  bool PEROFF;
  bool HCKOFF;
  bool IPGOFF;
  bool SDSTB;
  bool DLA;
  bool CDIHB;
  bool CIHB;
  //--

  //Protocol control
  bool WECRM;
  bool WECINS;
  bool WECINT;
  bool IABG;
  bool RWCTL;
  bool CREQ;
  bool SABGREQ;
  bool DMAS[2];
  bool CDSS;
  bool CDTL;
  bool EMODE[2];
  bool D3CD;
  bool DTW[2];
  bool LCTL;
  //--

  //System Control
  bool INITA;
  bool RSTD;
  bool RSTC;
  bool RSTA;
  char DTOCV;
  char SDCLKFS;
  char DVS;
  bool SDCLKEN;
  bool PEREN;
  bool HCKEN;
  bool IPGEN;
  //--

  // Watermark Level
  unsigned char WR_BRST_LEN;
  unsigned char WR_WML;
  unsigned char RD_BRST_LEN;
  unsigned char RD_WML;
  //--
  //ADMA Error Status
  bool ADMADCE;
  bool ADMALME;
  char ADMAES;
  //--

  //BLKATTR
  uint16_t BLKCNT;
  uint16_t BLKSIZE;
  //Since we must restore BLKCNT value after a MCD12 is issued
  uint16_t BLKCNT_BKP;
  //--

  static const int ESDHCV2_1_IRQ = 1;

  sd_card *port;
    std::queue < unsigned char >ibuffer;

  // This port is used to send interrupts to the processor
    tzic_module & tzic;

  // Fast read/write don't implement error checking. The bus (or other caller)
  // must ensure the address is valid.
  // Invalid read/writes are treated as no-ops.
  // Unaligned addresses have undefined behavior
  unsigned fast_read (unsigned address);
  void fast_write (unsigned address, unsigned datum);

  void sd_protocol ();
  void host_protocol ();

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
  void prc_ESDHCV2 ();

  // -- External signals
  SC_HAS_PROCESS (esdhc_module);
  esdhc_module (sc_module_name name_, tzic_module & tzic_);

  void connect_card (sd_card *card);

private:

  void reset_DAT_line ();

  // This function handles every type of outgoing ESDHC interruption.It
  // is responsable for checking if that kind of interruption
  // can be asserted and if so, sets IRQSTAT, and service_interrupt()
  // as necessary.
  // It is controlled by IRQSTATEN and IRQSIGEN

  void generate_signal (enum irqstat irqnum);
  void execute_xfertyp_command ();
  void update_state (const enum state new_state);

  // This method decodes a sd_response and distribute the response code
  // among CMD_RESP registers.
  void decode_response (struct sd_response response);

  void do_reset (bool hard_reset = true)
  {
    // Initial values
    regs[DSADR / 4] = 0x0;
    regs[BLKATTR / 4] = 0x0;
    regs[CMDARG / 4] = 0x0;

    //XFERTYP
    DMAEN = false;
    BCEN = false;
    AC12EN = false;
    DTDSEL = false;
    MSBSEL = false;
    RSPTYP = 0;
    CCCEN = false;
    CICEN = false;
    DPSEL = false;
    CMDTYP = 0;
    CMDINX = 0;
    //--

    regs[CMDRSP0 / 4] = 0;
    regs[CMDRSP1 / 4] = 0;
    regs[CMDRSP2 / 4] = 0;
    regs[CMDRSP3 / 4] = 0;
    regs[DATPORT / 4] = 0;

    //Present State = 0x0
    DLSL = 0;
    CLSL = false;
    WPSPL = false;
    CDPL = false;
    CINS = (port)? true:false; // Set if there is a card connected.
    BREN = false;
    BWEN = false;
    RTA = false;
    WTA = false;
    SDOFF = false;
    PEROFF = false;
    HCKOFF = false;
    IPGOFF = false;
    SDSTB = false;
    DLA = false;
    CDIHB = false;    CIHB = false;
    //--

    //Protocol control
    WECRM = false;
    WECINS = false;
    WECINT = false;
    IABG = false;
    RWCTL = false;
    CREQ = false;
    SABGREQ = false;
    DMAS[1] = false;
    CDSS = false;
    CDTL = false;
    EMODE[1] = false;
    D3CD = false;
    DTW[1] = false;
    LCTL = false;
    //--

    //System Control 0x8008
    INITA = false;
    RSTD = false;
    RSTC = false;
    RSTA = false;
    DTOCV = false;
    SDCLKFS = 0x08;
    DVS = false;
    SDCLKEN = true;
    PEREN = false;
    HCKEN = false;
    IPGEN = false;
    //--
    regs[WML / 4] = 0x08100810;
    WR_BRST_LEN = 0x8;
    WR_WML = 0x10;
    RD_BRST_LEN = 0x8;
    RD_WML = 0x10;

    regs[IRQSTAT / 4] = 0x0;
    regs[IRQSTATEN / 4] = 0x117F013F;
    regs[IRQSIGEN / 4] = 0x0;
    regs[AUTOC12ERR / 4] = 0x0;
    regs[HOSTCAPBLT / 4] = 0x07F30000;
    regs[ADMAES / 4] = 0x0;
    regs[ADSADDR / 4] = 0x0;
    regs[VENDOR / 4] = 0x1;
    regs[MMCBOOT / 4] = 0x0;
    regs[HOSTVER / 4] = 0x00001201;
    //--
  }
};

#endif // !ESDHC_H
