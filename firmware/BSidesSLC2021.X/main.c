/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
        Device            :  PIC16LF18346
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include <stdbool.h>
#include "mcc_generated_files/mcc.h"
#include "FrameBuffer.h"
#include "SSD1306.h"
#include "IR.h"
#include "main.h"
#include "Scenes.h"


uint8_t mm=0;
uint32_t millis;
uint16_t game_state;
uint8_t social_state;
uint32_t sleepat;
uint32_t key_down_t;
uint8_t key_pat_idx;
bool shaken;
uint16_t snow_speed;
uint8_t my_id;


//__EEPROM_DATA (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

void Movment_InterruptHandler(void) {
    mm=1;
}

void delaySleep() {
    sleepat = millis + SLEEP_TIMEOUT;
}

const uint16_t flake_pattern[10] = { 
    0x0010, 0x1020, 0x0804, 0x0400, 0x0202, 0x0009, 0x4000, 0x2040, 0x0100, 0x0080};

void rand_flakes(uint16_t t) {
    uint8_t pwm[15];
    for (uint8_t i=0; i<15; ++i) {
        if (isComplete(i) && ((t >> i) &1)) {
            pwm[i] = (rnd()%0x20);
        } 
        else
            pwm[i] = 0;//rnd()%0x4;
    }
    leds_set(pwm);
}

inline void TickIsr(void) {
    ++millis;
}


bool testLogic() {
    IO_RC6_SetLow();
    IO_RC7_SetLow();
    __delay_ms(2);
    if (IO_RC5_GetValue() != 1)
        return false;
    
    IO_RC6_SetLow();
    IO_RC7_SetHigh();
    __delay_ms(2);
    if (IO_RC5_GetValue() != 1)
        return false;
    
    IO_RC6_SetHigh();
    IO_RC7_SetLow();
    __delay_ms(2);
    if (IO_RC5_GetValue() != 1)
        return false;
    
    IO_RC6_SetHigh();
    IO_RC7_SetHigh();
    __delay_ms(2);
    if (IO_RC5_GetValue() != 0)
        return false;
    
    return true;
}

bool isComplete(uint8_t flag) {
    return (game_state >> flag) & 1;
}

void saveState() {
    DATAEE_WriteByte(ADDR_SAVE, game_state & 0xFF);
    DATAEE_WriteByte(ADDR_SAVE+1, game_state >> 8);
    DATAEE_WriteByte(ADDR_SAVE+2, social_state);
}

void markComplete(uint8_t flag) {
    if (!isComplete(flag)) {
        game_state |= (((uint16_t)1)<<flag);
        saveState();
        snow_speed -= 10;
        if (flag < SOCIAL_BITS)
            showSolved();
    }
}

void loadState() {
    game_state = DATAEE_ReadByte(ADDR_SAVE) |
        ((uint16_t)DATAEE_ReadByte(ADDR_SAVE+1) << 8);
    social_state = DATAEE_ReadByte(ADDR_SAVE+2);
    snow_speed = 360;
    for (uint8_t i=0; i<16; ++i)
        if ((game_state >> i) & 1)
            snow_speed -= 10;
}

static uint8_t s=0xaa,a=0;

inline void seed_rnd() {
    FVRCONbits.TSEN = 1; // enable temp
    s = ADC_GetConversion(channel_Temp) & 0xFF;
}

uint8_t rnd(void) {

        s^=s<<3;
        s^=s>>5;
        s^=a++>>2;
        return s;
}

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    millis = 0;
    TMR6_SetInterruptHandler(TickIsr);
    
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    INT_SetInterruptHandler(Movment_InterruptHandler);
    
    PWM5CONbits.PWM5EN = 0;
    leds_init();
    oled_init();
    mems_init();
    
    if (PCON0bits.nRWDT) {
        leds_on();
        __delay_ms(100);
    }
    leds_off();
    IrInit();
    
    seed_rnd();
    
    my_id = DATAEE_ReadByte(ADDR_ID);
    
    if ( (my_id & 0xF8) != SOCIAL_BITS) {
        my_id = SOCIAL_BITS + rnd()%8;
        game_state=0;
        social_state=0;
        saveState();
        DATAEE_WriteByte(ADDR_ID, my_id);
    }
    
    loadState();
    
    bool btn1 = false, btn2 = false;
    uint8_t idx= TASK_INTRO, c=0;
    sleepat = millis + SLEEP_TIMEOUT;
    uint32_t lastled = 0, lastIr = IR_SPAN;
    key_pat_idx=0;
    while (1)
    { 
        CLRWDT();
        shaken = false;
        uint8_t mems = mems_getStatus();
        if (mems) {
            if (mems & 0x8) {
                shaken = true;  
            }
            delaySleep();
            mm=0; //clear interrupt
        }
        
        if (IO_RA0_GetValue() == 0) {
            if (!btn1) {
                if (key_pat_idx < 4)
                    key_pat_idx++;
                else if (key_pat_idx!=4)
                    key_pat_idx = 1;
                delaySleep();
            }
            btn1=true;
        }
        else {
            if (btn1) {
                idx--;
                if (idx>SCENE_COUNT)
                    idx=SCENE_COUNT;
                if (idx == TASK_ETCH)
                    etch_init();
                else if (idx == TASK_SKI)
                    ski_game_init();
            }
            btn1=false;
        }
        
        if (IO_RA1_GetValue() == 0) {
            if (!btn2) {
                if (key_pat_idx >3)
                    key_pat_idx++;
                else {
                    key_pat_idx = 0;
                }
                key_down_t = millis;
                delaySleep();
            }
            btn2=true;
        }
        else {
            uint32_t d=millis - key_down_t;
            if ((idx==TASK_MATRIX) && (millis > 4000) && (d>2000) && (d<4000)) {
                markComplete(TASK_MATRIX);
            }
            else if ((key_pat_idx==9) && (idx==TASK_DIGITAL)) {
                markComplete(TASK_DIGITAL);
            }
            else if (btn2) {
                idx++;
                if (idx>SCENE_COUNT)
                    idx=0;
                if (idx == TASK_ETCH)
                    etch_init();
                else if (idx == TASK_SKI)
                    ski_game_init();
            }
            btn2=false;
            key_down_t = 0;
        }
       
        uint32_t now = millis;
        if (now < sleepat) {
            if ((now-lastled) > snow_speed) {
                rand_flakes(flake_pattern[c++%10]);
                lastled = now;
            }

            switch(idx) {
                case TASK_ETCH:
                    etch_draw(); break;
                case TASK_TONE:
                    tone_draw(); break;
                case TASK_DIGITAL:
                    digital_draw(); break;
                case TASK_TEMP:
                    temp_draw(); break;
                case TASK_TV:
                    static_draw(); break;
                case TASK_MATRIX:
                    matrix_draw(); break;
                case TASK_CLOCK:
                    clock_draw(); break;
                case TASK_RADIO:
                    radio_draw(); break;
                case TASK_EQUAL:
                    equal_draw(); break;
                case TASK_SLIDER:
                    slider_draw(); break;
                case TASK_ASSEM:
                    assem_draw(); break;
                case TASK_LIFTS:
                    lifts_draw(); break;
                case TASK_SKI:
                    ski_game_draw(); break;
                case TASK_SOCIAL:
                    social_draw(); break;
                case TASK_PONG:
                    pong_draw(); break;
                case TASK_INTRO:
                    intro_draw(); break;
                default:
                    idx = TASK_INTRO;
                    break;
            }

            if (now > lastIr) { 
                IrSendMessage(my_id);
                lastIr = now + IR_SPAN + rnd();
            }
        }
        else {
            leds_off();
            oled_sleep();
            FVRCON = 0; // disable FVR
            IO_RC2_SetLow();
            //IO_RB7_SetLow();
            if (mems_getStatus()) {//(just incase it moves while shutting down)
                __delay_ms(250); //give oled time to recover
            }
            else {
                SLEEP();  
                NOP();
                NOP();
            }
            //IO_RB7_SetHigh();
            IO_RC2_SetHigh();
            oled_init();
            leds_init();
            FVR_Initialize();
            sleepat = millis + SLEEP_TIMEOUT;
        }
        
    }
}


void panic() {
    for (uint8_t i=0; i<5; ++i) {
        leds_on();
        CLRWDT();
        __delay_ms(500);
        leds_off();
        __delay_ms(500);
    }
}
/**
 End of File
*/