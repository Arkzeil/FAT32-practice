#include "timer.h"

static volatile unsigned long long timer_ticks, timer_ticks_ms = 0;


void timer_init() {
    // Initialize the timer hardware
    asm volatile("mrs %0, cntfrq_el0" : "=r"(timer_ticks));
    timer_ticks_ms = timer_ticks / 1000; // ticks per millisecond
}

void delay_ms(unsigned int ms) {
    if(timer_ticks_ms == 0) {
        timer_init();
    }
    // Busy-wait loop for the specified milliseconds
    unsigned long long start, end;
    // asm: one output two inputs
    // '=' indicating that the operand is an output operand and will be written to by the assembly instruction
    // cntvct_el0 is read-only
    // asm volatile(
    //     "msr cntvct_el0, %0;"
    //     :
    //     : "r" (timer_ticks)
    //     :
    // );
    asm volatile(
        "mrs %0, cntvct_el0;"
        : "=r" (start)
    );
    end = start + ms * timer_ticks_ms;

    while (start < end) {
        asm volatile(
            "mrs %0, cntvct_el0;"
            : "=r" (start)
        );
    }
}

void timer_reset() {
    // Reset the timer
    timer_ticks = 0;
    timer_ticks_ms = 0;
    timer_init();
}
