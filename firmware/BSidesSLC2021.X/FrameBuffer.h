/* 
 * File:   FrameBuffer.h
 * Author: plum
 *
 * Created on September 3, 2021, 4:38 PM
 */

#ifndef FRAMEBUFFER_H
#define	FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
extern uint8_t canvas[];
  
void canvas_clearScreen(void);

void canvas_drawPixel(int x, int y, bool on);

void canvas_drawLine(int x1, int y1, int x2, int y2);
void canvas_drawHorizontalLine(int x1, int y, int x2);
void canvas_drawVerticalLine(int16_t x, int y1, int y2);

//Y location and height must be a multiple of 8
void canvas_drawImage(int x, int y, int w, int h, const uint8_t *data);

void canvas_drawImageSlow(int x, int y, int w, int h, const uint8_t *data);
void canvas_drawText(int x, int y, const char* text);
void canvas_drawChar(int x, int y, char c);
void canvas_drawTitle(const char* text);

void canvas_fillRect(int x, int y, int w, int h);
void canvas_drawRect(int x, int y, int w, int h);

void canvas_blt(void);


#ifdef	__cplusplus
}
#endif

#endif	/* FRAMEBUFFER_H */

