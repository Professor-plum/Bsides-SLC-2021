/*
 * File:   SSD1306.c
 * Author: plum
 *
 * Created on September 3, 2021, 3:19 PM
 */

#include <stdint.h>
#include "SSD1306.h"
#include "i2c.h"
#include <xc.h>
#include "mcc_generated_files/mcc.h"

#define SCREEN_ADDRESS  0x3C

void oled_init() {
    
    const uint8_t init_commands[] = {
        0xAE, //SSD1306_DISPLAYOFF
        0xD5, //SSD1306_SETDISPLAYCLOCKDIV
        0x80,
        0xA8, //SSD1306_SETMULTIPLEX
        OLED_HEIGHT-1, //Height -1 
        0xD3, //SSD1306_SETDISPLAYOFFSET
        0x00, //no offset
        0x40, //SSD1306_SETSTARTLINE
        0x8D, //SSD1306_CHARGEPUMP
        0x14, //SSD1306_SWITCHCAPVCC
        0x20, //SSD1306_MEMORYMODE
        0x00, //act like ks0108
        0xA1, //SSD1306_SEGREMAP
        0xC8, //SSD1306_COMSCANDEC
        0xDA, //SSD1306_SETCOMPINS
        0x12,
        0x81, //SSD1306_SETCONTRAST
        0x7F,
        0xD9, //SSD1306_SETPRECHARGE
        0xF1,
        0xDB, //SSD1306_SETVCOMDETECT
        0x40,
        0xA4, //SSD1306_DISPLAYALLON_RESUME
        0xA6, //SSD1306_NORMALDISPLAY
        0x2E, //SSD1306_DEACTIVATE_SCROLL
        0xAF, //SSD1306_DISPLAYON
    };
    
    IO_RB6_SetLow();
    __delay_ms(10);
    IO_RB6_SetHigh();
    __delay_ms(10);
    
    I2C1_writeData(0x3C, 0, init_commands, sizeof(init_commands));

}

void oled_draw_image(uint8_t* img) {
    const uint8_t draw_commands[] = {
        0x21, //SSD1306_COLUMNADDR
        0, 
        OLED_WIDTH -1,
        0x22, //SSD1306_PAGEADDR
        0,
        0x7,
    };
    
    I2C1_writeData(0x3C, 0, draw_commands, sizeof(draw_commands));
    
    I2C1_writeData(0x3C, 0x40, img, 1024);
  
}

void oled_sleep() {
    I2C1_writeReg(0x3C, 0, 0xAE);
    IO_RB6_SetLow();
}

void oled_wakeup() {
    I2C1_writeReg(0x3C, 0, 0xAF);
}

void oled_invert(bool inverted) {
    I2C1_writeReg(0x3C, 0, inverted?0xA7:0xA6);
}