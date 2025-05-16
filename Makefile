.PHONY: all clean run iso

all: bootloader kernel iso

bootloader:
	$(MAKE) -C boot

kernel:
	$(MAKE) -C kernel

iso:
	mkdir -p tmp/iso/boot/grub
	cp boot/bootloader.bin tmp/iso/boot/
	cp kernel/kernel.bin tmp/iso/boot/
	cp tools/Grub.cfg tmp/iso/boot/grub/grub.cfg
	grub-mkrescue -o RexOS.iso tmp/iso

run: all
	qemu-system-x86_64 -cdrom RexOS.iso

clean:
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	rm -rf tmp RexOS.iso