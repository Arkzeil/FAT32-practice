#include <uart.h>

void uart_init(void) {
    // allocate an 32 bits register(if we don't assign resiter, it would be allocated in memory)
    register unsigned int reg;

    *AUX_ENABLE         |=  1;      // enable mini UART. Then mini UART register can be accessed.
    *AUX_MU_CNTL_REG    =   0;      // Disable transmitter and receiver during configuration.
    *AUX_MU_IER_REG     =   0;      // Disable interrupt because currently you don’t need interrupt.
    *AUX_MU_LCR_REG     =   3;      // Set the data size to 8 bit.
    *AUX_MU_MCR_REG     =   0;      // Don’t need auto flow control.
    *AUX_MU_BAUD_REG    =   270;    // Set baud rate to 115200
    *AUX_MU_IIR_REG     =   6;      // No FIFO

    // p.92
    //reg = mmio_read(GPFSEL1);
    reg = *GPFSEL1;
    // clear GPIO14,15(~7 = 000)
    reg &= ~((7<<12) | (7<<15));    // 14-12 bits are for gpio14, 17-15 are fir gpio15
    reg |= (2<<12) | (2<<15);       // Assert: set to ALT5 for mini UART, while ALT0 is for PL011 UART
    
    // set GPIO14, 15 to miniUART
    //mmio_write(GPFSEL1, reg);
    *GPFSEL1 = reg;

    // p.101
    *GPPUD              =  0;       // Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither to remove the current Pull-up/down)

    reg = 150;
    // Wait 150 cycles – this provides the required set-up time for the control signal
    while(reg--){
        asm volatile("nop"); 
    }

    //mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    *GPPUDCLK0 = (1 << 14) | (1 << 15); // 1 = Assert Clock on line

    reg = 150;
    // Wait 150 cycles – this provides the required set-up time for the control signal
    while(reg--){
        asm volatile("nop"); 
    }

    // Write to GPPUDCLK0/1 to remove the clock 
    //mmio_write(GPPUDCLK0, 0);
    *GPPUDCLK0 = 0;
    reg = 150;
    // Wait 150 cycles – this provides the required set-up time for the control signal
    while(reg--){
        asm volatile("nop"); 
    }
    *AUX_MU_CNTL_REG    =   3;      // Enable the transmitter and receiver.
}

void uart_putc(char c) {
    // Wait for UART to become ready to transmit
    while (!(*AUX_MU_LSR_REG & (1 << 5))) {
        asm volatile("nop");
    }
    *AUX_MU_IO_REG = c;
    // If no CR, first line of output will be moved right for n chars(n=shell command just input), not sure why
    if(c == '\n'){
        while(!((*AUX_MU_LSR_REG) & 0x20) ){
            asm volatile("nop");
        }
        *AUX_MU_IO_REG = '\r';
    }
}

unsigned int uart_puts(const char *str) {
    unsigned int i = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if(str[i] == '\n')
            uart_putc('\r');
        uart_putc(str[i]);
    }
    return i;
}

unsigned char uart_getc(void) {
    char r;
    // Wait for UART to have received something
    while (!(*AUX_MU_LSR_REG & 0x01)) {
        asm volatile("nop");
    }

    r = (char)(*AUX_MU_IO_REG);
    return r == '\r' ? '\n' : r; // convert CR to LF
}