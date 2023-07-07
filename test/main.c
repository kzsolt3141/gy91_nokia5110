#include "twi.h"
#include "MPU9250.h"
#include "BMP280.h"
#include "nokia5110.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

typedef struct SPI_Data_t {
   MPU9250_data mpu_data;
   BMP280_final bmp_data;
} SPI_Data;

SPI_Data spi_data = {0};
const uint8_t max_idx = sizeof(SPI_Data);

static void SPI_cb_handle(void* ctx) {
    const uint8_t *p = (uint8_t*)ctx;

    const uint8_t data = SPDR;

    if (SPCR & (1 << MSTR)) {
    } else {
        SPDR = p[data % max_idx];
    }
}

int main(void) {
    uint8_t sts;
    char message[20] = {0};

    nokia_5110_init(SPI_cb_handle, &spi_data);

    // TWI init
    //-------------------------------
    TWI_init(TWI_PS_1, 2);

    // MPU9250 init
    //-------------------------------
    register_MPU_cb(TWI_write_reg, TWI_read_reg_burst);

    // nokia_5110_write("MPU9250 calib:");

    MPU9250_calib();

    // nokia_5110_write("done. ");

    sts = MPU9250_init();
    if (sts) return sts;

    // nokia_5110_write("MPU9250 OK");

    // BMP280 init
    //-------------------------------
    register_BMP_cb(TWI_write_reg, TWI_read_reg_burst);

    sts = BMP280_init();
    if (sts) return sts;

    // nokia_5110_set_xy(0,3);
    // nokia_5110_write("BMP280 OK");

    nokia_5110_write("   A   |   G");

    while(1) {
        MPU9250_get_data(&spi_data.mpu_data);
        BMP280_get_data(&spi_data.bmp_data);

        snprintf(message, sizeof(message), "X%06d %06d", spi_data.mpu_data.acc[0], spi_data.mpu_data.gyro[0]);
        nokia_5110_set_xy(0,1);
        nokia_5110_write(message);

        snprintf(message, sizeof(message), "Y%06d %06d", spi_data.mpu_data.acc[1], spi_data.mpu_data.gyro[1]);
        nokia_5110_set_xy(0,2);
        nokia_5110_write(message);

        snprintf(message, sizeof(message), "Z%06d %06d", spi_data.mpu_data.acc[2], spi_data.mpu_data.gyro[2]);
        nokia_5110_set_xy(0,3);
        nokia_5110_write(message);

        const int16_t a = spi_data.bmp_data.temp/256/100;
        const int16_t b = spi_data.bmp_data.baro/256/100;

        snprintf(message, sizeof(message), "T %d, P %d", a, b);
        nokia_5110_set_xy(0,5);
        nokia_5110_write(message);

        _delay_ms(100);
    }

    return sts;
}
