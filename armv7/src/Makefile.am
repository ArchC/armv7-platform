# Copyright (C) 2013 Gabriel Krisman Bertazi <gabriel@krisman.be>
#
# This file is part of arm-sim.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

VPATH = $(srcdir) $(builddir)

bin_PROGRAMS = armsim

armsim_SOURCES = 	arm.ac \
	arm_interrupts.cpp \
	arm_syscall.cpp \
	main.cpp \
	bus.cpp \
	ccm.cpp \
	cp15.cpp \
	debug_backtrace.cpp \
	dpllc.cpp \
	esdhcv2.cpp \
	gpt.cpp \
	mmu.cpp \
	ram.cpp \
	rom.cpp \
	sd.cpp \
	src.cpp \
	tzic.cpp \
	uart.cpp \
	arm_arch.cpp \
	arm_decode_unit.cpp \
	arm_arch_ref.cpp	\
	arm_intr_handlers.cpp \
	arm_gdb_funcs.cpp

LDADD =  -lm -larchc -lsystemc

.ac.cpp:
	acsim arm.ac -gdb -np
#	$(CC) -c arm.cpp $(CPPFLAGS) -I. -I$(srcdir)

