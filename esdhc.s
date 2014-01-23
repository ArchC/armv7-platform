@ -----[ esdhc.s ]---------------------------------------------------------@
@                                                                          @
@         i.MX53 SoC ESDHC based IO library                                @
@                                                                          @
@ -------------------------------------------------------------------------@
@ --[ Includes ]-----------------------------------------------------------@
@
	.include "esdhc_defs.inc"
      	.include "error.inc"
        .include "globals.inc"

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


@ --[ esdhc_get_response ]-----------------------------------------------@
@
@   Return Response for a command
@
@ Arguments
@	R0 - Response type
@
@ Returns
@       R0 - Return pointer to REPONSE_STRUCT
.globl esdhc_get_response
esdhc_get_response:
        stmfd sp!, {r4-r9, lr}
        ldr r4, =ESDHC_BASE
        ldr r1, =ESDHC_RESPONSE_STRUCT
        mov r9, r1

        tst r0, #MMC_RSP_136
        beq _get_response_simple

        ldr r8, [r4, #ESDHC_CMDRSP3]
        ldr r7, [r4, #ESDHC_CMDRSP2]
        ldr r6, [r4, #ESDHC_CMDRSP1]
        ldr r5, [r4, #ESDHC_CMDRSP0]

        @cmd->response[0]
        mov r2, r8, lsl #8
        orr r2, r2, r7, lsr #24
        str r2, [r1], #4

        @cmd->response[1]
        mov r2, r7, lsl #8
        orr r2, r2, r6, lsr #24
        str r2, [r1], #4

        @cmd->response[2]
        mov r2, r6, lsl #8
        orr r2, r2, r5, lsr #24
        str r2, [r1], #4

        @cmd->response[3]
        mov r2, r5, lsl #0x8
        str r2, [r1], #4

        b _end_of_get_response

_get_response_simple:
        @cmd->response[0]
        ldr r2, [r4, #ESDHC_CMDRSP0]
        str r2, [r1]

_end_of_get_response:
        mov r0, r9
        ldmfd sp!, {r4-r9, pc}


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

