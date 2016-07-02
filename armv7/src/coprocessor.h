// "coprocessor.h" - Generic Coprocessor interface
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
// Author : Gabriel Krisman Bertazi, 05/10/2012
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef COPROCESSOR_H
#define COPROCESSOR_H

#include "arm_interrupts.h"

class coprocessor
{
  virtual void reset () = 0;

public:
    virtual void MCR (arm_arch_ref * core, const arm_impl::PrivilegeLevel pl,
		      const unsigned opc1, const unsigned opc2,
		      const unsigned crn, const unsigned crm,
		      const unsigned rt_value) = 0;

  virtual unsigned MRC (arm_arch_ref * core,
			const arm_impl::PrivilegeLevel pl,
			const unsigned opc1, const unsigned opc2,
			const unsigned crn, const unsigned crm) = 0;
};

#endif // !COPROCESSOR_H.
