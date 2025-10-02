#ifndef TIMER_H
#define TIMER_H

void timer_init();
// simple blocking delay
// proper way is to use interrupt, but we don't have OS here
void delay_ms(unsigned int ms);
void timer_reset();

#endif // TIMER_H