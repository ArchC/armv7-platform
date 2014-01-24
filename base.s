@ --[ Dumboot Bootstrap v1.0 ]----------------------------------------------------@
@
@   This is the main reset handler. It loads bootloader, DCD and jumps
@   to bootloader.
@
@ --[ Global data and definitions ]-----------------------------------------------@
@
	.data
	.align 4

.set UNDEFINED_iRAM,  0
.set SWI_iRAM,        0
.set PREFETCH_iRAM,   0
.set ABORT_iRAM,      0
.set IRQ_iRAM,        0
.set FIQ_iRAM,        0
.set SW_MONITOR_iRAM, 0

.set IVT_BASE,           0x400
.set IVT_HEADER_OFFSET,   0x00
.set IVT_ENTRY_OFFSET,    0x04
.set IVT_RESERV1_OFFSET,  0x08
.set IVT_DCD_OFFSET,      0x0C
.set IVT_BOOTDATA_OFFSET, 0x10
.set IVT_SELF_OFFSET,     0x14
.set IVT_CSF_OFFSET,      0x18
.set IVT_RESERV2_OFFSET,  0x1C

.set BOOT_MODE_FROM_FUSES, 0x2
.set BOOT_DEVICE_SD, 0x5

.include "error.inc"
.include "globals.inc"

.text
.org 0x0
.globl _start
_start:  @IROM Interruption table
@ --[ Interruption table  ]------------------------------------------------------@
@
@   This is the ROM hardcoded interruption table. It reroutes every type of
@   interruption, except reset, to another interruption table at the end of
@   iRAM
@
_interruptionTable:
b   RESET_HANDLER           @ RESET
ldr pc, =UNDEFINED_iRAM     @ UNDEFINED
ldr pc, =SWI_iRAM           @ SWI/SVC
ldr pc, =PREFETCH_iRAM      @ PREFETCH
ldr pc, =ABORT_iRAM         @ ABORT
nop                         @ RESERVED
ldr pc, =IRQ_iRAM           @ IRQ
ldr pc, =FIQ_iRAM           @ FIQ
ldr pc, =SW_MONITOR_iRAM    @ SW MONITOR


@ --[  META DATA  ]--------------------------------------------------------------@
@
@   Some MetaData information as defined by iMX53 manual.
@

.org 0x5B
        .globl _METADATA
 _METADATA:
       .ascii "Dumboot Bootstrap v1.0 - "
       .ascii "ARM ArchC model at LSC-UNICAMP\n"
       .ascii "Author: Gabriel Krisman Bertazi"
       .asciz "30/01/2013"


@ --[ RESET_HANDLER ]------------------------------------------------------@
@
@   This is the reset handler entry point. It loads bootloader, DCD and
@   jumps to bootloader.
@
@   This is the only entry point for dumboot and is executed by performing a
@   RESET trap.

        .org 0xC0
RESET_HANDLER:
        msr CPSR_c, #0xD3
        ldr sp, =TEMP_STACK
        b system_init


@ --[ system_init ]--------------------------------------------------------@
@
@   This is the reset handler entry point. It loads bootloader, DCD and
@   jumps to bootloader.
@
@   This is the only entry point for dumboot and is executed by performing a
@   RESET trap.

system_init:
        bl configure_uart
        bl print_banner
        bl find_boot_reason
        bl find_boot_device
        bl init_sd_device
        bl initial_load
        bl print_ivt

        ldr r0, =_STRING_BYE_
        mov r1, #57
        bl write

        @ Jump to entry code.
        ldr r0, =INITIAL_LOAD_BUFFER
        add r0, r0, #IVT_BASE
        ldr r1, [r0, #IVT_ENTRY_OFFSET]

        mov pc, r1

        @Finish Boot, lets hang for now.
        mov r0, #SUCCESS
        bl hang

@ --[ hang ]---------------------------------------------------------------@
@
@ Enter infinite loop. R0 should have error code.

hang:
        @backup error code
        mov r5, r0
        @Hang...
        ldr r0, =_STRING_HANG_
        mov r1, #11
        bl write
        @Restore error code
        mov r0, r5
_hang:
        b _hang

@ --[ print_banner ]-------------------------------------------------------@
@
@ Print Dumboot Banner to UART0. It should be already configured.

print_banner:
	stmfd sp!, {lr}
	ldr r0, =_METADATA
	mov r1, #56
	bl write
	ldmfd sp!, {pc}

@ --[ find_boot_reason ]---------------------------------------------------@
@
@   This function verifies bootmode pins to find out boot reason
@

find_boot_reason:
      	stmfd sp!, {lr}

        @@ Verify boot mode
        bl src_read_boot_mode
        mov r1, #BOOT_MODE_FROM_FUSES
        cmp r0, r1
        movne r0, #ERROR_INVALID_BOOT_MODE
        blne hang

        ldr r0, =_STRING_BOOT_REASON_POR_
        mov r1, #20
        bl write

	ldmfd sp!, {pc}

@ --[ find_boot_device ]---------------------------------------------------@
@
@   This function verifies bootmode pins to find out boot device.
@
find_boot_device:
        stmfd sp!, {lr}

        @@Verify boot device
        bl src_read_boot_device

        cmp r0, #BOOT_DEVICE_SD
        movne r0, #ERROR_UNSUPORTED_BOOT_DEVICE
        blne hang

        @ Boot device is SD card.
        ldr r0, =_STRING_BOOT_DEVICE_SD_
        mov r1, #18
        bl write

        ldmfd sp!, {pc}

@ --[ init_sd_device  ]--------------------------------------------------@
@
@
@   This function is responsible for initializing boot device.

init_sd_device:
        stmfd sp!, {lr}
        bl configure_esdhc
        cmp r0, #SUCCESS
        bne hang

        bl init_sd
        cmp r0, #SUCCESS
        bne hang
        ldmfd sp!, {pc}

@ --[ print_ivt  ]--------------------------------------------------@
@
@
@   This function is responsible for printing IVT

print_ivt:
        stmfd sp!, {lr}
        .warning "Should print IVT"
        ldmfd sp!, {pc}

@ --[ initial_load  ]--------------------------------------------------@
@
@
@   Perform initial load of ivt.
initial_load:
        stmfd sp!, {lr}
        ldr r0, =INITIAL_LOAD_BUFFER
        mov r1, #0
        mov r2, #4      @ 2k bytes.
        bl sd_read

        ldr r0, =_STRING_INITIAL_LOAD_
        mov r1, #58
        bl write

        ldr r0, =_STRING_IVT_LOAD_
        mov r1, #23
        bl write
        ldmfd sp!, {pc}

_STRING_IVTHEAD_:
        .asciz "\nImage Vector Table(IVT):\n"
        .asciz "Tag:     0xD1\n"
        .asciz "Length:  0x20\n"
        .asciz "Version: 0x40\n"

_STRING_BOOT_REASON_POR_:
        .asciz "\nBoot Reason: [POR]\n"
_STRING_BOOT_DEVICE_SD_:
        .asciz "Boot Device: [SD]\n"
_STRING_HANG_:
        .asciz "Hanging...\n"
_STRING_INITIAL_LOAD_:
        .asciz "Initial load section loaded at 0xf801f100 size=0x800 (2k)\n"
_STRING_IVT_LOAD_:
        .asciz "IVT offset is at 0x400\n"

_STRING_BYE_:
        .asciz "\nTransfering control to loaded code at 0x778005e00...\n\n"

