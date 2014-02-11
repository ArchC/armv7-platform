@ -----[ syscalls.s ]----------------------------------------------------------@
@                                                                              @
@         DummyOS syscall entry point & dispatch logic                         @
@                                                                              @
@         Author: Rafael Auler, 09/18/2011                                     @
@                                                                              @
@ -----------------------------------------------------------------------------@

@ --[ Includes ]---------------------------------------------------------------@
@
	
@ --[ Initialization code ]----------------------------------------------------@
@

@ --[ Exception handler code ]-------------------------------------------------@
@
@ The SVC handler is the entry point for syscalls. According to the new
@ ARM EABI, we should expect syscall imm number 0x0 and look into r7 for the
@ specific syscall being requested.
	.globl svc_handler
svc_handler:
	@ Saving context
	stmfd sp!, {lr}
	mrs r14, SPSR
	stmfd sp!, {r14}
	msr CPSR_c, #0x1F     @ Enter system mode, IRQ/FIQ enabled
	@ Syscall dispatch logic
	cmp r7, #128
	blge unimplemented_syscall
	stmfd sp!, {r0-r1}
	adr r0, syscall_vector
	mov r1, r7, lsl #2
	add r7, r0, r1
	ldmfd sp!, {r0-r1}
	blx r7
	@ Restoring context
	msr CPSR_c, #0xD3     @ Enter supervisor mode, IRQ/FIQ disabled
	ldmfd sp!, {r14}
	msr SPSR_cf, r14
	ldmfd sp!, {PC}^

@ --[ Internal functions ]-----------------------------------------------------@
@
unimplemented_syscall:
	stmfd sp!, {lr}
	adr r0, _unmessage
	mov r1, #61
	bl write
_panic:	b _panic
_unmessage:
	.asciz "DummyOS error: unimplemented syscall. Entering panic mode :)\n"


@ --[ Syscall dispatch table ]-------------------------------------------------@
@
	.align 4
syscall_vector:
	b	sys0
	b	sys1
	b	sys2
	b	sys3
	b	sys4
	b	sys5
	b	sys6
	b	sys7
	b	sys8
	b	sys9
	b	sys10
	b	sys11
	b	sys12
	b	sys13
	b	sys14
	b	sys15
	b	sys16
	b	sys17
	b	sys18
	b	sys19
	b	sys20
	b	sys21
	b	sys22
	b	sys23
	b	sys24
	b	sys25
	b	sys26
	b	sys27
	b	sys28
	b	sys29
	b	sys30
	b	sys31
	b	sys32
	b	sys33
	b	sys34
	b	sys35
	b	sys36
	b	sys37
	b	sys38
	b	sys39
	b	sys40
	b	sys41
	b	sys42
	b	sys43
	b	sys44
	b	sys45
	b	sys46
	b	sys47
	b	sys48
	b	sys49
	b	sys50
	b	sys51
	b	sys52
	b	sys53
	b	sys54
	b	sys55
	b	sys56
	b	sys57
	b	sys58
	b	sys59
	b	sys60
	b	sys61
	b	sys62
	b	sys63
	b	sys64
	b	sys65
	b	sys66
	b	sys67
	b	sys68
	b	sys69
	b	sys70
	b	sys71
	b	sys72
	b	sys73
	b	sys74
	b	sys75
	b	sys76
	b	sys77
	b	sys78
	b	sys79
	b	sys80
	b	sys81
	b	sys82
	b	sys83
	b	sys84
	b	sys85
	b	sys86
	b	sys87
	b	sys88
	b	sys89
	b	sys90
	b	sys91
	b	sys92
	b	sys93
	b	sys94
	b	sys95
	b	sys96
	b	sys97
	b	sys98
	b	sys99
	b	sys100
	b	sys101
	b	sys102
	b	sys103
	b	sys104
	b	sys105
	b	sys106
	b	sys107
	b	sys108
	b	sys109
	b	sys110
	b	sys111
	b	sys112
	b	sys113
	b	sys114
	b	sys115
	b	sys116
	b	sys117
	b	sys118
	b	sys119
	b	sys120
	b	sys121
	b	sys122
	b	sys123
	b	sys124
	b	sys125
	b	sys126
	b	sys127

@ --[ Specific syscalls implementations ]--------------------------------------@
@

sys1:
exit_sys_entry:
	b	sys1

sys3:
read_sys_entry:
	mov	r0, r1
	mov	r1, r2
	b	read
	
sys4:
write_sys_entry:
	mov	r0, r1
	mov	r1, r2
	b	write

@ --[ Unimplemented syscalls ]-------------------------------------------------@
@

sys0:
@ sys1 implemented as exit
sys2:
@ sys3 implemented as read
@ sys4 implemented as write
sys5:
sys6:
sys7:
sys8:
sys9:
sys10:
sys11:
sys12:
sys13:
sys14:
sys15:
sys16:
sys17:
sys18:
sys19:
sys20:
sys21:
sys22:
sys23:
sys24:
sys25:
sys26:
sys27:
sys28:
sys29:
sys30:
sys31:
sys32:
sys33:
sys34:
sys35:
sys36:
sys37:
sys38:
sys39:
sys40:
sys41:
sys42:
sys43:
sys44:
sys45:
sys46:
sys47:
sys48:
sys49:
sys50:
sys51:
sys52:
sys53:
sys54:
sys55:
sys56:
sys57:
sys58:
sys59:
sys60:
sys61:
sys62:
sys63:
sys64:
sys65:
sys66:
sys67:
sys68:
sys69:
sys70:
sys71:
sys72:
sys73:
sys74:
sys75:
sys76:
sys77:
sys78:
sys79:
sys80:
sys81:
sys82:
sys83:
sys84:
sys85:
sys86:
sys87:
sys88:
sys89:
sys90:
sys91:
sys92:
sys93:
sys94:
sys95:
sys96:
sys97:
sys98:
sys99:
sys100:
sys101:
sys102:
sys103:
sys104:
sys105:
sys106:
sys107:
sys108:
sys109:
sys110:
sys111:
sys112:
sys113:
sys114:
sys115:
sys116:
sys117:
sys118:
sys119:
sys120:
sys121:
sys122:
sys123:
sys124:
sys125:
sys126:
sys127:
	b unimplemented_syscall
	