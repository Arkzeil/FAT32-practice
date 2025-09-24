# FAT32 Firmware Practice

This project provides a bare-metal environment for practicing FAT32 filesystem implementation on a QEMU-emulated Raspberry Pi 3 (AArch64).

## Prerequisites

- `qemu-system-aarch64`
- `aarch64-linux-gnu` toolchain
- `make`
- `mkfs.vfat` (usually available in `dosfstools` or a similar package)

## Structure

- `src/boot.S`: Minimal assembly bootloader. Sets up the stack and jumps to `main`.
- `src/main.c`: The main kernel entry point. Contains high-level logic and calls to subsystems.
- `src/uart.c`: Implements basic UART (serial) communication functions for console output.
- `include/gpio.h`: Header file for GPIO (General Purpose Input/Output) pin definitions and functions.
- `include/uart.h`: Header file for UART (serial) communication definitions and function prototypes.
- `linker.ld`: Linker script to place the kernel at the correct memory address (`0x80000`).
  - **Important Note:** The order inside the `.text` section of the linker script is critical. The boot code (from the `.text.boot` section) must be placed *first* to ensure the CPU starts at the `_start` label. A simple `*(.text*)` wildcard is not enough as it does not guarantee this order. The correct method is to explicitly place the boot section first, e.g., `KEEP(*(.text.boot))`, before including other text sections.
- `Makefile`: Automates the build process.
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
    - Launch QEMU with `fat32.img` attached as an SD card.

You will see the "Hello, world!" message from the UART in your terminal.

Your task is to implement the necessary drivers (e.g., for the SD card controller) and the FAT32 logic within `src/main.c` to read data from the attached disk image.
