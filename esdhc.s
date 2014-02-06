@ -----[ esdhc.s ]---------------------------------------------------------@
@                                                                          @
@         i.MX53 SoC ESDHC based IO library                                @
@                                                                          @
@       Based on the U-boot esdhc driver code.                             @
@                                                                          @
@      By Gabriel Krisman Bertazi       01/2014                            @
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

@ --[ esdhc_get_response ]---------------------------------------------------@
@
@   Return pointer to response structure.
@
@ Returns
@       R0 - Return pointer to REPONSE_STRUCT
        .globl esdhc_get_response
esdhc_get_response:
        ldr r0, =ESDHC_RESPONSE_STRUCT
        mov pc, lr

@ --[ esdhc_fill_response_struct ]-----------------------------------------@
@
@   Update response structure.
@
@ Arguments
@	R0 - Response type
@
@ Returns
@       R0 - Return pointer to REPONSE_STRUCT
esdhc_fill_response:
        stmfd sp!, {r4-r9, lr}
        ldr r4, =ESDHC_BASE
        ldr r1, =ESDHC_RESPONSE_STRUCT
        mov r9, r1

        tst r0, #MMC_RSP_136
        beq _fill_response_struct_simple

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

        b _end_of_fill_response_struct

_fill_response_struct_simple:
        @cmd->response[0]
        ldr r2, [r4, #ESDHC_CMDRSP0]
        str r2, [r1]

_end_of_fill_response_struct:
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

@ --[ esdhc_set_block_attr ]-----------------------------------------------@
@
@ Set block count on BLKATTR register.
@
@ Arguments
@        R0 - Block count
@        R1 - Block size
        .globl esdhc_set_block_attr
esdhc_set_block_attr:
        ldr r3, =ESDHC_BASE
        orr r0, r1, r0, lsl #16
        str r0, [r3, #ESDHC_BLKATTR]
        mov pc, lr

@ --[ esdhc_get_block_count ]-----------------------------------------------@
@
@ Get block count on BLKATTR register.
@
@ Returns:
@        R0 - Block count
esdhc_get_block_attr:
        ldr r1, =ESDHC_BASE
        ldr r0, [r1, #ESDHC_BLKATTR]
        mov pc, lr

@ --[ esdhc_send_command ]-----------------------------------------------@
@
@   Send command to bus
@
@ Argument
@       R0 - CMD index
@       R1 - Argument
@       R2 - RSPTYPE
@       r3 - Pointer to data region. Might be NULL if
@            command does not require data.
@ Returns
@       R0 - Pointer to  cmdresp data.

        .globl esdhc_send_command
esdhc_send_command:
        stmfd sp!, {r4-r8, lr}
        ldr r4, =ESDHC_BASE

        @ Clear ESDHC_IRQSTAT
        mov r5, #-1
        str r5, [r4, #ESDHC_IRQSTAT]

        @Activate SDHCI_IRQ_EN_BITS
        ldr r5, [r4, #ESDHC_IRQSTATEN]
        ldr r6, =SDHCI_IRQ_EN_BITS
        orr r5, r5, r6
        str r5, [r4, #ESDHC_IRQSTATEN]

        @ Wait for bus to be idle
        @ (Wait for PRSSTAT{CICHB, CIDHB, DLA} to be down)
_esdhc_send_cmd_while_not_idle:
        mov r6, #PRSSTAT_CICHB
        ldr r5, [r4, #ESDHC_PRSSTAT]
        orr r6, r6, #PRSSTAT_CIDHB
        orr r6, r6, #PRSSTAT_DLA
        tst r5, r6
        bne _esdhc_send_cmd_while_not_idle

        @Prepare string XFERTYP
        mov r0, r0, lsl  #24            @Command Index
        orr r0, r0, r2, LSL #16         @ Response Type

        @Prepare data.
        cmp r3, #0
        beq _esdhc_send_cmd_end_of_prepare_data

        @ Set wml
        ldr r5, [r4, #ESDHC_BLKATTR]
        ldr r6, =0xfff
        and r5, r6, r5, lsr #2
        cmp r5, #0x80
        movge r5, #0x80
        str r5, [r4, #ESDHC_WML]

        @prepare xfertyp
        ldr r5, [r4, #ESDHC_BLKATTR]
        lsr r5, #16
        cmp r5, #1
        ldreq r5, =XFERTYP_READ_BITS
        ldrne r5, =XFERTYP_MULTIPLE_READ_BITS
        orr r0, r0, r5
_esdhc_send_cmd_end_of_prepare_data:
        @ Send argument
        str r1, [r4, #ESDHC_CMDARG]

        @ Send command
        str r0, [r4, #ESDHC_XFERTYP]

        @Mask All IRQs
        mov r0, #0
        str r0, [r4, #ESDHC_IRQSIGEN]

        @Wait for command to complete.
_esdhc_send_cmd_while_no_complete:
        ldr r0, [r4, #ESDHC_IRQSTAT]
        ldr r1, =CMD_FINISHED
        tst r0, r1
        beq _esdhc_send_cmd_while_no_complete

        @ Check for errors
        tst r0, #CMD_ERR
        movne r0, #ERROR_SD_CMD_ERROR
        bne _esdhc_send_cmd_end

        tst r0, #IRQSTAT_CTOE
        movne r0, #ERROR_SD_CMD_TIMEOUT
        bne _esdhc_send_cmd_end

        @ Fill response structure
        mov r0, r2                @Response type
        mov r5, r3
        bl esdhc_fill_response
        mov r3, r5

        @ Still need to handle data.
        cmp r3, #0
        beq _esdhc_send_cmd_end_of_data_fetch
_esdhc_send_cmd_data_fetch:
        @Activate SDHCI_IRQ_EN_BITS
        ldr r5, [r4, #ESDHC_IRQSTATEN]
        ldr r6, =SDHCI_IRQ_EN_BITS
        orr r5, r5, r6
        str r5, [r4, #ESDHC_IRQSTATEN]

        mov r5, #0
        ldr r6, [r4, #ESDHC_BLKATTR]
        lsr r6, #16
_esdhc_send_cmd_FOR_blk_count:
        cmp r5, r6
        bge _esdhc_send_cmd_end_of_FOR_blk_count

____esdhc_send_cmd_wait_brr:
        ldr r0, [r4, #ESDHC_IRQSTAT]
        tst r0, #IRQSTAT_BRR
        beq ____esdhc_send_cmd_wait_brr

        mov r7, #0
@        ldr r8, [r4, #ESDHC_BLKATTR]
@        ldr r0 , =0xFFF
@        and r8, r8, r0
        ldr r8, =512
        lsr r8, #2
____esdhc_send_cmd_for_blksize:
        cmp r7, r8
        bge ____esdhc_send_cmd_end_of_for_blksize

        ldr r2, [r4, #ESDHC_DATPORT]
        str r2, [r3], #4

        add r7, r7, #1
        b ____esdhc_send_cmd_for_blksize
____esdhc_send_cmd_end_of_for_blksize:

        @ Clear IRQSTAT
        ldr r0, [r4, #ESDHC_IRQSTAT]
        orr r0, r0, #IRQSTAT_BRR
        str r0, [r4, #ESDHC_IRQSTAT]

        add r5, r5, #1
        b _esdhc_send_cmd_FOR_blk_count
_esdhc_send_cmd_end_of_FOR_blk_count:
_esdhc_send_cmd_end_of_data_fetch:
       @ Clear ESDHC_IRQSTAT and terminates.
        mov r5, #-1
        str r5, [r4, #ESDHC_IRQSTAT]

        mov r0, #0
_esdhc_send_cmd_end:
        ldmfd sp!, {r4-r8, pc}

_STRING_ERROR_NO_SD_CARD_:
        .asciz "ERROR: MMC: No SD card found\n"
_STRING_MMC_INIT_:
        .asciz "MMC Initialized.\n"

