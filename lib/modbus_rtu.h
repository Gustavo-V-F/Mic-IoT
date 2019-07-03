#ifndef _MODBUS_RTU_H
#define _MODBUS_RTU_H

typedef struct 
{
	uint8_t _ADDR;
	uint8_t _CMD;
	uint16_t _REG;
	uint16_t _DATA; 
	uint16_t _CRC;

	union
	{
		uint8_t FILLED;
		struct
		{
			uint8_t ADDR_F:1;
			uint8_t CMD_F:1;
			uint8_t REG_F:2;
			uint8_t DATA_F:2;
			uint8_t CRC_F:2;
		}BYTES;
	}PACKET_FLAG;
}PACKET_Type;

uint16_t CRC16_2(uint8_t *buf, int len);
uint8_t modbus_rtu_check();
uint16_t swap_endian(uint16_t data);

#endif