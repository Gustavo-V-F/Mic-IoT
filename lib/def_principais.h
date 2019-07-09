#ifndef DEF_PRINCIPAIS_H
#define DEF_PRINCIPAIS_H

#define F_CPU 16000000UL	//define a frequencia do microcontrolador - 16MHz

#include <avr/io.h> 	    //definições do componente especificado
#include <util/delay.h>		//biblioteca para o uso das rotinas de _delay_ms e _delay_us()
#include <avr/interrupt.h>
#include <avr/pgmspace.h>   //para o uso do PROGMEM, gravação de dados na memória flash
#include <stdio.h>
#include "../lib/bits.h"
#include "../lib/avr_gpio.h"
#include "../lib/avr_timer.h"
#include "../lib/timer0_setup.h"
#include "../lib/timer2_setup.h"
#include "../lib/avr_twi_master.h"
#include "../lib/avr_usart.h"
#include "../lib/modbus_rtu.h"
#include "../lib/mpu6050.h"

#endif