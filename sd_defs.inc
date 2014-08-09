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

	.set MMC_CMD_GO_IDLE_STATE,		0
	.set MMC_CMD_SEND_OP_COND,		1
	.set MMC_CMD_ALL_SEND_CID,		2
	.set MMC_CMD_SET_RELATIVE_ADDR,	        3
	.set MMC_CMD_SET_DSR,			4
	.set MMC_CMD_SWITCH,			6
	.set MMC_CMD_SELECT_CARD,		7
	.set MMC_CMD_SEND_EXT_CSD,		8
	.set MMC_CMD_SEND_CSD,			9
	.set MMC_CMD_SEND_CID,			10
	.set MMC_CMD_STOP_TRANSMISSION,		12
	.set MMC_CMD_SEND_STATUS,		13
	.set MMC_CMD_SET_BLOCKLEN,		16
	.set MMC_CMD_READ_SINGLE_BLOCK,	        17
	.set MMC_CMD_READ_MULTIPLE_BLOCK,	18
	.set MMC_CMD_WRITE_SINGLE_BLOCK,	24
	.set MMC_CMD_WRITE_MULTIPLE_BLOCK,	25
	.set MMC_CMD_ERASE_GROUP_START,	        35
	.set MMC_CMD_ERASE_GROUP_END,		36
	.set MMC_CMD_ERASE,			38
	.set MMC_CMD_APP_CMD,			55
	.set MMC_CMD_SPI_READ_OCR,		58
	.set MMC_CMD_SPI_CRC_ON_OFF,		59

	.set SD_CMD_SEND_RELATIVE_ADDR,         3
	.set SD_CMD_SWITCH_FUNC,		6
	.set SD_CMD_SEND_IF_COND,		8
	.set SD_CMD_SELECT_PARTITION,		43
	.set SD_CMD_MANAGE_PARTITIONS,	        44
	.set SD_CMD_QUERY_PARTITIONS,		45

	.set SD_CMD_APP_SET_BUS_WIDTH,	        6
	.set SD_CMD_ERASE_WR_BLK_START,	        32
	.set SD_CMD_ERASE_WR_BLK_END,		33
	.set SD_CMD_APP_SEND_OP_COND,		41
	.set SD_CMD_APP_SEND_SCR,		51

        .set MMC_RSP_NONE,                      0

	.set MMC_RSP_PRESENT,0b1
	.set MMC_RSP_136,    0b10		@@ 136 bit response
	.set MMC_RSP_CRC,    0b100		@@ expect valid crc
	.set MMC_RSP_BUSY,   0b1000		@@ card may send busy
	.set MMC_RSP_OPCODE, 0b10000		@@ response contains opcode

	.set MMC_RSP_NONE,   0
	.set MMC_RSP_R1,	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
	.set MMC_RSP_R1b,	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
	.set MMC_RSP_R2,	(MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
	.set MMC_RSP_R3,	(MMC_RSP_PRESENT)
	.set MMC_RSP_R4,	(MMC_RSP_PRESENT)
	.set MMC_RSP_R5,	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
	.set MMC_RSP_R6,	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
	.set MMC_RSP_R7,	(MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

        .set OCR_BUSY,		0x80000000

        .set READ_BLOCK_LEN,     512
        .set BLKSIZE,            512

