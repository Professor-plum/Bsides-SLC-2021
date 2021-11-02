#include "IR.h"
#include "mcc_generated_files/mcc.h"

enum irstate {
    none,
    leading_l,
    leading_h,
    bit_l,
    bit_h,
};

static bool ir_received, ir_sending;
static uint16_t ir_count;
static enum irstate ir_state;
static uint8_t ir_idx;
static uint32_t ir_msg, ir_out;

//0x30 0x31, 0x32, 0x34, 0x35, 0x36, 0x37, 0x38

void IrSendMessage(uint8_t msg) {
    uint32_t data = 0xFF00FF00; 
    data ^= IR_ADDR | (IR_ADDR << 8) | ((uint32_t)msg << 16) | ((uint32_t)msg << 24) ; //(FLIP_BYTE(IR_ADDR)<<8) ;
    
    ir_sending = true;
    ir_out = data;
    ir_state = none;
}

void IrReadIsr() {
    bool s = PORTCbits.RC3;
    switch(ir_state) {
        case none:
            if (!s) {
                ir_state = leading_l;
                ir_count=1;
            }
            break;
        case leading_l:
            if (s) {
                if ((ir_count > 75) && (ir_count < 85)) {
                    ir_state = leading_h;
                    ir_count = 1;
                }
                else {
                    ir_state = none;
                }
            }
            else
                ir_count++;
            break;
        case leading_h:
            if (!s) {
                if ((ir_count > 37) && (ir_count < 42)) {
                    ir_state = bit_l;
                    ir_count = 1;
                    ir_idx=0;
                    ir_received = false;
                }
                else {
                    ir_state = none;
                }
            }
            else
                ir_count++;
            break;
        case bit_l:
            if (ir_idx==32) {
                ir_state = none;
                ir_received = true;
                break;
            }
            if (s) {
                ir_state = bit_h;
                ir_count = 1;
            }
            break;
        case bit_h:
            if (!s) {
                if ((ir_count > 3) && (ir_count < 7)) {
                    ir_msg >>= 1;
                    ir_state = bit_l;
                    ir_count = 0;
                    ir_idx++;
                }
                else if ((ir_count > 12) && (ir_count < 18)) {
                    ir_msg >>= 1;
                    ir_msg |= 0x80000000;
                    ir_state = bit_l;
                    ir_count = 0;
                    ir_idx++;
                }
                else {
                    ir_state = none;
                }
            }
            else
                ir_count++;
            break;
    }
}

void IrSendIsr() {
    bool b;
    switch (ir_state) {
        case none:
            PWM5CONbits.PWM5EN = 1;
            ir_count = 1;
            ir_state = leading_h;
            break;
        case leading_h:
            if (ir_count>=80) {
                PWM5CONbits.PWM5EN = 0;
                ir_state = leading_l;
                ir_count = 1;
            }
            else 
                ir_count++;
            break;
        case leading_l:
            if (ir_count>=40) {
                PWM5CONbits.PWM5EN = 1;
                ir_state = bit_h;
                ir_count = 1;
                ir_idx = 0;
            }
            else 
                ir_count++;
            break;
        case bit_h:
            if (ir_count>=5){
                PWM5CONbits.PWM5EN = 0;
                if (ir_idx==32) {
                    ir_sending = false;
                    ir_state = none;
                    ir_count = 0;
                }
                else {
                    ir_state = bit_l;
                    ir_count = 1;
                }
            }
            else 
                ir_count++;
            break;
        case bit_l:
            b = (ir_out >> ir_idx) & 1;
            if ((b && (ir_count >= 15)) ||
                (!b && (ir_count >= 5))) {
                PWM5CONbits.PWM5EN = 1;
                ir_idx++;
                ir_count = 1;
                ir_state = bit_h;
            }
            else
                ir_count++;
            break;
    }
}

void IrIsr(void) {
    if (ir_sending)
    {
        IrSendIsr();
    }
    else {
        IrReadIsr();
    }
}

uint32_t IrGetMessage() {
    if (ir_received) {
        ir_received = false;
        return ir_msg;
    }
    return 0;
}

void IrInit() {
    TMR4_SetInterruptHandler(IrIsr);
}