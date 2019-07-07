/*
 * avr_usart.h
 *
 *  Created on: May 1, 2018
 *      Author: Renan Augusto Starke
 *      Instituto Federal de Santa Catarina
 */

#ifndef _USART_H
#define _USART_H

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>

#ifndef _IO
#define _IO volatile uint8_t
#endif

#ifndef _IO16
#define _IO16 volatile uint16_t
#endif

/* Baudrates */
#define B9600  (F_CPU/16/9600-1)
#define B57600  (F_CPU/16/57600-1)
#define B115200  (F_CPU/16/115200-1)

/* Registers structure definition */
typedef struct
{
	_IO UCSR_A;   	/*!< USART Control and Status Register A */
	_IO UCSR_B;		/*!< USART Control and Status Register B */
	_IO UCSR_C;		/*!< USART Control and Status Register C */
	_IO unused;
	_IO UBRR_L;   	/*!< USART Baud Rate Low Register */
	_IO UBRR_H;		/*!< USART Baud Rate High Register */
	_IO UDR_;		/*!< USART I/O Data Register  */
} UART_Type;

#define USART_0 ((UART_Type *) &UCSR0A)

typedef struct 
{
	union
	{
		uint64_t BUFFER_MASK;
		uint8_t BUFFER[8];
	}FRAME;
	uint8_t BUFFER_SIZE;
}FRAME_Type;

void USART_Init(uint16_t bauds);
void set_usart_buffer_size(uint8_t lenght);
uint8_t get_usart_buffer_size();
void read_usart_buffer(uint8_t readIndex);
void reset_usart_buffer();
uint8_t get_usart_buffer(uint8_t bufferIndex);
void USART_tx(uint8_t data);
uint8_t USART_rx();
uint8_t usart_check();

#endif