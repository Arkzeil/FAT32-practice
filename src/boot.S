.section ".text.boot"

.global _start

_start:
    // Set stack pointer to just before our code
    mov sp, #0x80000

    // usually we should disable cores except core0 and clear bss before Jump
    // Jump to the C main function
    bl main

// If main returns, hang here
halt:
    wfe
    b halt