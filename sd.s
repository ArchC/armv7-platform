@ -----[ sd.s ]----------------------------------------------------------@
@                                                                          @
@         i.MX53 SoC SD card Controller driver                             @
@                                                                          @
@       Based on the U-boot esdhc driver code.                             @
@                                                                          @
@      By Gabriel Krisman Bertazi       01/2014                            @
@                                                                          @
@ -------------------------------------------------------------------------@
@ --[ Includes ]-----------------------------------------------------------@
@
	.include "sd_defs.inc"
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
	.global init_sd

@ init_sd
@
@ Performs default inital configuration.
init_sd:
        stmfd sp!, {lr}

        @reset card.
        bl sd_go_idle
        cmp r0, #0
        movne r0, #ERROR_SD_INIT_FAIL
        bne _end_of_init_sd

        @Voltage validation
        bl sd_send_if_cond
        cmp r0, #0
        bne _end_of_init_sd

        bl sd_send_op_cond
        cmp r0, #0
        bne _end_of_init_sd

        bl sd_get_cid
        cmp r0, #0
        bne _end_of_init_sd

        bl sd_set_relative_address
        cmp r0, #0
        bne _end_of_init_sd

        @Put card 1 in transfer state.
        ldr r0, =SD_RCA
        ldr r0, [r0]
        bl sd_select_card

        mov r0, #0
_end_of_init_sd:
        ldmfd sp!, {pc}

 @ --------------------------------------------------------------------------------
 @ Expects: 'r0' Block length
 @ returns: 'r0' Result Status 0 if success
 @
 @ Set the block length by sending a CMD16 to
 @ SD card controller.
 sd_set_block_len:
         stmfd sp!, {lr}
         mov r1, r0     @ARG
         mov r2, #0x1   @CMDTYP
         mov r3, #0x0   @RSPTYP
 __setblklen_wait_cihb:
         mov r0, #16   @CMD_16 => SET_BLKLEN
         bl esdhc_send_command
         cmp r0, #0
         bne __setblklen_wait_cihb

 __setblklen_end:
         @@ Need to verify if received correct input!
         mov r0, #0      @return code
         ldmfd sp!, {pc}



@ --------------------------------------------------------------------------------
@ sd_go_idle
@
@
@ Send IDLE command to MMC.
@
@       On success, returns 0. On faiure, returns error code.
@
sd_go_idle:
        stmfd sp!, {lr}
        mov r0, #MMC_CMD_GO_IDLE_STATE
        mov r1, #0x0
        mov r2, #MMC_RSP_NONE
        mov r3, #0x0
        bl esdhc_send_command
        ldmfd sp!, {pc}

@ --------------------------------------------------------------------------------
@ Set relative address
@
@       On success, returns 0. On failure, returns
@
sd_set_relative_address:
        stmfd sp!, {lr}
        mov r0, #SD_CMD_SEND_RELATIVE_ADDR
        mov r1, #0x0
        mov r2, #MMC_RSP_R6
        mov r3, #0x0
        bl esdhc_send_command

        @ Verify return.
        mov r0, #0
        bl sd_get_response

        @Save card rca.
        mov r0, r0, lsr #16
        ldr r2, =0xFFFF
        and r0, r0, r2
        ldr r1, =SD_RCA
        str r0, [r1]

        mov r0, #0
        ldmfd sp!, {pc}
@ --------------------------------------------------------------------------------
@ Send select card
@
@ Arguments
@       R0 - Card RCA
@
@       On success, returns 0.
@
sd_select_card:
        stmfd sp!, {lr}
        mov r1, r0  @ rca argument
        mov r0, #MMC_CMD_SELECT_CARD
        mov r1, r1, lsl #16
        mov r2, #MMC_RSP_R1
        mov r3, #0x0
        bl esdhc_send_command

        mov r0, #0
        ldmfd sp!, {pc}

@ --------------------------------------------------------------------------------
@ Send If Cond
@
@
@ Send If Cond vor voltage validation.
@
@       On success, returns 0. On failure, returns SD_ERR_SEND_IF_COND_FAIL
@
sd_send_if_cond:
        stmfd sp!, {lr}
        mov r0, #SD_CMD_SEND_IF_COND
        mov r1, #0x100
        orr r1, r1, #0xaa         @ Verification bits
        mov r2, #MMC_RSP_R7
        mov r3, #0x0
        bl esdhc_send_command

        @ Verify return.
        mov r0, #0x0
        bl sd_get_response
        and r0, r0, #0xFF
        cmp r0, #0xaa             @ Check verification bits.
        movne r0, #ERROR_SD_SEND_IF_COND_FAIL
        moveq r0, #0
        ldmfd sp!, {pc}

@ --------------------------------------------------------------------------------
@ Send op Cond
@
@
@ Get op condition
@
@       On success, returns 0. On failure, returns SD_ERR_SEND_OP_COND_FAIL
@
sd_send_op_cond:
        stmfd sp!, {lr}
_do_while_op_cond_wait:
        @ Issue APP_CMD
        mov r0, #MMC_CMD_APP_CMD
        mov r1, #0x0
        mov r2, #MMC_RSP_R1
        mov r3, #0x0
        bl esdhc_send_command

        @ Issue APP_SEND_OP_COND
        mov r0, #SD_CMD_APP_SEND_OP_COND
        mov r1, #0x0
        mov r2, #MMC_RSP_R3
        mov r3, #0x0
        bl esdhc_send_command

        bl sd_get_response
        tst r0, #OCR_BUSY
        beq _do_while_op_cond_wait

        mov r0, #0
        ldmfd sp!, {pc}

@ --------------------------------------------------------------------------------
@ Recover CID
@
@       On success, returns 0. On failure, returns SD_ERR_SEND_OP_COND_FAIL
sd_get_cid:
        .warning "Fails to get CID."
        stmfd sp!, {r4, lr}
        @ Identification
        mov r0, #MMC_CMD_ALL_SEND_CID
        mov r1, #0x0
        mov r2, #MMC_RSP_R2
        mov r3, #0x0
        bl esdhc_send_command
        cmp r0, #0
        movne r0, #ERROR_SD_INIT_FAIL
        bne _end_of_init_sd

        bl esdhc_get_response
        ldmia r0, {r1-r4}
        ldr r0, =SD_CID @Target address.
        stmia r0, {r1-r4}

        mov r0, #0
        ldmfd sp!, {r4, pc}

@ --------------------------------------------------------------------------------
@ Get Response
@
@
@ Retrieve response of a command
@
@ Argument
@
sd_get_response:
        stmfd sp!, {r5, lr}
        mov r5, r0, lsl #2
        bl esdhc_get_response
        ldr r0, [r0, r5]
        ldmfd sp!, {r5, pc}

@ --------------------------------------------------------------------------------
@ sd_read
@
@ Argument
@   r0 - destination Address
@   r1 - SD Start block
@   r2 - Number of blocks
        .globl sd_read
sd_read:
        stmfd sp!, {r4-r8, lr}

        @ Backup register.
        mov r4, r0
        mov r5, r1
        mov r6, r2

        @ Setup block count and block size for controller.
        mov r0, r6
        ldr r1, =READ_BLOCK_LEN
        bl esdhc_set_block_attr

        @ Setup block size for sd card.
        ldr r0, =READ_BLOCK_LEN
        bl sd_set_block_len

        @Start Read

        @Decide whether multiblock or singleblock
        cmp r6, #1
        moveq r0, #MMC_CMD_READ_SINGLE_BLOCK
        movne r0, #MMC_CMD_READ_MULTIPLE_BLOCK

        mov r1, r5
        mov r2, #MMC_RSP_R1
        mov r3, r4
        bl esdhc_send_command

        ldmfd sp!, {r4-r8, pc}

