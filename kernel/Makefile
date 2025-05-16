CC=x86_64-elf-gcc
AS=nasm
LD=x86_64-elf-ld
CFLAGS=-ffreestanding -O2 -Wall -Wextra -Iinclude -Ikernel/include
LDFLAGS=-T kernel.ld
ASFLAGS=-f 
FS_DIR := ../fs
# Include the filesystem Makefile
include $(FS_DIR)/Makefile
ASFLAGS+=--32
CFLAGS += -I$(FS_DIR)
LDFLAGS += -T $(FS_DIR)/fs.ld

ASM_SRCS=$(wildcard asm/*.asm)
ASM_OBJS=$(ASM_SRCS:.asm=.o)

C_SRCS=$(wildcard c/*.c)
C_OBJS=$(C_SRCS:.c=.o)
C_SRCS = $(wildcard c/*.c) $(FS_DIR)/elf_loader.c
C_OBJS = $(C_SRCS:.c=.o)

OBJS=$(ASM_OBJS) $(C_OBJS)

all: kernel.bin

%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.bin *.o asm/*.o c/*.o
	