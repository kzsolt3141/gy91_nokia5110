/*
 * adc.c
 *
 * Created: 12/17/2019 12:09:35 AM
 * Author : Zsolt
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "MPU9250.h"
#include "BMP280.h"

#include "spi.h"
#include "uart.h"

typedef struct SPI_Data_t {
   MPU9250_data mpu_data;
   BMP280_final bmp_data;
} SPI_Data;

SPI_Data spi_data = {0};
const uint8_t max_idx = sizeof(SPI_Data);

int main(void) {
    // UART INIT
    //-------------------------------
    const uint16_t baud_rate = 38400;

    USART_init(baud_rate);

    printf("Init Done UART baud: %u\n", (uint16_t)baud_rate);
    //-------------------------------

    // SPI INIT
    //-------------------------------

    regiter_SPI_isr_cb(NULL, NULL);

    // PORTC INIT
    //-------------------------------
    DDRC |= (1 << PC0);
    PORTC |= (1 << PC0);

    uint8_t* p = (uint8_t*)&spi_data;

    while (1) {
        SPI_init_master(SPI_PS_16);
        // printf("Init Done SPI\n");
        for (uint8_t i = 0; i <= max_idx; i++) {
            PORTC &= ~(1 << PC0);
            const uint8_t rcv = SPI_RW_byte(i);
            if (i) {
                p[i-1] = rcv;
                printf("req:%d rcv:%02x\n", i-1, rcv);
            }
        }

        // printf( "ACC: %06d %06d %06d\n", spi_data.mpu_data.acc[0], spi_data.mpu_data.acc[1], spi_data.mpu_data.acc[2]);
        // printf( "GYR: %06d %06d %06d\n", spi_data.mpu_data.gyro[0], spi_data.mpu_data.gyro[1], spi_data.mpu_data.gyro[2]);
        // printf( "MAG: %06d %06d %06d\n", spi_data.mpu_data.mag[0], spi_data.mpu_data.mag[1], spi_data.mpu_data.mag[2]);
        // printf( "T: %06d P:%06d \n", (int16_t)(spi_data.bmp_data.temp/256/100), (int16_t)(spi_data.bmp_data.baro/256/100));

        SPCR = 0;
        DDRB = 0;

        PORTC |= (1 << PC0);
        _delay_ms(1000);

    };

    return 0;
}
