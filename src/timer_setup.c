#include "../lib/timer_setup.h"

void timer0_hardware_init()
{
	GPIO_D->DDR = 0xFF;

	/* Prescaler */
	TIMER_0->TCCRB = SET(CS02) | SET(CS00);
	/* Overflow enable */
	TIMER_IRQS->TC0.BITS.TOIE = 1;
}
