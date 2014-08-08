// 'sd.h' - SD Card model
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
// Author : Gabriel Krisman Bertazi, 15/07/2012
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef SD_H
#define SD_H

#include "peripheral.h"
#include "tzic.h"

#include <sys/stat.h>
#include <systemc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <queue>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ac_tlm_protocol.H>

//This represents a generic SD card device based on the SD Card
// specification v3.0 to be connected to ESDHCV in the ARM SoC by
// Freescale iMX35.

enum response_type
{ R1 = 1, R1b, R1bCMD12, R2, R3, R4, R5, R5b, R6, R7 };

struct sd_response
{
  enum response_type type;
  unsigned char response[17];
};

class sd_card:public sc_module
{
  enum sd_state
  {
    SD_IDLE,			// Idle State
    SD_READY,			// Ready State
    SD_IDENT,			// Identification State
    SD_STBY,			// Standby State
    SD_TRAN,			// Transfer State
    SD_DATA,			// Sending Data
    SD_RCV,			// Receive Data
    SD_PRG,			// Programming State
    SD_DIS,			// Disconnect State
    SD_INA			// Inactive State
  };

  // CSD of a generic 4Gb card.
  static const char CSD[];
  // CID of a generic 4Gb card.
  static const char CID[];
  // SCR of a generic 4Gb card.
  static const char SCR[];

  // SCR BlockLen size.
  static const int SCR_SIZE = 8;

private:

  // FSM state pointer
  enum sd_state current_state;

  // Whether performing a single block operation.
  bool single_block_p;

  // Flag for treating next  command as application specific.
  bool application_specific_p;

  // Main pointer to SD memory on RAM
  void *data;

  // Size of SD card bin file
  size_t data_size;

  // Block Length defined by cmd16
  int blocklen;

  // Current accessed block
  uint32_t current_block;

  // Data line buffer.
  unsigned char data_line[4096];

  // RCA - Relative card address
  uint16_t rca;

  // Whether current card was selected by the hos.
  bool card_selected_p;

  // -- Bus width. --
  unsigned char bus_width;

  // -- States Handler --
  void exec_state_data ();

  // Command Handlers
  struct sd_response cmd0_handler (uint32_t arg);	// Set card to Idle.
  struct sd_response cmd2_handler (uint32_t arg);	// Send CID.
  struct sd_response cmd3_handler (uint32_t arg);	// Publish new RCA.
  struct sd_response cmd6_handler (uint32_t arg);	// Switch.
  struct sd_response cmd7_handler (uint32_t arg);	// Select/deselect card.
  struct sd_response cmd8_handler (uint32_t arg);	// Send if_cond.
  struct sd_response cmd9_handler (uint32_t arg);	// Send CSD.
  struct sd_response cmd12_handler (uint32_t arg);	// Suspend transfer.
  struct sd_response cmd13_handler (uint32_t arg);	// Send Status.
  struct sd_response cmd16_handler (uint32_t arg);	// Set_BlockLen.
  struct sd_response cmd17_handler (uint32_t arg);	// Read Single Block.
  struct sd_response cmd18_handler (uint32_t arg);	// Read Multiple Block.
  struct sd_response cmd55_handler (uint32_t arg);	// Next cmd is application

  // Application command handler
  struct sd_response acmd6_handler (uint32_t arg);	//Set bus width
  struct sd_response acmd41_handler (uint32_t arg);	//Get OCR
  struct sd_response acmd51_handler (uint32_t arg);	//Send SCR

  // Set up backend memory by loading an binary image file from disk.
  int load_image_from_file (const char *file);

  const char *state_to_string (const enum sd_state state);
  void update_state (const enum sd_state new_state);
public:

  // External signals
    SC_HAS_PROCESS (sd_card);
  void prc_sdcard ();

    sd_card (sc_module_name name_, const char *file);
   ~sd_card ();

  struct sd_response exec_cmd (short cmd_index, short cmd_type, uint32_t arg);

  // This function is used by external controllers to read the sd card
  // IO buffer It doesn't check any data integrity.
  bool read_dataline (std::queue < unsigned char >&buffer, uint32_t len);

  // Semaphor for data_line
  bool data_line_busy;
};

#endif // !SD_H.
