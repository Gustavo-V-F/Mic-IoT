#ifndef _MPU6050_H
#define _MPU6050_H

#define MPU6050_ADDR_W 0xD0
#define MPU6050_ADDR_R 0xD1
#define MPU6050_ADDR_C 0x6B

#define GYRO_ADDR_R 0x43
#define GYRO_ADDR_C 0x1B

#define ACEL_ADDR_R 0x3B
#define ACEL_ADDR_C 0x1C

typedef struct{
    uint16_t X;
    uint16_t Y;
    uint16_t Z;
    uint16_t SCALE;
    
    struct 
    {
        uint16_t XOFFSET;
        uint16_t YOFFSET;
        uint16_t ZOFFSET;
        union 
        {   
            uint8_t BITS_MASK;
            struct{
                uint8_t OFT1:1;
                uint8_t OFT2:1;
                uint8_t OFT3:1;
                uint8_t UNUSED:5;
            }SINGLE_BIT;
        }BITS_SIGN;
    }OFFSET;
}COORD_Type;

typedef struct
{
    COORD_Type ACELEROMETER;
    COORD_Type GYROSCOPE;
}MPU6050_Type;

void MPU6050_init();
void MPU6050_write(unsigned char regAddr, uint8_t);
void MPU6050_read(unsigned char regAddr, uint8_t *data, uint8_t size);
void MPU6050_read_raw_acel();
void MPU6050_read_raw_gyro();
void MPU6050_read_acel();
void MPU6050_read_gyro();
void MPU6050_get_raw_data(uint16_t *acelData, uint16_t *gyroData);
void MPU6050_get_data(uint16_t *acelData, uint16_t *gyroData);

#endif