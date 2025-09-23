# Toolchain definition
CC = aarch64-none-elf-gcc
AS = aarch64-none-elf-as
LD = aarch64-none-elf-ld
OBJCOPY = aarch64-none-elf-objcopy

# Project files
SRCS = src/boot.S src/main.c
OBJS = $(SRCS:.S=.o)
OBJS := $(OBJS:.c=.o)
KERNEL = kernel8.img
ELF = kernel.elf

# Compiler and Linker flags
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -Iinclude
ASFLAGS =
LDFLAGS = -T linker.ld -nostdlib

.PHONY: all clean run

all: $(KERNEL)

$(KERNEL): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(KERNEL)

$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $(ELF) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(ELF) $(KERNEL) fat32.img

run: all
	@bash run.sh
