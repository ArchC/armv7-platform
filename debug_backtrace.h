// 'debug_backtrace.h' - Debug_backtrace
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
// Author : Gabriel Krisman Bertazi, 06/02/2013
//
// Please report bugs to <krisman.gabriel@gmail.com>
// ----------------------------------------------------------------------

#ifndef DEBUG_BACKTRACE_H
#define DEBUG_BACKTRACE_H

#include <stdint.h>		// define types uint32_t, etc
extern bool DEBUG_FLOW;
static int debug_btlevel = 0;

#define dprintbt_enter(entry, ret) if(DEBUG_FLOW) dprint_beginFunc(entry, ret);
#define dprintbt_leave()           if(DEBUG_FLOW) dprint_endFunc();
#define dprintbt_verifyEnd(lr)     dprint_verifyEndOfFunc(lr)

void dprint_beginFunc (uint32_t entry_add, uint32_t return_add);
void dprint_endFunc ();
bool dprint_verifyEndOfFunc (uint32_t lr);
void dprint_flow_indentation ();

#endif // !DEBUG_BACKTRACE_H
