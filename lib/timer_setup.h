#ifndef TIMER_SETUP_H_
#define TIMER_SETUP_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../lib/avr_timer.h"
#include "../lib/avr_gpio.h"
#include "../lib/bits.h"

void timer0_hardware_init();

#endif  /* TIMER_SETUP_H_ */