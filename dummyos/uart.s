@ -----[ uart.s ]--------------------------------------------------------------@
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
	.align 4
read_buffer:	
	.fill 1024, 1, 0  @ 1024 bytes zero-initialized
read_cnt:
	.word 0
write_buffer_ptr:
	.word 0
write_cnt:
	.word 0

@ --[ Module initialization ]--------------------------------------------------@
@
@   Executed at boot time
	.text
	.align 4
	.globl configure_uart
@ Configures UART
configure_uart:	
	ldr r1, _sys_uart_base_ptr
	@ Assert UART_ENABLE  (write to UCR1)
@	mov r0, #1
@	str r0, [r1, #UART_UCR1]
	@ Assert TXEN (transmitter enable), RXEN (receiver enable),
	@ SRST (reset), WS (8-bit data), PREN (parity), CTSC (hardware flow
	@ control) at UCR2
	mov r0, #0x27
@	orr r0, r0, #(0x21 << 8)
	orr r0, r0, #(0x40 << 8)
	str r0, [r1, #UART_UCR2]
	@ Assert RXDMUXSEL, DSR, DCD, RI at UCR3
@	mov r0, #0x04
@	orr r0, r0, #(0x07 << 8)
@	str r0, [r1, #UART_UCR3]
	@ Set CTSTL = 31 (CTS trigger level)
@	mov r0, #0x7C00
@	str r0, [r1, #UART_UCR4]
	@ uart clock is 21.6MHz
	@ Set module clock divider to /2, so internal clock is 10.8MHz
	@ and brm_clk is 0.675 MHz (16x supersampling). Set RXTL = 20, TXTL = 2
@	mov r0, #0x1E
@	orr r0, r0, #(0xA << 8)
@	str r0, [r1, #UART_UFCR]
	@ Set UBIR (numerator) (must be written first)
	@ Set UBRM (denominator) ((UBIR+1)/UBRM+1) multiplyed by brm_clk gives
	@ baud rate. In this case we wish 115.2kbps)
@	mov r0, #63
@	str r0, [r1, #UART_UBIR]
@	mov r0, #76
@	orr r0, r0, #(1 << 8)
@	str r0, [r1, #UART_UBRM]
	@ Enable TRDY and RRDY interrupts
	mov r0, #0x1
	orr r0, r0, #(0x22 << 8)
	str r0, [r1, #UART_UCR1]
	mov pc, lr
_sys_uart_base_ptr:
	.word UART_BASE

@ --[ Exported functions ]-----------------------------------------------------@
@
	.globl read
@ Expects:
@  'r0' buffer pointer
@  'r1' number of chars to read
read:	
	stmfd	sp!, {r4-r7,lr}
	ldr	r4, =UART_BASE
	mov	r6, #0 			@ bytes_lidos
_read_loop:
	cmp	r6, r1
	bge	_read_end
_read_poll:
	ldr	r5, [r4, #UART_USR2]
	and	r5, r5, #1
	cmp	r5, #1
	bne	_read_poll
	@ dado disponível
	ldr	r7, [r4, #UART_URXD]
	and	r7, r7, #0xff
	strb	r7, [r0, r6]
	add	r6, r6, #1		@ ++bytes_lidos
	b _read_loop
_read_end:
	mov	r0, r6
	ldmfd	sp!, {r4-r7,pc}
	
	.globl write
@
@ Expects:
@  'r0' buffer pointer
@  'r1' number of chars to write
write:
	stmfd	sp!, {r4-r7,lr}
	ldr	r4, =UART_BASE
	mov	r6, #0 			@ bytes_escritos
__write_loop:
	cmp	r6, r1
	bge	_read_end
_write_poll:
	ldr	r5, [r4, #UART_USR1]
	mov	r5, r5, lsr #13
	and	r5, r5, #1
	cmp	r5, #1
	bne	_write_poll
	@ all clear, go ahead
	ldrb	r7, [r0, r6]
	and	r7, r7, #0xff
	strb	r7, [r4, #UART_UTXD]
	add	r6, r6, #1		@ ++bytes_escritos
	b __write_loop
_write_end:
	mov	r0, r6
	ldmfd	sp!, {r4-r7,pc}
	

	
write2:
	@ Prologue
	stmfd sp!, {r4-r5,lr}
	mov r4, r0
	mov r5, r1
	bl busy_wait_for_write_cnt
	ldr r0, _wr_write_buffer_ptr_ptr
	str r4, [r0]
	ldr r0, _wr_write_cnt_ptr
	str r5, [r0]
	bl enable_trdy
	ldmfd sp!, {r4-r5,pc}
@ write function data references
_wr_write_buffer_ptr_ptr:
	.word write_buffer_ptr
_wr_write_cnt_ptr:
	.word write_cnt

@ --[ Internal functions ]-----------------------------------------------------@
@
busy_wait_for_write_cnt:
	ldr r0, _bw_write_cnt_ptr
_bw_w_loop:	
	ldr r1, [r0]
	teq r1, #0
	bne _bw_w_loop
	mov pc, lr
@ data references
_bw_write_cnt_ptr:
	.word write_cnt
	
check_tx:
	@ Check if we are already ready
	ldr r0, _check_uart_base_ptr
	ldr r0, [r0, #UART_USR1]
	and r0, r0, #(1 << 13)
	teq r0, #0
	bne _check_ok
	eor r0, r0, r0
	mov pc, lr
_check_ok:
	mov r0, #1
	mov pc, lr
@ data references
_check_uart_base_ptr:
	.word UART_BASE

enable_trdy:
	ldr r0, _en_trdy_uart_base_ptr
	ldr r1, [r0, #UART_UCR1]
	orr r1, r1, #(1 << 13)
	str r1, [r0, #UART_UCR1]
	mov pc, lr
@ data references
_en_trdy_uart_base_ptr:
	.word UART_BASE

disable_trdy:
	ldr r0, _dis_trdy_uart_base_ptr
	ldr r1, [r0, #UART_UCR1]
	mvn r2, #(1 << 13)
	and r1, r1, r2
	str r1, [r0, #UART_UCR1]
	mov pc, lr
@ data references
_dis_trdy_uart_base_ptr:
	.word UART_BASE

@ --[ IRQ handling ]-----------------------------------------------------------@
@
	.globl int31
int31:
	mov pc, lr
handle_uart_interrupt:
	@ Prologue
	stmfd sp!, {r4-r5,lr}
	@ Check what event generated this interrupt
	@ Check if it is RRDY
	ldr r1, _uart_uart_base_ptr
	ldr r0, [r1, #UART_USR1]
	and r0, r0, #(1 << 9)
	teq r0, #0
	beq _int31_test_TXDY
	mov r4, r0
	mov r5, r1
	bl irq_uart_receive
	@ clean this interrupt flag
	str r4, [r5, #UART_USR1]
_int31_test_TXDY:
	ldr r1, _uart_uart_base_ptr
	ldr r0, [r1, #UART_USR1]
	and r0, r0, #(1 << 13)
	teq r0, #0
	beq _int31_exit
	mov r4, r0
	mov r5, r1
	bl irq_uart_transmit
	@ clean this interrupt flag
	str r4, [r5, #UART_USR1]
_int31_exit:
	@ Epilogue
	ldmfd sp!, {r4-r5,lr}
	mov pc, lr
_uart_uart_base_ptr:
	.word UART_BASE

@ Servicing receive interrupts
irq_uart_receive:
	mov pc, lr

@ Servicing transmit interrupts
irq_uart_transmit:
	mov pc, lr

