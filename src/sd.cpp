// "sd.cpp" - SD card model
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

#include "sd.h"
#include <errno.h>

extern bool DEBUG_SD;
#define dprintf(args...) if(DEBUG_SD){fprintf(stderr,args);}

// CSD of a generic 4Gb card. Extracted from imx53 boot SD card.
const char sd_card::CSD[] = { 0x40, 0x0e, 0x00, 0x32,
                              0x5b, 0x59, 0x00, 0x00,
                              0x1d, 0x6f, 0x7f, 0x80,
                              0x0a, 0x40, 0x00, 0x00 };

// CID of a generic 4Gb card. Extracted from imx53 boot SD card.
const char sd_card::CID[] = { 0x02, 0x54, 0x4d, 0x53,
                              0x41, 0x30, 0x34, 0x47,
                              0x06, 0x23, 0x65, 0xd7,
                              0xfd, 0x00, 0xb2, 0x00 };

// SCR of a generic 4Gb card. Extracted from imx53 boot SD card.
const char sd_card::SCR[] = { 0x02, 0x35, 0x80, 0x00,
                              0x01, 0x00, 0x00, 0x00 };

sd_card::sd_card (sc_module_name name_, const char *file):
sc_module (name_)
{
  //*command_handler[0] = cmd0_handler;

  if (load_image_from_file (file) != 0)
    {
      exit (1);
    }

  // Set current state to idle.
  this->current_state = SD_IDLE;

  // Set dataline empty.
  this->data_line_busy = false;

  // Set next command as non-application specific.
  this->application_specific_p = false;

  // Every RCA is initialized with zeroes.
  this->rca = 0x00;

  this->card_selected_p = false;

  // A SystemC thread never finishes execution, but transfers control
  // back to SystemC kernel via wait() calls.
  SC_THREAD (prc_sdcard);

};

sd_card::~sd_card ()
{
  //munmap: free data allocated by mmap
  if (munmap (data, data_size) != 0)
    {
      fprintf (stderr, "%s: Unable to free SD mmapped memory", this->name ());
    }
}

// Load a given binary image from disk to SD_card memory backend.  It
// attemps to mmap file passed as argument to data pointer.  On success
// defines data_size and returns 0. On failure returns -1.
// data pointes must be unmapped when no longer needed.
int
sd_card::load_image_from_file (const char *file)
{
  int dataFile;
  struct stat st;

  // Check if a file was provided.
  if (file == NULL)
    {
      printf ("No sd image provided");
      return -1;
    }

  fprintf (stderr, "ArchC: %s: Loading SD card file: %s\n",
	   this->name (), file);

  dataFile = open (file, O_LARGEFILE);
  if (dataFile == -1)
    {
      fprintf (stderr, "%s: Unable to load file %s: Error: %s",
	       this->name (), file, strerror (errno));
      return -1;
    }

  stat (file, &st);
  this->data_size = st.st_size;

  this->data = mmap (NULL, data_size, PROT_READ | PROT_WRITE,
		     MAP_PRIVATE, dataFile, 0);

  if (data == MAP_FAILED)
    {
      fprintf (stderr, "%s: Unable to mmap file %s: Error: %s\n",
	       this->name (), file, strerror (errno));
      return -1;
    }

  close (dataFile);
  return 0;
}

// Main SC thread. This executes every SD card clock cycle and executes
// SD current state.
void
sd_card::prc_sdcard ()
{
  do
    {
      wait (1, SC_NS);

      if (current_state == SD_IDLE)
	continue;

      //      dprintf ("-------------------- SD CARD -------------------- \n");

      // Main FSM dispatcher.
      switch (current_state)
	{
	case SD_DATA:
	  exec_state_data ();
	  break;
        case SD_READY:
        case SD_IDENT:
        case SD_STBY:
        case SD_TRAN:
        case SD_DIS:
        case SD_INA:
          continue;
	default:
	  printf ("%s: FSM State %d was not implemented in this model\n",
		  this->name (), current_state);
	  exit (1);
	}
    }
  while (1);
}

//  Current state DATA procedure.
void
sd_card::exec_state_data ()
{
  if (data_line_busy)
    return;			// Avoid overwritting unread data on dataline

  dprintf ("%s: Block Read: Sending data from block 0x%x to bus. blocklen=%d\n",
	   this->name (), current_block, blocklen);

  // Send next block to dataline.

  memcpy (data_line, &( ((char *) data)[current_block*blocklen]), blocklen);
  current_block += 1;
  data_line_busy = true;

  //If single read, stop it
  if (single_block_p == true)
    update_state (SD_TRAN);
}

// This function is used by external controllers to read the sd card IO buffer
// It doesn't check any data integrity.
bool
sd_card::read_dataline (std::queue < unsigned char >&buffer, uint32_t len)
{
  if (data_line_busy)
    {
      for (int i = 0; i < blocklen; i++)
	buffer.push (data_line[i]);

      data_line_busy = false;	//Dataline is cleared for new data.
      return true;
    }
  return false;
}

//  SD Specification commands Handlers
struct sd_response
sd_card::exec_cmd (short cmd_index, short cmd_type, uint32_t arg)
{
  if (!application_specific_p)
    {
      //Routes each command to its handler
      switch (cmd_index)
	{
	case 0:
	  return cmd0_handler (arg);
	case 2:
	  return cmd2_handler (arg);
        case 3:
          return cmd3_handler (arg);
	case 6:
	  return cmd6_handler (arg);
	case 7:
	  return cmd7_handler (arg);
	case 8:
	  return cmd8_handler (arg);
	case 9:
	  return cmd9_handler (arg);
	case 12:
	  return cmd12_handler (arg);
          //   case 13:
          //	  return cmd13_handler (arg);
	case 16:
	  return cmd16_handler (arg);
	case 17:
	  return cmd17_handler (arg);
	case 18:
	  return cmd18_handler (arg);
	case 55:
	  return cmd55_handler (arg);
        case 43:
          struct sd_response resp;
          resp.type = R1;
          return resp;
	default:
	  fprintf (stderr, "%s: CMD%d not supported/implemented "
		   "in this model\n", this->name (), cmd_index);
	  exit (1);
        }
    }
  else
    {
      // Next command is not application specific.
      application_specific_p = false;

      //Routes each application command to its handler
      switch (cmd_index)
	{
	case 6:
	  return acmd6_handler (arg);
	case 41:
	  return acmd41_handler (arg);
	case 51:
	  return acmd51_handler (arg);
	default:
	  fprintf (stderr, "%s: ACMD%d not supported/implemented "
		   "in this model\n", this->name (), cmd_index);
	  exit (1);
	}
    }
}

// CMD0 ==>  SET_CARD_TO_IDLE
struct sd_response
sd_card::cmd0_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s: CMD0: arg=NOARG\n",
	   this->name ());

  update_state (SD_IDLE);

  data_line_busy = false;
  // No response. Just return anything.
  resp.type = R1;
  return resp;
}

// CMD2 ==>  SEND_CID
struct sd_response
sd_card::cmd2_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s: CMD2: arg=NOARG\nCurrent State: %s\n",
	   this->name (), state_to_string (current_state));

  resp.type = R2;
  if (current_state == SD_READY)
    {
      update_state (SD_IDENT);

      resp.response[0] = 0x3F;
      memcpy (&(resp.response[1]), CID, 16);
      resp.response[16] |= 0x1;  // Replace last bit with EOT bit.
    }
  else
    {
      dprintf ("Invalid command for this mode.");
      resp.response[0] = 0x03;
      resp.response[1] = 0x0;
      resp.response[2] = 0x0;
      resp.response[3] = 0x0;
      resp.response[3] = 0x0;
      resp.response[4] = 0x1;	// Fake CRC and end transmission
    }
  return resp;

}

// CMD3 ==>  GENERATE_NEW_RCA
struct sd_response
sd_card::cmd3_handler (uint32_t arg)
{
  struct sd_response resp;

  dprintf ("%s: CMD3: arg=NOARG\n", this->name ());

  resp.type = R6;
  if (current_state == SD_IDENT || current_state == SD_STBY)
    {
      update_state (SD_STBY);

      // Generate a new RCA.
      rca = 1;
      dprintf ("%s: New RCA=%d \n", this->name (), this->rca);

      resp.response[0] = 0x03;
      resp.response[1] = (rca & 0xFF00) >> 8;
      resp.response[2] = (rca & 0x00FF);
      resp.response[3] = 0x0;	//Dummy values for card status
      resp.response[3] = 0x0;	// ^
      resp.response[4] = 0x1;	// Fake CRC and end transmission
    }
  else
    {
      dprintf ("Invalid command for this mode.");
      resp.response[0] = 0x03;
      resp.response[1] = 0x0;
      resp.response[2] = 0x0;
      resp.response[3] = 0x0;
      resp.response[3] = 0x0;
      resp.response[4] = 0x1;	// Fake CRC and end transmission
    }
  return resp;
}

// CMD6 ==> SWITCH
struct sd_response
sd_card::cmd6_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s CMD6: arg=0x%X (RCA)\n", this->name (), arg);

  // If Switch mode 0.
  if (!(arg & (1 << 31)))
    {
      blocklen = 64;
      memset (data_line, 0x0, blocklen);
      data_line_busy = true;
    }
  else // Mode 1.
    {
      printf ("%s: Mode 1 not implemented in SWITCH instruction",
              this->name());
      exit (1);
    }
  return resp;
}

// CMD7 ==> SELECT/DESELECT CARD
struct sd_response
sd_card::cmd7_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s CMD7: arg=arg (RCA)\n", this->name ());

  arg = arg >> 16;
  if (arg == rca && arg != 0x0)
    {
      // This card is selected.
      card_selected_p = true;

      if (current_state == SD_STBY)
	{
	  update_state (SD_TRAN);
	}
      else if (current_state == SD_DIS)
	{
	  update_state (SD_PRG);
	}
    }
  else
    {
      // Card was deselected.
      card_selected_p = false;

      if (current_state == SD_STBY
	  || current_state == SD_TRAN || current_state == SD_DATA)
	{
	  update_state (SD_STBY);
	}
      else if (current_state == SD_PRG)
	{
	  update_state (SD_PRG);
	}
    }

  resp.type = R1b;
  resp.response[0] = 0xC7;
  resp.response[1] = 0x00;
  resp.response[2] = 0x00;
  resp.response[3] = 0x00;	//Dummy values for card status
  resp.response[3] = 0x00;	// ^
  resp.response[4] = 0x01;	// Fake CRC and end transmission

  return resp;
}

// CMD8 ==>  SEND_IF_COND
struct sd_response
sd_card::cmd8_handler (uint32_t arg)
{
  struct sd_response resp;
  unsigned char check_pattern;
  unsigned char voltage;

  dprintf ("%s: CMD8: arg=0x%X\n", this->name (), arg);

  resp.type = R7;
  if (current_state == SD_IDLE)
    {
      // Decode argument.
      check_pattern = arg & 0xFF;
      voltage = (arg & 0xF00) >> 8;

      // Build response string.

      resp.response[0] = 0x48;	// cmdindx = 8
      resp.response[1] = 0x00;	// Reserved
      resp.response[2] = 0x00;	// Reserved
      resp.response[3] = voltage;	// Voltage supplied
      resp.response[4] = check_pattern;	//Check pattern
      resp.response[5] = 0x01;	//Fake CRC and end bit.
    }
  return resp;
}

// CMD9 ==>  SEND_CSD
struct sd_response
sd_card::cmd9_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s: CMD9: arg=NOARG\n", this->name ());
  resp.type = R2;

  if (current_state == SD_STBY)
    {
      resp.response[0] = 0x3F;
      memcpy (&(resp.response[1]), CSD, 16);
    }

  return resp;
}

// CMD12 ==>  STOP_MULTIBLOCK READ
struct sd_response
sd_card::cmd12_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s: CMD12: arg=NOARG\n", this->name ());

  if (current_state == SD_DATA)
    {
      update_state (SD_TRAN);
    }
  else if (current_state == SD_RCV)
    {
      update_state (SD_PRG);
    }
  return resp;

}

// CMD13 ==>  SEND STATUS
struct sd_response
sd_card::cmd13_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s: CMD13: arg=NOARG\n", this->name ());

  // if (((arg & 0xFFFF0000) >> 16) != rca)
  //   return resp;

  if ((current_state == SD_IDLE) || (current_state == SD_READY)
      || (current_state == SD_IDENT) || (current_state == SD_INA))
      return resp;

  resp.response[0] = (0b11 << 6) | 13;
  resp.response[1] = 0x0;
  resp.response[2] = 0x0;
  resp.response[3] = 0x0;
  resp.response[4] = 0x0;
  resp.response[4] = 0x1;

  return resp;
}

// CMD16 ==>  SET_LOCKLEN
struct sd_response
sd_card::cmd16_handler (uint32_t arg)
{
  struct sd_response resp;

  dprintf ("%s: CMD16: arg=0x%X\n", this->name (), arg);

  if (current_state == SD_TRAN)
    {
      if (arg > 4098)
	{
	  fprintf (stderr, " SDCARD: BLOCKLEN too high. "
		   "Overflow internal Buffer. aborting\n");
	  exit (1);
	}

      blocklen = arg;
    }

  resp.response[0] = 0;
  resp.response[1] = 0;
  resp.response[2] = 0;
  return resp;
}

// CMD17 ==>  READ_SINGLEBLK
struct sd_response
sd_card::cmd17_handler (uint32_t arg)
{
  struct sd_response resp;

  dprintf ("%s: CMD17: arg=0x%X\n", this->name (), arg);

  if (current_state == SD_TRAN)
    {
      data_line_busy = false;
      current_block = arg;
      single_block_p = true;
      // Enter transfer mode
      update_state (SD_DATA);
    }

  return resp;
}

// CMD18 ==>  READ_MULTIPLE_BLOCK
struct sd_response
sd_card::cmd18_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s: CMD18: arg=0x%X\n", this->name (), arg);

  if (current_state == SD_TRAN)
    {
      current_block = arg;
      single_block_p = false;
      update_state (SD_DATA);
    }
  return resp;
}

// CMD55 ==> APP_CMD
struct sd_response
sd_card::cmd55_handler (uint32_t arg)
{
  struct sd_response resp;
  dprintf ("%s: CMD55: arg=0x%X. Next command must be application "
	   "specific\n", this->name (), arg);

  if (current_state != SD_READY && current_state != SD_INA
      && current_state != SD_IDENT)
    {
      application_specific_p = true;
    }

  resp.type = R1;
  return resp;
}

// Application command handler
struct sd_response
sd_card::acmd41_handler (uint32_t arg)
{
  struct sd_response resp;

  dprintf ("%s: ACMD41: arg=0x%X\n", this->name (), arg);

  if (current_state == SD_IDLE)
    {
      update_state (SD_READY);

      resp.type = R3;
      resp.response[0] = 0x3F;	//Begin of trasmission
      resp.response[1] = 0xFF;	// Fake OCR
      resp.response[2] = 0xFF;	//
      resp.response[3] = 0xFF;	//
      resp.response[4] = 0xFF;	// Last bit indicates we're not busy.
      resp.response[5] = 0xFF;	//End of transmission
    }
  return resp;
}

struct sd_response
sd_card::acmd51_handler (uint32_t arg)
{
  struct sd_response resp;

  dprintf ("%s: ACMD51: arg=%d\n", this->name (), arg);

  // Send SCR to command line.
  blocklen = SCR_SIZE;
  memcpy (data_line, SCR, blocklen);
  data_line_busy = true;

  resp.type = R1;
  resp.response[0] = 0x0;
  resp.response[1] = 0x0;
  resp.response[2] = 0x0;
  resp.response[3] = 0x0;
  resp.response[5] = 0xFF;	//End of transmission
  return resp;
}

struct sd_response
sd_card::acmd6_handler (uint32_t arg)
{
  struct sd_response resp;

  dprintf ("%s: ACMD6: arg=%d\n", this->name (), arg);

  dprintf ("%s: Driver setting bus_width. Ignoring\n", this->name ());

  bus_width = arg & 0b11;
  return resp;

}

// FSM handlers.
const char *
sd_card::state_to_string (const enum sd_state state)
{
  switch (state)
    {
    case SD_IDLE:
      return "IDLE";
    case SD_READY:
      return "READY";
    case SD_IDENT:
      return "IDENT";
    case SD_STBY:
      return "STAND-BY";
    case SD_TRAN:
      return "TRANSFER";
    case SD_DATA:
      return "DATA";
    case SD_RCV:
      return "RECEIVE";
    case SD_PRG:
      return "PROGRAM";
    case SD_DIS:
      return "DISCONNECT";
    case SD_INA:
      return "INACTIVE";
    }
  return 0;
}

void
sd_card::update_state (const enum sd_state new_state)
{
  dprintf ("%s: updating state: Last State: %s Current state: %s\n",
	   this->name (), state_to_string (current_state),
	   state_to_string (new_state));

  current_state = new_state;
}

