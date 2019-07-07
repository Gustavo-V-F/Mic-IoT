#ifndef _MODBUS_RTU_H
#define _MODBUS_RTU_H

#define MODBUS_RTU_ADDR 0x15
#define MODBUS_RTU_CMD_W 0x01
#define MODBUS_RTU_CMD_R 0x02

typedef struct 
{
	uint8_t _ADDR;
	uint8_t _CMD;
	uint16_t _REG;
	uint16_t _DATA; 
	uint16_t _CRC;
}PACKET_Type;

void modbus_rtu_init();
uint16_t modbus_rtu_check();
void modbus_rtu_read();
void modbus_rtu_write(uint16_t reg, uint16_t data);
void modbus_rtu_error(uint8_t error);
uint16_t get_modbus_rtu_data();
uint16_t CRC16_2(uint8_t *buf, int len);
uint16_t swap_endian(uint16_t data);

#endif