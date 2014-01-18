CROSS_PREFIX=/home/gabriel/unicamp/ic/cross/arm-eabi-4.4.3/bin/arm-eabi-

AS=$(CROSS_PREFIX)as
LD=$(CROSS_PREFIX)ld
OBJDUMP=$(CROSS_PREFIX)objdump
OBJCOPY=$(CROSS_PREFIX)objcopy

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

elf: base.o uart.o src.o
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
