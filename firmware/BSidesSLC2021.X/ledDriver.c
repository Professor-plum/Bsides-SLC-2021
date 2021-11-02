#include "i2c.h"
#include "mcc_generated_files/mcc.h"

//#define USE_PREV_LED

#ifdef USE_PREV_LED

#define LED_DRIVER_ADDR   0x14

void leds_init() {
    const uint8_t led_init[] = {
        0x40, 0x3C, 0x00,
        0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF,
        0,0,0,0,0,0,0,0,0,0,0,0
    };
    I2C1_writeReg(LED_DRIVER_ADDR, 0x00, 0x40);
    uint8_t t = I2C1_readReg(LED_DRIVER_ADDR, 0x00);
    if(t)
        __delay_ms(10);
    I2C1_writeReg(LED_DRIVER_ADDR, 0x01, 0x3C);
    I2C1_writeReg(LED_DRIVER_ADDR, 0x02, 0x00);
    I2C1_writeData(LED_DRIVER_ADDR, 0x00, led_init, sizeof(led_init));
}

void leds_set(const uint8_t* pwm) {
    I2C1_writeData(LED_DRIVER_ADDR, 0x0B, pwm, 12);
}

void leds_off() {
    static const uint8_t nn[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    leds_set(nn);
}
#else

#define LED_DRIVER_ADDR        0x54


void leds_init() {
    const uint8_t led_init[] = {
        0x01,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x38, 0x3F, 0x3F,
        0x00, 
    };
    I2C1_writeData(LED_DRIVER_ADDR, 0x00, led_init, sizeof(led_init));
}

void leds_set(const uint8_t* pwm) {
    I2C1_writeData(LED_DRIVER_ADDR, 0x04, pwm, 15);
    I2C1_writeReg(LED_DRIVER_ADDR, 0x16, 0x0);
}

void leds_off() {
    static const uint8_t nn[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    leds_set(nn);
}

void leds_on() {
    static const uint8_t nn[15] = {
        0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
        0x20,0x20,0x20,0x20,0x20,0x20,0x20,};
    leds_set(nn);
}
#endif
