@ -----[ irq.s ]---------------------------------------------------------------@
@                                                                              @
@         i.MX53 SoC interrupt vector                                          @
@                                                                              @
@         Author: Rafael Auler, 09/18/2011                                     @
@                                                                              @
@ -----------------------------------------------------------------------------@

@ --[ Includes ]---------------------------------------------------------------@
@
	.include "tzic_defs.inc"
	
@ --[ Initialization code ]----------------------------------------------------@
@
@    Executed at boot time to configure the interrupt controller
	.text
	.align 4
	.globl configure_tzic
@ Configures TZIC
configure_tzic:	
	@ R1 <= TZIC_BASE
	ldr r1, _sys_tzic_base_ptr
	@ Configure all interrupts as non-secure
	eor r2, r2, r2
	sub r0, r2, #1 @ r0 = 0xFFFFFFFF
	str r0, [r1, #TZIC_INTSEC0]
	str r0, [r1, #TZIC_INTSEC1]
	str r0, [r1, #TZIC_INTSEC2]
	str r0, [r1, #TZIC_INTSEC3]
	@ Enables interrupt 31 (UART) and 39 (GPT)
	@ reg0 bit 31 (uart)
	@ reg1 bit 7 (gpt)
	mov r0, #(1 << 31)
	str r0, [r1, #TZIC_ENSET0]
	mov r0, #(1 << 7)
	str r0, [r1, #TZIC_ENSET1]
	mov r0, #0
	str r0, [r1, #TZIC_ENSET2]
	str r0, [r1, #TZIC_ENSET3]
	@ Configure interrupt31 priority as 1
	@ reg7, byte 3
	ldr r0, [r1, #TZIC_PRIORITY7]
	bic r0, r0, #0xFF000000
	mov r2, #1
	orr r0, r0, r2, lsl #24
	str r0, [r1, #TZIC_PRIORITY7]
	@ Configure interrupt39 priority as 1
	@ reg9, byte 3
	ldr r0, [r1, #TZIC_PRIORITY9]
	bic r0, r0, #0xFF000000
	mov r2, #1
	orr r0, r0, r2, lsl #24
	str r0, [r1, #TZIC_PRIORITY9]
	@ Configure PRIOMASK as 0
	eor r0, r0, r0
	str r0, [r1, #TZIC_PRIOMASK]
	@ Enables TZIC
	mov r0, #1
	str r0, [r1, #TZIC_INTCTRL]
	mov pc, lr
_sys_tzic_base_ptr:
	.word TZIC_BASE

@ --[ Exception handler code ]-------------------------------------------------@
@
@ IRQ handler loads TZIC (the SoC interrupt controller) registers to discover
@ which are the highest priority interrupts pending. Among these, it picks
@ the lowest IRQ number to service and sets priority mask to the priority of the
@ current interrupt being serviced. In this way, no other interrupt with equal
@ or lower priority will occur while we are servicing the current interrupt.
@ Afterwards, the code access the interrupt vector to jump to the IRQ handler.
@ On return, we restore previous priority mask and finish.
@
	.globl irq_handler
irq_handler:
	sub 	lr, lr, #4	        @ remember LR_irq has PC+8
	stmfd	sp!, {lr}        	@ save adjusted LR_IRQ
	mrs    	r14, SPSR
	stmfd  	sp!, {r5-r9, r14}  	@ save workreg & SPSR_IRQ
        @ discover which IRQ we are handling
	ldr r9, _irq_tzic_base_ptr
	ldr r5, [r9, #TZIC_HIPND0] @ load bit vectors for the highest priority
	ldr r6, [r9, #TZIC_HIPND1] @ pending interupts
	ldr r7, [r9, #TZIC_HIPND2]
	ldr r8, [r9, #TZIC_HIPND3]
	@ now test to discover the first pending interrupt (lowest number)	
	stmfd sp!, {r9}            @ spill our base address reg, won't be used
	mov r9, #0                 @ r9 will be used to hold an offset
	teq r5, #0                 
	bne _irq_found             @ r5 is not 0, then it must have an interrupt
	mov r5, r6
	add r9, r9, #32
	teq r5, #0
	bne _irq_found
	mov r5, r7
	add r9, r9, #32
	teq r5, #0
	bne _irq_found
	mov r5, r8
	add r9, r9, #32
	teq r5, #0
	bne _irq_found
	@ if we reached here, no interrupts are pending - weird. TZIC
	@ must be fooling us. just quit.
	b _irq_quit	
_irq_found:
	@ now test to discover the exact bit location of the interrupt, inside
	@ r5 register	
	teq r5, #1               @ test if it is the current bit (rightmost)
	beq _irq_bit_found
	add r9, r9, #1
	mov r5, r5, lsr #1
	b _irq_found	
_irq_bit_found:
	mov r5, r9               @ now r5 has the IRQ number
	ldmfd	sp!, {r9}        @ restore our base address spill
	@ now we need to set PRIOMASK equal to the current serviced interrupt
	@ discover this interrupt (stored in r5) priority
	add r6, r9, #TZIC_PRIORITY0
	ldrb r6, [r6, r5]
	ldr r7, [r9, #TZIC_PRIOMASK]
	stmfd	sp!, {r7}        @ save the previous PRIOMASK value
	str r6, [r9, #TZIC_PRIOMASK]  @ change PRIOMASK
	@ -- now tzic will ignore interrupts with priority equal or lower
	@    than the current serviced interrupt priority --	
	msr     CPSR_c, #0x1F    @ go to System mode, IRQ & FIQ enabled
        stmfd   sp!, {r0-r3, lr} @ save LR_USR and non-callee saved registers
	@ Calculate the address of the interrupt handler
	adr     r6, interrupt_vector
	mov	r5, r5, lsl #2
	add	r6, r6, r5	
	blx     r6               @ Jump to the interrupt handler
	ldmfd  	sp!, {r0-r3, lr} @ restore
	msr     CPSR_c, #0x92    @ go back to IRQ mode, disable IRQ
				 @	(FIQ still enabled)
	ldmfd   sp!, {r7} 	 @ restore previous PRIOMASK value
	ldr 	r5, _irq_tzic_base_ptr
	str	r7, [r5, #TZIC_PRIOMASK]
_irq_quit:	
	ldmfd  	sp!, {r5-r9, r14}  @ restore workreg & SPSR_IRQ
	msr     SPSR_cf, r14
	ldmfd   sp!, {PC}^       @ and return
@ data used by interrupt handler
_irq_tzic_base_ptr:
	.word TZIC_BASE

	
  .globl interrupt_vector

@ --[ Interrupt vector  ]------------------------------------------------------@
@
@    This is the interrupt vector. Should range from 0 to 127 branches.
@    We define int0 to int127 weak symbols and in each vector position we
@    jump to the corresponding symbol address. Remember that when a weak
@    symbol is not resolved by the linker, it is just replaced by 0 without
@    generating a linker error. This means that if no function handling some
@    IRQ number is presented, we will jump to 0 if this interrupt occurs.
@    Jump to 0 is equal to resetting the machine, since we are jumping to the
@    reset position in the exception vector.
@
  .weak int0, int1, int2, int3, int4, int5, int6, int7, int8, int9
  .weak int10, int11, int12, int13, int14, int15, int16, int17, int18, int19
  .weak int20, int21, int22, int23, int24, int25, int26, int27, int28, int29
  .weak int30, int31, int32, int33, int34, int35, int36, int37, int38, int39
  .weak int40, int41, int42, int43, int44, int45, int46, int47, int48, int49
  .weak int50, int51, int52, int53, int54, int55, int56, int57, int58, int59
  .weak int60, int61, int62, int63, int64, int65, int66, int67, int68, int69
  .weak int70, int71, int72, int73, int74, int75, int76, int77, int78, int79
  .weak int80, int81, int82, int83, int84, int85, int86, int87, int88, int89
  .weak int90, int91, int92, int93, int94, int95, int96, int97, int98, int99
  .weak int100, int101, int102, int103, int104, int105, int106, int107, int108
  .weak int109, int110, int111, int112, int113, int114, int115, int116, int117
  .weak int118, int119, int120, int121, int122, int123, int124, int125, int126
  .weak int127

interrupt_vector:
	b	int0
	b	int1
	b	int2
	b	int3
	b	int4
	b	int5
	b	int6
	b	int7
	b	int8
	b	int9
	b	int10
	b	int11
	b	int12
	b	int13
	b	int14
	b	int15
	b	int16
	b	int17
	b	int18
	b	int19
	b	int20
	b	int21
	b	int22
	b	int23
	b	int24
	b	int25
	b	int26
	b	int27
	b	int28
	b	int29
	b	int30
	b	int31
	b	int32
	b	int33
	b	int34
	b	int35
	b	int36
	b	int37
	b	int38
	b	int39
	b	int40
	b	int41
	b	int42
	b	int43
	b	int44
	b	int45
	b	int46
	b	int47
	b	int48
	b	int49
	b	int50
	b	int51
	b	int52
	b	int53
	b	int54
	b	int55
	b	int56
	b	int57
	b	int58
	b	int59
	b	int60
	b	int61
	b	int62
	b	int63
	b	int64
	b	int65
	b	int66
	b	int67
	b	int68
	b	int69
	b	int70
	b	int71
	b	int72
	b	int73
	b	int74
	b	int75
	b	int76
	b	int77
	b	int78
	b	int79
	b	int80
	b	int81
	b	int82
	b	int83
	b	int84
	b	int85
	b	int86
	b	int87
	b	int88
	b	int89
	b	int90
	b	int91
	b	int92
	b	int93
	b	int94
	b	int95
	b	int96
	b	int97
	b	int98
	b	int99
	b	int100
	b	int101
	b	int102
	b	int103
	b	int104
	b	int105
	b	int106
	b	int107
	b	int108
	b	int109
	b	int110
	b	int111
	b	int112
	b	int113
	b	int114
	b	int115
	b	int116
	b	int117
	b	int118
	b	int119
	b	int120
	b	int121
	b	int122
	b	int123
	b	int124
	b	int125
	b	int126
	b	int127

