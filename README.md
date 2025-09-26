# FAT32 + USB Firmware Practice

This project provides a bare-metal environment for practicing FAT32 filesystem and USB driver implementation on a QEMU-emulated Raspberry Pi 3 (AArch64).

Your task is to implement a simple USB Mass Storage driver and then use it to read from a FAT32-formatted disk image.

## Prerequisites

- `qemu-system-aarch64`
- `aarch64-linux-gnu` toolchain
- `make`
- `mkfs.vfat` (usually available in `dosfstools` or a similar package)

## Structure

- `src/boot.S`: Minimal assembly bootloader. Sets up the stack and jumps to `main`.
- `src/main.c`: The main kernel entry point. Calls the USB and FAT32 initialization routines.
- `src/uart.c`: Implements basic UART (serial) communication for console output.
- `src/usb.c`: **Your workspace.** A stub file for you to implement the USB Mass Storage driver. Contains `TODO` comments to guide you.
- `include/usb.h`: Header file for your USB driver. Contains function prototypes and `TODO` comments.
- `linker.ld`: Linker script to place the kernel at the correct memory address (`0x80000`).
- `Makefile`: Automates the build process. It automatically finds and compiles all `.c` and `.S` files in the `src` directory.
- `run.sh`: Creates a 10MB FAT32 disk image (`fat32.img`) and launches QEMU.

## How to Use

1.  **Build the kernel:**
    ```bash
    make
    ```
    This compiles all source files and links them into `kernel8.img`.

2.  **Run the emulator:**
    ```bash
    make run
    ```
    This command executes the `run.sh` script, which will:
    - Create a `fat32.img` file if it doesn't exist.
    - Format it as FAT32.
    - Launch QEMU with `fat32.img` attached as a USB storage device.

## Your Task

Your goal is to bring the system to a state where it can read from the USB-attached disk image.

1.  **Implement the USB Driver:**
    - Open `src/usb.c` and `include/usb.h`.
    - Follow the `TODO` comments to implement the `usb_init()` and `read_block()` functions.
    - You will need to interact with the Raspberry Pi's USB controller (DWC2).
    - A highly recommended reference is the [USPi library](https://github.com/rsta2/uspi), which is a C-based bare-metal USB driver for the Raspberry Pi.

2.  **Implement the FAT32 Reader:**
    - Open `src/main.c`.
    - In `fat32_init()`, use your `read_block()` function to read the Master Boot Record (MBR) and subsequent structures of the FAT32 filesystem.
    - Parse the filesystem to find files and directories.

Good luck!