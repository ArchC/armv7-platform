@ -----[ esdhc.s ]---------------------------------------------------------@
@                                                                          @
@         i.MX53 SoC ESDHC based IO library                                @
@                                                                          @
@ -------------------------------------------------------------------------@
@ --[ Includes ]-----------------------------------------------------------@
@
	.include "esdhc_defs.inc"
      	.include "error.inc"

@ --[ Global data ]--------------------------------------------------------@
@
	.data
	.align 4


@ --[ Module initialization ]----------------------------------------------@
@
@   Executed at boot time
	.text
	.align 4

	.globl configure_esdhc
configure_esdhc:
        stmfd sp!, {lr}

        ldr r0, =ESDHC_BASE

        @ Software reset edhc
        ldr r1, [r0, #ESDHC_SYSCTL]
        orr r1, r1, #SYSCTL_RSTA
        str r1, [r0, #ESDHC_SYSCTL]
_sw_reset_wait_loop:
        ldr r1, [r0, #ESDHC_SYSCTL]
        tst r1, #SYSCTL_RSTA
        bne _sw_reset_wait_loop

        @Reset MMCBOOT.
        mov r1, #0
        str r1, [r0, #ESDHC_MMCBOOT]

        @Reset PROCTL
        ldr r1, =PROCTL_INIT
        str r1, [r0, #ESDHC_PROCTL]

        @ Verify if a card is connected
        bl card_connected
        cmp r0, #0
        beq _no_sd_err

        ldr r0, =_STRING_MMC_INIT_
        mov r1, #17
        bl write
        mov r0, #SUCCESS
        b _end_of_configure_esdhc

_no_sd_err:
        ldr r0, =_STRING_ERROR_NO_SD_CARD_
        mov r1, #29
        bl write
        mov r0, #ERROR_NO_SD_CARD
_end_of_configure_esdhc:
        ldmfd sp!, {pc}

@ --[ card_connected ]---------------------------------------------------@
@
@   Verify whether there is a card connected, using the CINS bit.
@
@ Returns
@       R0 - 0 if no card was found
@          - > 0 if card was found
card_connected:
        ldr r0, =ESDHC_BASE
        ldr r0, [r0, #ESDHC_PRSSTAT]
        and r0, r0, #PRSSTAT_CINS
        mov pc, lr

_STRING_ERROR_NO_SD_CARD_:
        .asciz "ERROR: MMC: No SD card found\n"
_STRING_MMC_INIT_:
        .asciz "MMC Initialized.\n"

        .align 4
       	.globl init_sd
init_sd:
        mov pc, lr
