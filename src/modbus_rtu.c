#include <stdio.h>
#include <avr/interrupt.h>

#include "../lib/avr_usart.h"
#include "../lib/bits.h"
#include "../lib/timer2_setup.h"
#include "../lib/modbus_rtu.h"

PACKET_Type modbus_rtu_pckt;

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

uint8_t modbus_rtu_check()
{
    // Return error or success
}

uint16_t swap_endian(uint16_t data)
{
    return (data << 8) | (data >> 8);
}

ISR(TIMER2_OVF_vect)
{
    // Count bits
}