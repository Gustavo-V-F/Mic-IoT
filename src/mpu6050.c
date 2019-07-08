#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../lib/avr_twi_master.h"
#include "../lib/bits.h"
#include "../lib/mpu6050.h"

#define ACELX_OFFSET 5327
#define ACELX_BIT 1
#define ACELY_OFFSET 307
#define ACELY_BIT 1
#define ACELZ_OFFSET 3697
#define ACELZ_BIT 0
#define ACEL_SCALE_2 0x00
#define ACEL_SCALE_4 0x08
#define ACEL_SCALE_8 0x10
#define ACEL_SCALE_16 0x18

#define GYROX_OFFSET 113
#define GYROX_BIT 0
#define GYROY_OFFSET 19
#define GYROY_BIT 1
#define GYROZ_OFFSET 69
#define GYROZ_BIT 1
#define GYRO_SCALE_250 0x00
#define GYRO_SCALE_500 0x08
#define GYRO_SCALE_1000 0x10
#define GYRO_SCALE_2000 0x18

#define START 0x08
#define LOW_PASS_FILTER 0x1A
#define LPF0 0x00
#define LPF1 0x01

static MPU6050_Type MPU6050;
static unsigned char readMPU6050Register[7] = {MPU6050_ADDR_R};
static unsigned char writeMPU6050Register[3] = {MPU6050_ADDR_W};

void MPU6050_init()
{
    TWI_Master_Initialise();
    
    MPU6050_write(MPU6050_ADDR_C, START);
    MPU6050_write(LOW_PASS_FILTER, LPF_01);
    MPU6050_write(GYRO_ADDR_C, GYRO_SCALE_250);
    MPU6050_write(ACEL_ADDR_C, ACEL_SCALE_2);

    MPU6050.ACELEROMETER.OFFSET.XOFFSET = ACELX_OFFSET;
    MPU6050.ACELEROMETER.OFFSET.YOFFSET = ACELY_OFFSET;
    MPU6050.ACELEROMETER.OFFSET.ZOFFSET = ACELZ_OFFSET;
    MPU6050.ACELEROMETER.OFFSET.BITS_SIGN.BITS_MASK = (ACELX_BIT | ACELY_BIT | ACELZ_BIT);
    MPU6050.ACELEROMETER.SCALE = 16384;

    MPU6050.GYROSCOPE.OFFSET.XOFFSET = GYROX_OFFSET;
    MPU6050.GYROSCOPE.OFFSET.YOFFSET = GYROY_OFFSET;
    MPU6050.GYROSCOPE.OFFSET.ZOFFSET = GYROZ_OFFSET;
    MPU6050.GYROSCOPE.OFFSET.BITS_SIGN.BITS_MASK = (GYROX_BIT | GYROY_BIT | GYROZ_BIT);
    MPU6050.GYROSCOPE.SCALE = 131;

    _delay_ms(30);
}

void MPU6050_write(unsigned char regAddr, unsigned char cmd)
{
    writeMPU6050Register[1] = regAddr;
    writeMPU6050Register[2] = cmd;
    TWI_Start_Transceiver_With_Data(writeMPU6050Register, 3);
}

void MPU6050_read(unsigned char regAddr, uint8_t *data, uint8_t size)
{
    uint8_t readIndex;
    writeMPU6050Register[1] =  regAddr;

    TWI_Start_Transceiver_With_Data(writeMPU6050Register, 2);
    TWI_Start_Transceiver_With_Data(readMPU6050Register, size+1);
    TWI_Get_Data_From_Transceiver(readMPU6050Register, size+1);
    for(readIndex = 0; readIndex < size; readIndex++)
        data[readIndex] = readMPU6050Register[readIndex+1];
}

void MPU6050_read_raw_acel()
{
    MPU6050_read(ACEL_ADDR_R, (uint8_t *)(&MPU6050.ACELEROMETER), 6);
}

void MPU6050_read_raw_gyro()
{
    MPU6050_read(GYRO_ADDR_R, (uint8_t *)(&MPU6050.GYROSCOPE), 6);
}

void MPU6050_read_acel()
{
    uint16_t *acelAux = (uint16_t *)(&MPU6050.ACELEROMETER);
    uint16_t *acelOffsetAux = (uint16_t *)(&MPU6050.ACELEROMETER.OFFSET);
    uint8_t readIndex;

    MPU6050_read_raw_acel();

    for(readIndex = 0; readIndex < 3; readIndex++){
        if(TST_BIT(MPU6050.ACELEROMETER.OFFSET.BITS_SIGN.BITS_MASK, readIndex))
            acelAux[readIndex] = (acelAux[readIndex]-acelOffsetAux[readIndex])/MPU6050.ACELEROMETER.SCALE;
        else
            acelAux[readIndex] = (acelAux[readIndex]+acelOffsetAux[readIndex])/MPU6050.ACELEROMETER.SCALE;
    }    
}

void MPU6050_read_gyro()
{
    uint16_t *gyroAux = (uint16_t *)(&MPU6050.GYROSCOPE);
    uint16_t *gyroOffsetAux = (uint16_t *)(&MPU6050.GYROSCOPE.OFFSET);
    uint8_t readIndex;

    MPU6050_read_raw_gyro();

    for(readIndex = 0; readIndex < 3; readIndex++){
        if(TST_BIT(MPU6050.GYROSCOPE.OFFSET.BITS_SIGN.BITS_MASK, readIndex))
            gyroAux[readIndex] = (gyroAux[readIndex]-gyroOffsetAux[readIndex])/MPU6050.GYROSCOPE.SCALE;
        else
            gyroAux[readIndex] = (gyroAux[readIndex]+gyroOffsetAux[readIndex])/MPU6050.GYROSCOPE.SCALE;
    }    
}

void MPU6050_get_raw_data(uint16_t *acelData, uint16_t *gyroData)
{
    uint8_t readIndex;

    uint16_t *acelAux = (uint16_t *)(&MPU6050.ACELEROMETER);
    uint16_t *gyroAux = (uint16_t *)(&MPU6050.GYROSCOPE);

    MPU6050_read_raw_acel();
    MPU6050_read_raw_gyro();

    for(readIndex = 0; readIndex < 3; readIndex++){
        acelData[readIndex] = acelAux[readIndex];
        gyroData[readIndex] = gyroAux[readIndex];
    }
}

void MPU6050_get_data(uint16_t *acelData, uint16_t *gyroData)
{
    uint8_t readIndex;

    uint16_t *acelAux = (uint16_t *)(&MPU6050.ACELEROMETER);
    uint16_t *gyroAux = (uint16_t *)(&MPU6050.GYROSCOPE);

    MPU6050_read_acel();
    MPU6050_read_gyro();

    for(readIndex = 0; readIndex < 3; readIndex++){
        acelData[readIndex] = acelAux[readIndex];
        gyroData[readIndex] = gyroAux[readIndex];
    }
}