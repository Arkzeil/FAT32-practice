#include "lock.h"
#include "uart.h"

static unsigned long long lock_count = 0;

void lock() {
    // Disable interrupts to enter critical section
    // in uspi, it use 0x2 to disable IRQ only while 0xf to disable all (FIQ, IRQ, SError)
    asm volatile("msr daifset, 0x2;");
    lock_count++;
}

void unlock() {
    if(lock_count > 0) {
        lock_count--;
        // Enable interrupts to exit critical section
        asm volatile("msr daifclr, 0x2;");
    }
    else{
        uart_puts("Unlock called without a matching lock!\r\n");
        while(1); // Hang here for debugging
    }
}