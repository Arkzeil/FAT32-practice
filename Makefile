# Toolchain definition
CROSS_PLATFORM = aarch64-linux-gnu
CC = $(CROSS_PLATFORM)-gcc
# This should not be necessary, as GCC can handle assembly files directly
AS = ${CROSS_PLATFORM}-as
LD = ${CROSS_PLATFORM}-ld
OBJCOPY = ${CROSS_PLATFORM}-objcopy

#Directories (remember to manually create the build directory)
BUILD_DIR = ./build
SRC_DIR = ./src
INCLUDE_DIR = ./include

# Project files
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
SRCS = $(wildcard $(SRC_DIR)/*.c)
ASMS = $(wildcard $(SRC_DIR)/*.S)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(filter %.c, $(SRCS)))
ASM_OBJS = $(patsubst $(SRC_DIR)/%.S, $(BUILD_DIR)/%.o, $(filter %.S, $(ASMS)))
KERNEL = kernel8.img
ELF = kernel.elf
LINKER = linker.ld

# Compiler and Linker flags
CFLAGS = -Wall -ffreestanding -nostdinc -nostdlib -Iinclude -g
# ASFLAGS =
LDFLAGS = -nostdlib -g

.PHONY: all clean run

all: $(KERNEL)

$(KERNEL): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(KERNEL)

$(ELF): $(OBJS) $(ASM_OBJS)
	$(LD) $(LDFLAGS) -T $(LINKER) -o $(ELF) $(OBJS) $(ASM_OBJS)

$(OBJS): $(SRCS) 
	$(CC) $(CFLAGS) -c $< -o $@

# GCC can handle assembly files directly
$(ASM_OBJS): $(ASMS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(ELF) $(KERNEL) fat32.img

run: all
	@bash run.sh
