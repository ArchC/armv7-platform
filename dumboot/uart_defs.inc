@ ICBoot - Bootstrapping Code for ARMv7-sim
@
@ Copyright (C) 2014 Gabriel Krisman Bertazi <gabriel@krisman.be>
@
@ This program is free software: you can redistribute it and/or modify
@ it under the terms of the GNU General Public License as published by
@ the Free Software Foundation, either version 3 of the License, or
@ (at your option) any later version.
@
@ This program is distributed in the hope that it will be useful,
@ but WITHOUT ANY WARRANTY; without even the implied warranty of
@ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
@ GNU General Public License for more details.
@
@ You should have received a copy of the GNU General Public License
@ along with this program.  If not, see <http://www.gnu.org/licenses/>.

	.set UART_URXD, 0x0
	.set UART_UTXD, 0x40
	.set UART_UCR1, 0x80
	.set UART_UCR2, 0x84
	.set UART_UCR3, 0x88
	.set UART_UCR4, 0x8C
	.set UART_UFCR, 0x90
	.set UART_USR1, 0x94
	.set UART_USR2, 0x98
	.set UART_UESC, 0x9C
	.set UART_UTIM, 0xA0
	.set UART_UBIR, 0xA4
	.set UART_UBRM, 0xA8
	.set UART_UBRC, 0xAC
	.set UART_ONEMS, 0xB0
	.set UART_UTS, 0xB4
	.set UART_BASE, 0x53FBC000
	.set UART_IRQNUM, 31

