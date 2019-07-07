#include <avr/io.h>
#include <avr/interrupt.h>

#include "../lib/avr_timer.h"
#include "../lib/avr_gpio.h"
#include "../lib/bits.h"
#include "../lib/timer2_setup.h"

void timer2_hardware_init()
{
	OCR2A = 208;
    // CTC
    TIMER_2->TCCRA = SET(WGM21);
    // Prescaler = 1
	TIMER_2->TCCRB = SET(CS21) | SET(CS20);
    TIMER_IRQS->TC2.BITS.OCIEA = 1;
}
