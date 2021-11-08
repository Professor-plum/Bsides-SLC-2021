/* 
 * File:   Scenes.h
 * Author: plum
 *
 * Created on September 13, 2021, 5:17 PM
 */

#ifndef SCENES_H
#define	SCENES_H

#ifdef	__cplusplus
extern "C" {
#endif

void int2hex(uint16_t i, char* out); 
void showSolved(void);
void etch_init(void);
void ski_game_init(void);

void etch_draw(void);
void tone_draw(void);
void digital_draw(void);
void temp_draw(void);
void static_draw(void);
void matrix_draw(void);
void clock_draw(void);
void radio_draw(void);
void equal_draw(void);
void slider_draw(void);
void assem_draw(void);
void lifts_draw(void);
void ski_game_draw(void);
void social_draw(void);
void pong_draw(void);
void intro_draw(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SCENES_H */

