/*
 * avr_usart.c
 *
 *  Created on: Mar 27, 2018
 *      Author: Renan Augusto Starke
 *      Instituto Federal de Santa Catarina
 */

#include <stdio.h>
#include <avr/interrupt.h>

#include "../lib/avr_usart.h"
#include "../lib/bits.h"

static volatile FRAME_Type UART_PCKT;

void USART_Init(uint16_t bauds)
{
	UART_PCKT.FRAME.BUFFER_MASK = 0;
	UART_PCKT.BUFFER_SIZE = 0;
	USART_0->UBRR_H = (uint8_t) (bauds >> 8);
	USART_0->UBRR_L = bauds;

	/* Disable double speed  */
	USART_0->UCSR_A = 0;
	/* Enable TX and RX */
	USART_0->UCSR_B = SET(RXEN0) | SET(TXEN0) | SET(RXCIE0);
	/* Asynchronous mode:
	 * - 8 data bits
	 * - 1 stop bit
	 * - no parity 	 */
	USART_0->UCSR_C = SET(UCSZ01) | SET(UCSZ00);
}


/* Send one byte: busy waiting */
void USART_tx(uint8_t data) 
{
	/* Wait until hardware is ready */
	while (!(USART_0->UCSR_A & (1 << UDRE0)));

	USART_0->UDR_ = data;
}

/* Receive one byte: busy waiting */
uint8_t USART_rx() 
{
	/* Wait until something arrive  */
	while (!(TST_BIT(USART_0->UCSR_A, RXC0)));
	
	return USART_0->UDR_;
}

uint8_t usart_check()
{
	static uint8_t usartReceived = 0;

	if(TST_BIT(USART_0->UCSR_A, RXC0))
		usartReceived = 1;
	else
		usartReceived = 0;
	
	return usartReceived;
}

uint8_t get_usart_buffer_size()
{	
	return UART_PCKT.BUFFER_SIZE;
}

void set_usart_buffer_size(uint8_t lenght)
{
	UART_PCKT.BUFFER_SIZE = lenght-1;
}

void read_usart_buffer(uint8_t readIndex)
{	
	UART_PCKT.FRAME.BUFFER[readIndex] = USART_0->UDR_;
}

void reset_usart_buffer()
{
	UART_PCKT.FRAME.BUFFER[0] = USART_0->UDR_;
	UART_PCKT.FRAME.BUFFER_MASK = 0;
}

uint8_t get_usart_buffer(uint8_t bufferIndex)
{
	return UART_PCKT.FRAME.BUFFER[bufferIndex];
}

ISR(USART_RX_vect){}