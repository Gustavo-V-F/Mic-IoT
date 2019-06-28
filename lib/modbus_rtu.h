#ifndef _MODBUS_RTU_H
#define _MODBUS_RTU_H

typedef struct 
{
	uint8_t BUFFER;
	union
	{
		uint8_t EMPTY;
		struct
		{
			uint8_t ONE:1;
			uint8_t TWO:1;
			uint8_t THREE:1;
			uint8_t FOUR:1;
			uint8_t FIVE:1;
			uint8_t SIX:1;
			uint8_t SEVEN:1;
			uint8_t EIGHT:1;
		}EMPTY_BYTES;
	}PACKET_FLAG;
}PACKET_Type;

#endif