#include <stddef.h>
#include <xc.h>
#include "i2c.h"
#include "mcc_generated_files/mcc.h"

typedef struct
{
    size_t len;
    uint8_t *data;
}i2c1_buffer_t;

static i2c1_operations_t rd1RegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(ptr,1);
    I2C1_SetDataCompleteCallback(NULL,NULL);
    return I2C1_RESTART_READ;
}

uint8_t I2C1_readReg(uint8_t address, uint8_t reg) {
    
    uint8_t data;
    while(!I2C1_Open(address)); // sit here until we get the bus..
    I2C1_SetDataCompleteCallback(rd1RegCompleteHandler,&data);
    I2C1_SetBuffer(&reg,1);
    I2C1_SetAddressNackCallback(NULL,NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.
    return data;
    /*
    uint8_t data=0;
    uint8_t readAddress = (address << 1) | 1;
    I2C1_open();
    I2C1_startCondition();
    
    I2C1_sendData(readAddress);
    if (I2C1_getAckstatBit())
    {
        goto cleanup;
    }
    
    I2C1_sendData(reg);
    if (I2C1_getAckstatBit())
    {
        goto cleanup;
    }
    
    SSP1CON2bits.RCEN = 1;
    I2C1_interruptFlagPolling();
    data = SSP1BUF;
    
    SSP1CON2bits.ACKDT = 1;
    SSP1CON2bits.ACKEN = 1;
    I2C1_interruptFlagPolling();
    
cleanup:
    I2C1_stopCondition();
    I2C1_close();
    return data;*/
    
}

static i2c1_operations_t wr1RegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(ptr,1);
    I2C1_SetDataCompleteCallback(NULL,NULL);
    return I2C1_CONTINUE;
}

void I2C1_writeCmd(uint8_t address, uint8_t reg) {
    while(!I2C1_Open(address)); // sit here until we get the bus..
    I2C1_SetDataCompleteCallback(NULL,NULL);
    I2C1_SetBuffer(&reg,1);
    I2C1_SetAddressNackCallback(NULL,NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.*/
}

void I2C1_writeReg(uint8_t address, uint8_t reg, uint8_t val) {
    uint8_t cmd[2] = {reg, val};
    while(!I2C1_Open(address)); // sit here until we get the bus..
    I2C1_SetDataCompleteCallback(NULL,NULL);
    I2C1_SetBuffer(&cmd,2);
    I2C1_SetAddressNackCallback(NULL,NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.*/
    
    /* Shift the 7 bit address and add a 0 bit to indicate write operation */
    /*uint8_t writeAddress = (address << 1);
    
    I2C1_open();
    I2C1_startCondition();
    
    I2C1_sendData(writeAddress);
    if (I2C1_getAckstatBit())
    {
        goto cleanup;
    }
    
    I2C1_sendData(reg);
    if (I2C1_getAckstatBit())
    {
        goto cleanup;
    }
    
    I2C1_sendData(val);
    if (I2C1_getAckstatBit())
    {
        goto cleanup;
    }
    
cleanup:
    I2C1_stopCondition();
    I2C1_close();
     */
}

static i2c1_operations_t wrBlkRegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(((i2c1_buffer_t *)ptr)->data,((i2c1_buffer_t*)ptr)->len);
    I2C1_SetDataCompleteCallback(NULL,NULL);
    return I2C1_CONTINUE;
}

void I2C1_writeData(uint8_t address, uint8_t reg, const uint8_t* data, size_t length)
{  
    i2c1_buffer_t bufferBlock; // result is little endian
    bufferBlock.data = data;
    bufferBlock.len = length;

    while(!I2C1_Open(address)); // sit here until we get the bus..
    I2C1_SetDataCompleteCallback(wrBlkRegCompleteHandler,&bufferBlock);
    I2C1_SetBuffer(&reg,1);
    I2C1_SetAddressNackCallback(NULL,NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.*/
    
    /*
    uint8_t writeAddress = (address << 1);
    
    I2C1_open();
    I2C1_startCondition();
    
    I2C1_sendData(writeAddress);
    if (I2C1_getAckstatBit())
    {
        goto cleanup;
    }
    
    if (reg) {
        I2C1_sendData(reg);
        if (I2C1_getAckstatBit())
        {
            goto cleanup;
        }
    }
    
    for (size_t i=0; i<length; ++i) {
    
        I2C1_sendData(data[i]);
        if (I2C1_getAckstatBit())
        {
            goto cleanup;
        }
    }
    
cleanup:
    I2C1_stopCondition();
    I2C1_close();
     * */
}
