AS=$(CROSS_COMPILE)as
LD=$(CROSS_COMPILE)ld
OBJDUMP=$(CROSS_COMPILE)objdump
OBJCOPY=$(CROSS_COMPILE)objcopy

CFLAGS= -g -O0

all: image

head:
	@echo "==========================================================="
	@echo "=============== dumboot Bootstrap v1.0 ===================="
	@echo "==========================================================="

image: head elf
	@echo "Making dumboot.elf bootable..."
	$(OBJCOPY) -O binary -j .text dumboot.elf dumboot.bin

	@echo "==========================================================="
	@echo "DONE! dumboot.bin generated. That's your bootstrap!"
	@echo "Just plug it into your ROM device simulator"
	@echo "==========================================================="

elf: base.o uart.o src.o esdhc.o sd.o
	$(LD) -g -Ttext=0x0 $+ -o dumboot.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) -g $(ASFLAGS) $< -o $@

clean:
	-rm dumboot.elf dumboot.bin *.o
dumpelf:
	$(OBJDUMP) -D dumboot.elf | less

dump:
	xxd dumboot.bin | less
