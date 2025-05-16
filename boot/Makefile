ASM=nasm
ASFLAGS=-f bin
OBJS=asm/bootloader.asm asm/bootloader_utils.asm asm/bootloader_pm.asm asm/gdt.asm

all: bootloader.bin

bootloader.bin: $(OBJS)
	$(ASM) $(ASFLAGS) -o bootloader.bin asm/bootloader.asm

clean:
	rm -f *.bin
