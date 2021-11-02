/* 
 * File:   SSD1306.h
 * Author: plum
 *
 * Created on September 3, 2021, 3:24 PM
 */

#ifndef SSD1306_H
#define	SSD1306_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define OLED_ADDRESS    0x3C
#define OLED_WIDTH      128
#define OLED_HEIGHT     64
#define OLED_BYTES      (OLED_WIDTH*OLED_HEIGHT/8)

void oled_init(void);
void oled_draw_image(uint8_t* img);
void oled_sleep(void);
void oled_wakeup(void);
void oled_invert(bool inverted);

#ifdef	__cplusplus
}
#endif

#endif	/* SSD1306_H */

