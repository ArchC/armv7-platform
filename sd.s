@ -----[ sd.s ]----------------------------------------------------------@
@                                                                          @
@         i.MX53 SoC SD card Controller driver                             @
@                                                                          @
@      By Gabriel Krisman Bertazi    19/12/2012                            @
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
@ Expects:
@  'r0' Destination address
@  'r1' SD card start address
@  'r2' Number of blocks to be read.globl sd_readSingleblock
@  'r3' Watermark Lvl
@
@ WARNING: Blocksize must be set prior calling this function
.globl sd_readblock
sd_readblock:
        stmfd sp!, {r0,r4-r9, lr}

        @set cmdarg
        bl wait_cihb
        ldr r4, =ESDHC_BASE
        str r1, [r4, #ESDHC_CMDARG]


        movw r8, #0x1fff
        ldr r7, [r4, #ESDHC_BLKATTR]
        and r7, r7, r8         @R5 agora tem o block size
        mul r6, r2, r7        @ r6 tem o numero de bytes

        @set BLKATTR  <- BLKSIZE must be previously set
        mov r2, r2, LSL #16   @Calculate BLKATTR
        orr r7, r7, r2
        str r7, [r4, #ESDHC_BLKATTR]
        mov r5, r6

        @Prepare XFERTYP
        mov r1, #18
        mov r1, r1, LSL #24
        ldr r2, =0x210036            @mask{DPSEL|RSTYP[0]|MSBSEL|DTDSEL|AC12EN|BCEN}
        orr r1, r1, r2

        bl wait_cihb
        str r1, [r4, #ESDHC_XFERTYP]

__readblk_wait_data_ready:
        ldr r1, [r4, #ESDHC_PRSSTAT]
        tst r1, #0x800
        beq __readblk_wait_data_ready

        mov r6, r3   @WML lvl
___read_until_wml:
        ldr r1, [r4, #ESDHC_DATPORT]
        str r1, [r0], #4

        subs r5, r5, #4
        nop                @no pipeline issues
        beq __readblk_end

        subs r6, r6, #0x4
        nop                @no pipeline issues
        bne ___read_until_wml
        b __readblk_wait_data_ready

        mov r0, #0
__readblk_end:
         ldmfd sp!, {r0, r4-r9, pc}

 @ --------------------------------------------------------------------------------
 @ Expects:
 @  'r0' buffer pointer to bytes to be written
 @  'r1' SD card start address to be written to
 @  'r1' number of chars to write

 .globl sd_writeblock
 sd_writeblock:
         mov pc, lr

 @ --------------------------------------------------------------------------------
 @ Expects: 'r0' Block length
 @ returns: 'r0' Result Status 0 if success
 @
 @ Set the block length by sending a CMD16 to
 @ SD card controller.

 .globl sd_set_blockLen
 sd_set_blockLen:
         stmfd sp!, {lr}
         mov r1, r0    @ARG
         mov r2, #00   @CMDTYP
         mov r3, #01   @RSPTYP
 __setblklen_wait_cihb:
         mov r0, #16   @CMD_16 => SET_BLKLEN
         bl sd_send_command
         cmp r0, #0
         bne __setblklen_wait_cihb

 __setblklen_end:
         @@ Need to verify if received correct input!
         mov r0, #0      @return code
         ldmfd sp!, {pc}

 @ --------------------------------------------------------------------------------
 @ Expects: 'r0' CMD INDEX
 @          'r1' ARGUMENT
 @          'r2' CMDTYPE
 @          'r3' RSPTYPE
 @
 @ returns: 'r0' Result Status 0 if success
 @
 @ Send a command to ESDHC by writing to XFERTYP register
 @
 sd_send_command:
         stmfd sp!, {r4-r9, lr}
         ldr r4, =ESDHC_BASE

         @Verify ESDHC ready for input by checking CIHB on PRSSTAT
         ldr r5, [r4, #ESDHC_PRSSTAT]
         tst r5, #0b1                    @ Verify bit 0
         movne r0, #1                    @ Failure return code
         bne _send_cmd_end               @ If cant write end function

         @Send Argument
         str r1, [r4, #ESDHC_CMDARG]     @Store argument

         @Prepare string XFERTYP
         mov r0, r0, LSL  #24
         orr r0, r0, r2, LSL #22
         orr r0, r0, r3, LSL #16
         str r0, [r4, #ESDHC_XFERTYP]

         mov r0, #0 @sucess return code
_send_cmd_end:
         ldmfd sp!, {r4-r9,pc}

 @ --------------------------------------------------------------------------------
 @ Expects: 'r0' WR_BRST_LEN
 @          'r1' WR_WML
 @          'r2' RD_BRST_LEN
 @          'r3' RD_WML
 @
 @ Set ESDHC watermarkLevel
 @
 sd_set_watermark_level:
         and r0, r0, #0b11111
         mov r0, r0, LSL #24

         and r1, r1, #0xFF
         orr r0, r0, r1, LSL #16

         and r2, r2, #0b11111
         orr r0, r0, r2, LSL #8

         and r3, r3, #0xFF
         orr r0, r0,r3

         ldr r1, =ESDHC_BASE
         str r0, [r1, #ESDHC_WML]
         mov pc, lr

 @ --------------------------------------------------------------------------------
 @ Expects: 'r0' Block Count
 @          'r1' Block Size
 @
 @ Set the block length of ESDHC writing to BlockAttr reg
 @
        .globl  esdhc_setBlockAttr
 esdhc_setBlockAttr:

         mov r0, r0, LSL #16

         movw r2, #0x1fff
         and r1, r1, r2

         orr r0, r0, r1
         ldr r2, =ESDHC_BASE
         str r0, [r2, #ESDHC_BLKATTR]
         mov pc, lr

@ --------------------------------------------------------------------------------
@ wait_cihb
@
@
@
wait_cihb:
       stmfd sp!, {r4,lr}
_cihb_not_ready:
       ldr r4, =ESDHC_BASE
       ldr r4, [r4, #ESDHC_PRSSTAT]
       tst r4, #0b1
       bne _cihb_not_ready
       ldmfd sp!, {r4,pc}

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
        mov r1, #0
        mov r2, #MMC_RSP_NONE
        mov r3, #MMC_RSP_NONE
        bl sd_send_command
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
        mov r2, #0x0
        mov r3, #MMC_RSP_R6
        bl sd_send_command

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
        mov r2, #0x0
        mov r3, #MMC_RSP_R1
        bl sd_send_command

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
        mov r2, #0x0
        mov r3, #MMC_RSP_R7
        bl sd_send_command

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
        mov r2, #0x0
        mov r3, #MMC_RSP_R1
        bl sd_send_command

        @ Issue APP_SEND_OP_COND
        mov r0, #SD_CMD_APP_SEND_OP_COND
        mov r1, #0x0
        mov r2, #0x0
        mov r3, #MMC_RSP_R3
        bl sd_send_command

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
        mov r2, #0x0
        mov r3, #MMC_RSP_R2
        bl sd_send_command
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
@ sd_load_block
@
@ Argument
@   r0 - destination address
@   r1 - Block
@   r2 - Number of blocks
        .globl sd_load_block
sd_load_block:
        stmfd sp!, {lr}

        stmfd sp!, {r0-r2}
	@SD blocklen
        mov r0, #0x4 @ Set block to 512
        bl  sd_set_blockLen
        mov r0, #0x2
        mov r1, #0x4 @ Must be same as blockLen
        bl esdhc_setBlockAttr

        @ldr self value
        ldmfd sp!, {r0-r2}
        mov r3, #1  @wml
        bl sd_readblock
        ldmfd sp!, {pc}

