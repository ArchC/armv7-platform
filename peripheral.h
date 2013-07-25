// 'peripheral.h' - Default interface for peripheral devices
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

#ifndef  PERIPHERAL_H
#define  PERIPHERAL_H

#include<stdint.h>

class peripheral
{

public:
  virtual uint32_t read_signal (uint32_t address, uint32_t offset) = 0;

  virtual void write_signal (uint32_t address,
			     uint32_t datum, uint32_t offset) = 0;
};

#endif // !PERIPHERAL_H.
