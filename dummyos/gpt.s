@ -----[ gpt.s ]---------------------------------------------------------------@
@                                                                              @
@         i.MX53 SoC General Purpose Timer driver                              @
@                                                                              @
@         Author: Rafael Auler, 09/18/2011                                     @
@                                                                              @
@ -----------------------------------------------------------------------------@

@ --[ Includes ]---------------------------------------------------------------@
@
	.include "gpt_defs.inc"

@ --[ Initialization code ]----------------------------------------------------@
@
@   Here we put the code for the GPT initialization at system boot time
@
	.text
	.align 4
	.globl configure_gpt
@ Configures GPT
configure_gpt:
	@ R1 <= GPT_BASE
	ldr r1, _sys_gpt_base_ptr
	@ Enables GPT with clock_src = 001 (peripheral clock)
@	mov r0, #0x041
	str r0, [r1, #GPT_CR]
	@ Set prescaler to 0
	eor r0, r0, r0
	str r0, [r1, #GPT_PR]
	@ Configure OCR1 (compare register 1 - will trigger an event
	@ when counter reaches this value)
	mov r0, #(1 << 8)
	str r0, [r1, #GPT_OCR1]
	@ Set GPT to trigger interrupt when the counter reaches this value.
	mov r0, #1
	str r0, [r1, #GPT_IR]
	mov pc, lr
_sys_gpt_base_ptr:
	.word GPT_BASE


@ --[ Interrupt handlers code ]------------------------------------------------@
@
@   Here we put the code for the actual interrupt handlers
@
	.globl int39
	.weak irq_gpt
int39:
handle_gpt_interrupt:
	stmfd sp!, {lr}
	@ Tells GPT this interrupt is already being serviced
	@ R1 <= GPT_BASE
	ldr r1, _gpt_gpt_base_ptr	
	eor r0, r0, r0
	sub r0, r0, #1
	str r0, [r1, #GPT_SR]
	ldr r0, _gpt_handler_ptr
	teq r0, #0
	beq _gpt_handle_exit
	blx r0
_gpt_handle_exit:	
	@ Return
	ldmfd sp!, {pc}
_gpt_gpt_base_ptr:
	.word GPT_BASE
_gpt_handler_ptr:
	.word irq_gpt
	