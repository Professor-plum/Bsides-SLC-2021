#include "i2c.h"
#include "main.h"
#include "mcc_generated_files/mcc.h"

#define MEMS_ADDR   0x4C

void mems_init() {
    
    static const uint8_t cfg[] = {
        0x80, //anyM LSB
        0x00, //anyM MSB
        0x10, //anyM deb                 ounce
        0x40, //shake LSB,
        0x02, //shake MSB,
        0x10, //shake duration LSB
        0x11, //shake duration MSB
    };
    
    I2C1_writeReg(MEMS_ADDR, 0x07, 0);
    __delay_ms(10);
    
    uint8_t t = I2C1_readReg(MEMS_ADDR, 0x18); 
    if (t!=0xA4) 
        panic();  //Couldn't talk to chip!
    
    I2C1_writeReg(MEMS_ADDR, 0x06, 0x0C); //enable anyM and shake interrupts
    I2C1_writeReg(MEMS_ADDR, 0x08, 0x10); //25Hz IDR/ODR
    I2C1_writeReg(MEMS_ADDR, 0x09, 0x0C); //enable anyM and Shake
    
    I2C1_writeData(MEMS_ADDR, 0x43, cfg, 7);
    I2C1_writeReg(MEMS_ADDR, 0x14, 0x00); //Clear interrupts 
    
    I2C1_writeReg(MEMS_ADDR, 0x07, 0x01); //Enable operations
}


uint8_t mems_getStatus() {
    uint8_t r = I2C1_readReg(MEMS_ADDR, 0x14);
    I2C1_writeReg(MEMS_ADDR, 0x14, 0x00); //Clear interrupts
    return r & 0xC;
}