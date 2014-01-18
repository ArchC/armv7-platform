@ -----[ uart.s ]----------------------------------------------------------@
@                                                                          @
@         i.MX53 SoC UART based IO library                                 @
@                                                                          @
@ -------------------------------------------------------------------------@
@ --[ Includes ]-----------------------------------------------------------@
@
	.include "uart_defs.inc"

@ --[ Global data ]--------------------------------------------------------@
@
	.data
	.set CHAR_EOF, 0x03
	.set CHAR_NEW_LINE, 0x0a
	.align 4


@ --[ Module initialization ]----------------------------------------------@
@
@   Executed at boot time
	.text
	.align 4
	.globl configure_uart
@ Configures UART
configure_uart:

	@ do something
	mov r0, #0x01
	ldr r1, =UART_BASE

	str r0,[r1,#UART_UCR1]

	ldr r0, =0x2127
	str r0, [r1,#UART_UCR2]

	ldr r0, =0x0704
	str r0, [r1, #UART_UCR3]

	ldr r0, =0x7C00
	str r0, [r1, #UART_UCR4]

	ldr r0, =0x089E
	str r0, [r1,#UART_UFCR]

	ldr r0, =0x08FF
	str r0, [r1,#UART_UBIR]

	ldr r0, =0x0C34
	str r0, [r1,#UART_UBRM]
	mov pc, lr


@ --------------------------------------------------------------------------------
@ Expects:
@  'r0' buffer pointer
@  'r1' number of chars to read
@ --------------------------------------------------------------------------------
.globl read_handler
read_handler:
	stmfd sp!,{r4-r12,lr}
	@espera um input
	cmp r1,#0
	beq _return_read_handler
	mov r4,#0     @contador de caracteres lidos
	ldr r2, =UART_BASE
_wait_for_char_loop:
	ldr r3,[r2,#UART_USR2]
	tst r3,#1
	beq _wait_for_char_loop
	
_read_char:
	ldr r3,[r2,#UART_URXD]   @ Carrega o caracter em r3
	and r3, #0xFF            @ remove lixo

	@ armazena o char e soma
	strb r3,[r0,r4]          @ salva o caracter
	add r4,r4, #1            @ itera o contador
	
	cmp r3,#CHAR_NEW_LINE	 @ se for nova linha
	beq _return_read_handler @ salta para o final

	cmp r1, r4               @ verifica se atingiu o max
	bls _return_read_handler
	b _wait_for_char_loop
	
_return_read_handler:
	mov r0,r4
	ldmfd sp!,{r4-r12,pc}
@ --------------------------------------------------------------------------------
@ --------------------------------------------------------------------------------
	
@ Expects:
@  'r0' buffer pointer
@  'r1' number of chars to write
.globl write
write:
	stmfd sp!, {r4-r10,lr}
	mov r2,#0   @ contador de caracteres escritos
	ldr r3,=UART_BASE
_loop_escrita:
	cmp r2,r1
	beq _end_loop_escrita

__loop_aguarda_TRDY:
	ldr r4,[r3, #UART_USR1]
	tst r4, #0x1000   @13th bit
	bne __loop_aguarda_TRDY
__end_loop_aguarda_TRDY:
	ldrb r5,[r0]
	str r5, [r3,#UART_UTXD]
	add r0,r0,#1
	add r2,r2,#1
	b _loop_escrita
_end_loop_escrita:
	ldmfd sp!, {r4-r10,pc}

	
