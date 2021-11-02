/* 
 * File:   main.h
 * Author: plum
 *
 * Created on September 13, 2021, 3:09 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define TASK_ETCH       0
#define TASK_TONE       1
#define TASK_DIGITAL    2
#define TASK_TEMP       3
#define TASK_TV         4
#define TASK_MATRIX     5
#define TASK_CLOCK      6
#define TASK_RADIO      7
#define TASK_EQUAL      8
#define TASK_SLIDER     9
#define TASK_ASSEM      10
#define TASK_LIFTS      11
#define TASK_SKI        12
#define TASK_SOCIAL     13
#define TASK_PONG       14
#define SOCIAL_BITS     16


void leds_init(void);
void leds_set(const uint8_t* pwm);
void leds_off(void);
void leds_on(void);

void mems_init(void);
uint8_t mems_getStatus(void);


bool isComplete(uint8_t flag);
void markComplete(uint8_t flag);

void panic(void);
uint8_t rnd(void);

void delaySleep(void);
bool testLogic(void);

extern bool shaken;
extern uint32_t game_state;


#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

