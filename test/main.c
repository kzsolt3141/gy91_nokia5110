#include "twi.h"
#include "MPU9250.h"
#include "BMP280.h"
#include "nokia5110.h"
#include "spi.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

static inline void spi_init_master() {
    SPI_init_master(SPI_PS_4);
}

int main(void) {
    uint8_t sts;
    char message[100] = {0};

    if (nokia_5110_init(spi_init_master, SPI_RW_byte)) return 1;

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

    MPU9250_data mpu_data = {};

    // BMP280 init
    //-------------------------------
    register_BMP_cb(TWI_write_reg, TWI_read_reg_burst);

    sts = BMP280_init();
    if (sts) return sts;

    // nokia_5110_set_xy(0,3);
    // nokia_5110_write("BMP280 OK");

    BMP280_final bmp_data = {};

    _delay_ms(2000);

    nokia_5110_clear();
    nokia_5110_write("   A   |   G");

    while(1) {
        
        MPU9250_get_data(&mpu_data);
        BMP280_get_data(&bmp_data);

        // printf("%05d\n", mpu_data.tmp);

        snprintf(message, sizeof(message), "X%06d %06d", mpu_data.acc[0], mpu_data.gyro[0]);
        nokia_5110_set_xy(0,1);
        nokia_5110_write(message);

        snprintf(message, sizeof(message), "Y%06d %06d", mpu_data.acc[1], mpu_data.gyro[1]);
        nokia_5110_set_xy(0,2);
        nokia_5110_write(message);

        snprintf(message, sizeof(message), "Z%06d %06d", mpu_data.acc[2], mpu_data.gyro[2]);
        nokia_5110_set_xy(0,3);
        nokia_5110_write(message);


        // printf("%05d, %05d, %05d ", mpu_data.acc[0], mpu_data.acc[1], mpu_data.acc[2]);
        // printf("%d, %d\n", (int16_t)(bmp_data.temp/256/100), (int16_t)(bmp_data.baro/256/100));

        snprintf(message, sizeof(message), "T %d, P %d", (int16_t)(bmp_data.temp/256/100), (int16_t)(bmp_data.baro/256/100));
        nokia_5110_set_xy(0,5);
        nokia_5110_write(message);

        // printf("%05d, %05d, %05d\n", mpu_data.gyro[0], mpu_data.gyro[1], mpu_data.gyro[2]);
        // printf("%05d, %05d, %05d\n", mpu_data.mag[0] - (min_mag[0] + max_mag[0]) / 2, mpu_data.mag[1] -  (min_mag[1] + max_mag[1]) / 2, mpu_data.mag[2] - (min_mag[2] + max_mag[2]) / 2);

        _delay_ms(500);
    }

    return sts;
}
