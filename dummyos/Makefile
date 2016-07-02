arm-as=$(CROSS_COMPILE)as
arm-ld=$(CROSS_COMPILE)ld
asflags=-g
ldflags=-g --section-start=.iv=0x778005e0 -Ttext=0x77800600 -Tdata=0x77800ff0 -e 0x778005e0

all:	knrl

base.o: base.s
	$(arm-as) $(asflags) base.s -o base.o

gpt.o:	gpt.s gpt_defs.inc
	$(arm-as) $(asflags) gpt.s -o gpt.o

irq.o:	irq.s tzic_defs.inc
	$(arm-as) $(asflags) irq.s -o irq.o

syscalls.o:	syscalls.s
	$(arm-as) $(asflags) syscalls.s -o syscalls.o

uart.o: uart.s uart_defs.inc
	$(arm-as) $(asflags) uart.s -o uart.o

knrl:	base.o gpt.o irq.o syscalls.o uart.o
	$(arm-ld) $(ldflags) base.o gpt.o irq.o syscalls.o uart.o -o knrl

clean:
	-rm *.o knrl
