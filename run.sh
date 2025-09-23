#!/bin/bash

set -e

# Path to the kernel image
KERNEL=kernel8.img
DISK_IMG=fat32.img

# Check if kernel image exists
if [ ! -f "$KERNEL" ]; then
    echo "Kernel image '$KERNEL' not found. Build it first with 'make'."
    exit 1
fi

# Create and format a 10MB FAT32 disk image if it doesn't exist
if [ ! -f "$DISK_IMG" ]; then
    echo "Creating 10MB FAT32 disk image: $DISK_IMG"
    dd if=/dev/zero of=$DISK_IMG bs=1M count=10
    mkfs.vfat $DISK_IMG
fi

# Launch QEMU
qemu-system-aarch64 \
    -M raspi3b \
    -kernel $KERNEL \
    -serial stdio \
    -drive file=$DISK_IMG,format=raw,if=sd \
    -nographic

echo "QEMU has exited."
