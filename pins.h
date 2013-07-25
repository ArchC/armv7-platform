// 'pins.h' - Default definitions for external pins
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

#ifndef PINS_H
#define PINS_H

namespace PINS
{
  // Set TEST_MODE pins as low.
  const bool TEST_MODE[3] = { false, false, false };

  // Set BT_FUSE_SEL as low since we use eFUSE.
  const bool BT_FUSE_SEL = false;

  // Set BMOD to use eFUSE.
  const bool BMOD[2] = { false, true };

  // Set BOOT_CFG to show POR and use SD card.
  const char BOOT_CFG[3] = { 0x40, 0x00, 0x00 };
}

#endif // !PINS_H.
