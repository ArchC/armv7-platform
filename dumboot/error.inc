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

        .set SUCCESS,                           0x00000000
        .set ERROR_INVALID_BOOT_MODE,           0x00000001
        .set ERROR_UNSUPORTED_BOOT_DEVICE,      0x00000002

        .set ERROR_NO_SD_CARD,                  0x00000003
        .set ERROR_SD_INIT_FAIL,                0x00000004
        .set ERROR_SD_SEND_IF_COND_FAIL,        0x00000005

        .set ERROR_SD_CMD_ERROR,                0x00000006
        .set ERROR_SD_CMD_TIMEOUT,              0x00000007

        .set ERROR_NO_IVT,                      0x00000008
        .set ERROR_NO_BOOTDATA,                 0x00000009


