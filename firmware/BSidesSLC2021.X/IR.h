/* 
 * File:   IR.h
 * Author: plum
 *
 * Created on September 8, 2021, 11:55 AM
 */

#ifndef IR_H
#define	IR_H

#include <stdbool.h>
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define IR_ADDR 0x04
    
void IrInit(void);
void IrSendMessage(uint8_t msg);
uint32_t IrGetMessage(void);    


#ifdef	__cplusplus
}
#endif

#endif	/* IR_H */

