/* 
 * File:   i2c.h
 * Author: plum
 *
 * Created on September 13, 2021, 1:35 PM
 */

#ifndef I2C_H
#define	I2C_H

#include <stdint.h>
#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
uint8_t I2C1_readReg(uint8_t address, uint8_t reg);
void I2C1_writeCmd(uint8_t address, uint8_t reg);
void I2C1_writeReg(uint8_t address, uint8_t reg, uint8_t val);
void I2C1_writeData(uint8_t address, uint8_t reg, const uint8_t* data, size_t length);


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

