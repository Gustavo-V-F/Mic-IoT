#include <stdio.h>
#include <avr/interrupt.h>

#include "../lib/avr_usart.h"
#include "../lib/bits.h"
#include "../lib/timer2_setup.h"
#include "../lib/modbus_rtu.h"

typedef struct
{
  uint8_t tx_sent:1;
  uint8_t rx_received:1;
  uint8_t rx_timeout:1;
  uint8_t not_used:5;
}modbus_rtu_flags_Type;

static volatile PACKET_Type modbus_rtu_pckt[2];
static volatile modbus_rtu_flags_Type modbus_rtu_flags; 

uint8_t *modbus_rtu_tx = (uint8_t *)(&modbus_rtu_pckt[0]);
uint8_t *modbus_rtu_rx = (uint8_t *)(&modbus_rtu_pckt[1]);

void modbus_rtu_init()
{
  modbus_rtu_pckt[0]._ADDR = MODBUS_RTU_ADDR;
  modbus_rtu_flags.tx_sent = 0;
  modbus_rtu_flags.rx_received = 0;
  modbus_rtu_flags.rx_timeout = 0;

  timer2_hardware_init();

  USART_Init(B9600);
}

uint16_t modbus_rtu_check()
{
  uint8_t rxIndex;

  if(modbus_rtu_flags.rx_received == 1){
    for(rxIndex = 0; rxIndex < 8; rxIndex++)
      modbus_rtu_rx[rxIndex] = get_usart_buffer(rxIndex);

    modbus_rtu_flags.tx_sent = 0;
    modbus_rtu_flags.rx_received = 0;

    if(modbus_rtu_pckt[1]._REG != modbus_rtu_pckt[0]._REG)
      return (modbus_rtu_pckt[1]._CMD << 8) | 0x00FE;

    if(modbus_rtu_pckt[1]._CRC != swap_endian(CRC16_2(modbus_rtu_rx, 6)))
      return (modbus_rtu_pckt[1]._CMD << 8) | 0x00FF;

    return (0x0000) | modbus_rtu_pckt[1]._CMD;

  }else if(modbus_rtu_flags.rx_timeout == 1){
    for(rxIndex = 0; rxIndex < 8; rxIndex++)
      modbus_rtu_rx[rxIndex] = get_usart_buffer(rxIndex);

    modbus_rtu_flags.tx_sent = 0;
    modbus_rtu_flags.rx_received = 0;
    modbus_rtu_flags.rx_timeout = 0; 

    return (modbus_rtu_pckt[1]._CMD << 8) | 0x00FF;
  }

  return 0;
}

void modbus_rtu_read(uint16_t reg)
{
  uint8_t pcktIndex;

  if(modbus_rtu_flags.tx_sent == 0){
    modbus_rtu_pckt[0]._CMD = MODBUS_RTU_CMD_R;
	  modbus_rtu_pckt[0]._DATA = swap_endian(0x0001);
    modbus_rtu_pckt[0]._REG = swap_endian(reg);
    modbus_rtu_pckt[0]._CRC = swap_endian(CRC16_2(modbus_rtu_tx, 6));

    reset_usart_buffer();

    for(pcktIndex = 0; pcktIndex < 8; pcktIndex++)
      USART_tx(modbus_rtu_tx[pcktIndex]);
  }
  
  modbus_rtu_flags.tx_sent = 1;
}

void modbus_rtu_write(uint16_t reg, uint16_t data)
{
  uint8_t pcktIndex;

  if(modbus_rtu_flags.tx_sent == 0){
    modbus_rtu_pckt[0]._CMD = MODBUS_RTU_CMD_W;
	  modbus_rtu_pckt[0]._DATA = swap_endian(data);
    modbus_rtu_pckt[0]._REG = swap_endian(reg);
    modbus_rtu_pckt[0]._CRC = swap_endian(CRC16_2(modbus_rtu_tx, 6));

    reset_usart_buffer();

    for(pcktIndex = 0; pcktIndex < 8; pcktIndex++)
      USART_tx(modbus_rtu_tx[pcktIndex]);

    modbus_rtu_flags.tx_sent = 1;
  }
}

void modbus_rtu_error(uint8_t error)
{
  uint8_t cmdAux;

  if(modbus_rtu_flags.tx_sent == 0){
    cmdAux = modbus_rtu_pckt[0]._CMD;
    modbus_rtu_pckt[0]._CMD = error;
    modbus_rtu_pckt[0]._CRC = swap_endian(CRC16_2(modbus_rtu_tx, 2));
    modbus_rtu_pckt[0]._CMD = cmdAux;

    reset_usart_buffer();

    USART_tx(modbus_rtu_tx[0]);
    USART_tx(error);
    USART_tx(modbus_rtu_tx[6]);
    USART_tx(modbus_rtu_tx[7]);

    modbus_rtu_flags.tx_sent = 1;
  }
}

uint16_t get_modbus_rtu_data()
{
  return swap_endian(modbus_rtu_pckt[1]._DATA);
}

uint16_t CRC16_2(uint8_t *buf, int len)
{
  uint32_t crc = 0xFFFF;
  int i;

  for (i = 0; i < len; i++)
  {
    crc ^= (uint16_t)buf[i];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {    // Loop over each bit
      if ((crc & 0x0001) != 0) {      // If the LSB is set
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}

uint16_t swap_endian(uint16_t data)
{
  return (data << 8) | (data >> 8);
}

ISR(TIMER2_COMPA_vect)
{
  static uint8_t packetIndex = 0;
  static uint16_t timeoutIndex = 0;
  static uint8_t usartReceived;
  static uint8_t usartReceivedOld = 0;

  if(modbus_rtu_flags.tx_sent == 1){
    if(packetIndex > 7){
      packetIndex = 0;
      timeoutIndex = 0;
      usartReceived = 0;
      usartReceivedOld = 0;
      modbus_rtu_flags.rx_received = 1;
    }else if(timeoutIndex > 10000){
      packetIndex = 0;
      timeoutIndex = 0;
      usartReceived = 0;
      usartReceivedOld = 0;
		  modbus_rtu_flags.rx_timeout = 1;
	  }

    usartReceived = usart_check();

    if(usartReceivedOld == 0 && usartReceived == 1){
      read_usart_buffer(packetIndex);
      packetIndex++;
    }

    usartReceivedOld = usartReceived;

    timeoutIndex++;
  }
}