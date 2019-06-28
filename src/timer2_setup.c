#include <avr/io.h>
#include <avr/interrupt.h>

#include "../lib/avr_timer.h"
#include "../lib/avr_gpio.h"
#include "../lib/bits.h"
#include "../lib/timer2_setup.h"

void timer2_hardware_init()
{
	GPIO_D->DDR = 0xFF;

	/* Prescaler */
	TIMER_2->TCCRB = SET(CS02) | SET(CS00);
	/* Overflow enable */
	TIMER_IRQS->TC2.BITS.TOIE = 1;
}
