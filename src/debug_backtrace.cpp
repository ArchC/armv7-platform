// "debug_backtrace.cpp" - Debug Backtrace
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

#include "debug_backtrace.h"
#include<stack>
#include<stdio.h>

static
  std::stack <
  uint32_t >
  rets;

void
dprint_beginFunc (uint32_t entry_add, uint32_t return_add)
{
  debug_btlevel++;
  dprint_flow_indentation ();
  fprintf (stderr, "<Entered Func => 0x%X.  Return address => 0x%X>\n",
	   entry_add, return_add);

  rets.push (return_add);
}

void
dprint_endFunc ()
{

  if (rets.size () == 0)
    return;
  dprint_flow_indentation ();
  fprintf (stderr, "<Returning to address 0x%X>\n", rets.top ());
  debug_btlevel--;
  rets.pop ();
}

bool
dprint_verifyEndOfFunc (uint32_t lr)
{
  if (DEBUG_FLOW && rets.size () > 0)
    return (lr == rets.top ());
  else
    return false;
}

void
dprint_flow_indentation ()
{
  for (int i = 0; i < debug_btlevel; i++)
    fprintf (stderr, "    ");
}
