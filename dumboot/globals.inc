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


@ Position where stack starts. any position beetween beggining of iRAM
@ and this is reserved for stack.

        .set TEMP_STACK, 0xF801F000

@ Position where ESDHC backups response structure. 16 bytes

        .set ESDHC_RESPONSE_STRUCT, 0xF801F010

@ Position where SD CID is stored. 16 bytes
        .set SD_CID, 0xF801F020

@ Position where SD RCA is stored. 4 bytes
        .set SD_RCA, 0xF801F030

@ Initial Load Buffer. 2K bytes (0x800 bytes) (in Iram)
        .set INITIAL_LOAD_BUFFER, 0xF801F100

@ Initial load ivt position 
        .set IVT_POSITION,  INITIAL_LOAD_BUFFER+0x400

