@ -----[ base.s ]--------------------------------------------------------------@
@                                                                              @
@         i.MX53 SoC simple system level initialization                        @
@                                                                              @
@         Author: Rafael Auler, 09/16/2011                                     @
@                                                                              @
@ -----------------------------------------------------------------------------@

@ Configurable STACK values for each ARM core operation mode
	
	.set SVC_STACK, 0x77701000
	.set UND_STACK, 0x77702000
	.set ABT_STACK, 0x77703000
	.set IRQ_STACK, 0x77704000
	.set FIQ_STACK, 0x77705000
	.set USR_STACK, 0x77706000

@ --[ Exception vector  ]------------------------------------------------------@
@
@   Note this is different from the interrupt vector. This is the entry point
@   used by ARM core to switch between different operation modes.
@
@	.org 0x0                 @ -> exception vector begins at address zero
        .section .iv,"a"
_start:				 @ This symbol is not necessary, just put it
	                         @ to stop nagging of the user-level app linker
interrupt_vector:
	b reset_handler		 @  0x0
	b undefined_i_handler    @  0x4
	b svc_handler		 @  0x8
	b prefetch_abort_handler @  0xC
	b data_abort_handler	 @  0x10
	nop			 @  0x14
	b irq_handler		 @  0x18
	b fiq_handler		 @  0x1C

@ --[ Exception handlers code ]------------------------------------------------@
@
@   The exception vector is a jump table. Here we put the code for the actual
@   handlers
@
@	.org 0x100            	@ -> we can put this anywhere we like
				@    as long as it is inside the addressable
				@    memory range!
	.text

@ Reset handler initialize all stacks and jump to system_init in
@ system mode.
@
reset_handler:
	@ First configure stacks for all modes
	ldr sp, =SVC_STACK
	msr CPSR_c, #0xDF	@ Enter system mode, FIQ/IRQ disabled
	ldr sp, =USR_STACK
	msr CPSR_c, #0xD1       @ Enter FIQ mode, FIQ/IRQ disabled
	ldr sp, =FIQ_STACK
	msr CPSR_c, #0xD2       @ Enter IRQ mode, FIQ/IRQ disabled
	ldr sp, =IRQ_STACK
	msr CPSR_c, #0xD7       @ Enter abort mode, FIQ/IRQ disabled
	ldr sp, =ABT_STACK
	msr CPSR_c, #0xDB       @ Enter undefined mode, FIQ/IRQ disabled
	ldr sp, =UND_STACK

        @Set interrupt table base address on coprocessor 15.
        ldr r0, =interrupt_vector
        mcr p15, 0, r0, c12, c0, 0

        msr CPSR_c, #0x1F       @ Enter system mode, IRQ/FIQ enabled
	@ Now transfer to system_init
	b system_init

@
@ Unimplemented stuff
@
@ (remember svc_handler and irq_handler are implemented in syscalls.s and
@  irq.s)
prefetch_abort_handler:
data_abort_handler:
undefined_i_handler:
fiq_handler:
	movs	pc, lr

@ --[ System initialization code ]---------------------------------------------@
@
@   Here the symbol system_init is defined, with code to start up all necessary
@   IPs in the SoC. Afterwards, it jumps to user-level code.
@

@
@ System initialization
@
system_init:
	@ note there is no need to save LR here, since we don't have to
	@ return
	bl configure_tzic
	bl configure_gpt
	bl configure_uart
	bl print_boot_message
	msr CPSR_c, #0x10        @ Enter user mode, IRQ/FIQ enabled

        ldr r0, =0x77802000
        blx r0
loop:
	@ finished application. nothing to do, so just loop endlessly
	b loop

print_boot_message:
	stmfd sp!, {lr}
	adr r0, _bootmessage
	mov r1, #16
	bl write
	ldmfd sp!, {pc}
_bootmessage:
	.asciz "Booted DummyOS.\n "

