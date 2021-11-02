

#include "FrameBuffer.h"
#include "SSD1306.h"
#include "font5x7.h"
#include <string.h>
#include <math.h>


uint8_t canvas[OLED_BYTES];

#if !defined(max)
 #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#if !defined(min)
 #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

void canvas_clearScreen() {
    for (uint8_t* ptr=canvas; ptr != &canvas[OLED_BYTES]; ++ptr)
        *ptr=0;
}

void canvas_drawPixel(int x, int y, bool on) {
    uint8_t b = y&7;
    if (on)
        canvas[x + (y/8)*OLED_WIDTH] |= (1<<b);
    else
        canvas[x + (y/8)*OLED_WIDTH] &= ~(1<<b);
}

void canvas_drawLine(int x1, int y1, int x2, int y2) {
    if (y1 == y2)
        canvas_drawHorizontalLine(x1, y1, x2);
    else if (x1 == x2)
        canvas_drawVerticalLine(x1, y1, y2);
    else {
        int dx = abs(x2-x1), sx = x1<x2?1:-1;
        int dy = abs(y2-y1), sy = y1<y2?1:-1;

        int err = (dx>dy?dx:-dy)/2, e2;
        int x=x1, y=y1;
        for(;;){
          if ((x >= 0) && (y >=0) && (x < OLED_WIDTH) && (y < OLED_HEIGHT)){
              uint8_t b = y&7;
              canvas[x + (y/8)*OLED_WIDTH] |= (1<<b);
          }
          if (x==x2 && y==y2) break;
          e2 = err;
          if (e2 >-dx) { err -= dy; x += sx; }
          if (e2 < dy) { err += dx; y += sy; }
        }
    }
}

void canvas_drawHorizontalLine(int x1, int y, int x2) {
    if ((y<0) || (y>=OLED_HEIGHT))
        return;
    int s1 = max(min(x1, x2), 0);
    int s2 = min(max(x1, x2), OLED_WIDTH-1);
    int m = 1 << (y&7);
    for (int i=s1; i<=s2; ++i)
        canvas[(y/8)*OLED_WIDTH+i] |= m;
}

void canvas_drawVerticalLine(int16_t x, int y1, int y2) {
    if ((x<0) || (x>=OLED_WIDTH))
        return;
    uint8_t mask;
    int s1 = max(min(y1, y2), 0);
    int s2 = min(max(y1, y2), OLED_HEIGHT-1);
    int h = s2-s1;
    uint8_t mod= 8-(s1&7);
    if (mod) {
        mask = ~(0xFF >> mod);
        if (h < mod)
          mask &= (0XFF >> (mod - h));
        canvas[x+(s1/8)*OLED_WIDTH] |= mask;
        s1 += mod;
    }
    while ((s1 +7) < s2) {
        canvas[x+(s1/8)*OLED_WIDTH] = 0xFF;
        s1+=8;
    }
    if (s1<s2) {
        mask = (1 << (s2&7)) - 1;
        canvas[x+(s1/8)*OLED_WIDTH] |= mask;
    }
}

void canvas_drawImage(int x, int y, int w, int h, const uint8_t *data) {
    for (int j=0; j<h; j+=8) {
        uint8_t *dst=&canvas[x+((y+j)/8)*OLED_WIDTH];
        uint8_t *src=&data[j/8*w];
        for (int i=0; i<w; ++i)
            *dst++=*src++;
    }
}

void canvas_drawImageSlow(int x, int y, int w, int h, const uint8_t *data) {
    int ys = y<16?16-y:0;
    int ye = ((y+h)>OLED_HEIGHT)?OLED_HEIGHT-y:h;
    int xs = x<0?-x:0;
    int xe = ((x+w)>OLED_WIDTH)?OLED_WIDTH-x:w;
    for (int j=ys; j<ye; j++) {
        for (int i=xs; i<xe; ++i) {
            uint8_t b = data[i + (j/8)*w];
            if ((b>> (j%8))&1)
                canvas_drawPixel(x+i, y+j, true);
        }
    }
}

void canvas_drawText(int x, int y, const char* text) {
    for (int i=0; i<strlen(text); ++i) {
        canvas_drawChar(x+i*6, y, text[i]);
    }
}

void canvas_drawChar(int x, int y, char c) {
    c -= 0x20;
    uint8_t xs = 0, xe=5;
    if (x<0) xs=-x;
    if (x>122) xe=127-x;
    if ((y%8)==0)
        for (int i=xs; i<xe; ++i) {
            canvas[x + i + (y/8)*OLED_WIDTH] = font5x7[c*5+i];
        }
    else {
        uint8_t dy = y%8;
        for (int i=xs; i<xe; ++i) {
            canvas[x + i + (y/8)*OLED_WIDTH + OLED_WIDTH] |= font5x7[c*5+i] >> (8-dy);
            canvas[x + i + (y/8)*OLED_WIDTH] |= font5x7[c*5+i] << dy;
        }
    }
}

void canvas_drawTitle(const char* text) {
    int l = strlen(text);
    canvas_drawText(64-l*3, 0, text);
}

void canvas_fillRect(int x, int y, int w, int h) {
    for (int i=0; i<w; ++i)
        canvas_drawVerticalLine(x+i,y, y+h-1);
}

void canvas_drawRect(int x, int y, int w, int h) {
    canvas_drawHorizontalLine(x, y, x+w-1);
    canvas_drawHorizontalLine(x, y+h-1, x+w-1);
    canvas_drawVerticalLine(x, y, y+h-1);
    canvas_drawVerticalLine(x+w-1, y, y+h-1);
}

void canvas_blt() {
    oled_draw_image(canvas);
}
