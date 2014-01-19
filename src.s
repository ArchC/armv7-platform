@ -----[ src.s ]-----------------------------------------------------------@
@                                                                          @
@         i.MX53 SoC System Reset Controller IO library                    @
@                                                                          @
@ -------------------------------------------------------------------------@
@ --[ Includes ]-----------------------------------------------------------@
@
	.include "src_defs.inc"
@ --[ Global data ]--------------------------------------------------------@
@

.align 4
.text

.boot_device_table:
         .byte BOOTDEVICE_NOR_EIM      @ 0b0000
         .byte BOOTDEVICE_RESERVED     @ 0b0001
         .byte BOOTDEVICE_HARD_DISK    @ 0b0010
         .byte BOOTDEVICE_SROM         @ 0b0011
         .byte BOOTDEVICE_SD_ESD       @ 0b0100
         .byte BOOTDEVICE_SD_ESD       @ 0b0101
         .byte BOOTDEVICE_MMC_EMMC     @ 0b0110
         .byte BOOTDEVICE_MMC_EMMC     @ 0b0111
         .byte BOOTDEVICE_NAND         @ 0b1000
         .byte BOOTDEVICE_NAND         @ 0b1001
         .byte BOOTDEVICE_NAND         @ 0b1010
         .byte BOOTDEVICE_NAND         @ 0b1011
         .byte BOOTDEVICE_NAND         @ 0b1100
         .byte BOOTDEVICE_NAND         @ 0b1101
         .byte BOOTDEVICE_NAND         @ 0b1110
         .byte BOOTDEVICE_NAND         @ 0b1111
.align 4

@ --[ Module initialization ]----------------------------------------------@
@
@   Executed at boot time
	.text
	.align 4

@Nothing special to initialize at boot time

@ Return boot mode
@
@ 0 - Internal boot
@ 1 - Reserved
@ 2 - Boot From Fuses
@ 3 - Serial Downloader
.globl src_read_boot_mode
src_read_boot_mode:
        ldr r1, =SRC_BASE
        ldr r0, [r1, #SRC_SBMR]
        and r0, r0, #BMOD_MASK
        lsr r0, r0, #24
        mov pc, lr

@ Return boot device
@ Return code
@	R0 = 0x1	 NOR_EIM
@	R0 = 0x2	 RESERVED
@	R0 = 0x3	 HARD_DISK
@	R0 = 0x4	 SROM
@	R0 = 0x5	 SD_ESD
@	R0 = 0x6	 MMC_EMMC
@	R0 = 0x7	 NAND

.globl src_read_boot_device
src_read_boot_device:
        ldr r1, =SRC_BASE
        ldr r1, [r1, #SRC_SBMR]

        @Apply mask
        and r1, r1, #BOOTDEVICE_MASK
        lsr r1, r1, #4

        ldr r2, =.boot_device_table
        ldrb r0, [r2, r1]
        mov pc, lr
