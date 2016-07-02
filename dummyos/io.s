@ -----[ io.s ]----------------------------------------------------------------@
@                                                                              @
@         i.MX53 SoC UART based IO library                                     @
@                                                                              @
@         Author: Rafael Auler, 09/16/2011                                     @
@                                                                              @
@ -----------------------------------------------------------------------------@

@ --[ Includes ]---------------------------------------------------------------@
@
	.include "uart_defs.inc"

@ --[ Global data ]------------------------------------------------------------@
@
	.data
read_buffer:	
	.fill 1024, 1, 0  @ 1024 bytes zero-initialized
read_cnt:
	.word 0
tx_ready:
	.word 1

@ --[ Internal functions ]-----------------------------------------------------@
@
	.text
busy_wait_for_tx:
	@ Check if we are already ready
	ldr r0, _bw_uart_base_ptr
	ldr r0, [r0, #UART_USR1]
	and r0, r0, #(1 << 13)
	teq r0, #0
	bne _bw_end
	ldr r2, _bw_tx_ready_ptr
	str r0, [r2]
_bw_loop:	
	ldr r0, [r2]
	teq r0, #0
	beq _bw_loop
_bw_end:	
	mov pc, lr
@ data references
_bw_uart_base_ptr:
	.word UART_BASE
_bw_tx_ready_ptr:
	.word tx_ready

@ --[ Exported functions ]-----------------------------------------------------@
@
	.globl write
@
@ Expects:
@  'r0' buffer pointer
@  'r1' number of chars to write
write:
	@ Prologue
	stmfd sp!, {r4-r6, lr}
	mov r4, r0
	mov r5, r1
	mov r6, #0
	@ while there are chars to write
_write_loop:
	@ test condition
	teq r5, #0
	beq _write_loop_exit
	@ loop body
	bl busy_wait_for_tx 		@ wait until we are cleared to transmit
	eor r0, r0, r0
	ldrb r0, [r5, r6]
	ldr r1, _wr_uart_base_ptr
	str r0, [r1, #UART_UTXD]	
	@ increment induction variables
	add r6, r6, #1
	sub r5, r5, #1
	b _write_loop
_write_loop_exit:
	@ Epilogue
	ldmfd sp!, {r4-r6, lr}
	mov pc, lr
@ write function data references
_wr_uart_base_ptr:
	.word UART_BASE

@ --[ IRQ handling ]-----------------------------------------------------------@
@
	.globl irq_uart_receive
	.globl irq_uart_transmit
irq_uart_receive:
	mov pc, lr

irq_uart_transmit:
	ldr r0, _irq_xmit_tx_ready_ptr
	mov r1, #1
	str r1, [r0]	
	mov pc, lr
@ irq_uart_transmit function data references
_irq_xmit_tx_ready_ptr:
	.word tx_ready
	