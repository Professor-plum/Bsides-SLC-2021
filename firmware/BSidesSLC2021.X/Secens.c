
#include <stdbool.h>
#include "mcc_generated_files/mcc.h"
#include "FrameBuffer.h"
#include "resources.h"
#include "SSD1306.h"
#include "main.h"
#include "IR.h"

uint16_t framecount;
uint16_t p1, p2;
uint8_t correct_count;

uint16_t getPot1() {
    uint16_t r = ADC_GetConversion(4);
    if (abs(r-p1) > 64) {
        p1=r;
        delaySleep();
    }
    return r;
}

uint16_t getPot2() {
    uint16_t r = ADC_GetConversion(5);
    if (abs(r-p2) > 64) {
        p2=r;
        delaySleep();
    }
    return r;
}

const uint8_t check_bits [] = {0x7e, 0xef, 0xdf, 0xbf, 0xcf, 0xf3, 0xfd, 0x7e,};
inline void checkSolved(uint8_t id) {
    if (isComplete(id))
        canvas_drawImage(120, 0, 8, 8, check_bits); 
}

void showSolved() {
    //canvas_drawText(28, 8, "!!!Solved!!!");
    canvas_blt();
    
    for (int i=0; i<3; ++i) {
        __delay_ms(100);
        oled_invert(true);
        __delay_ms(100);
        oled_invert(false);
        CLRWDT();
    }
}

void int2hex(uint16_t num, char* out) {
    const uint8_t tbl[] = "0123456789ABCDEF";
    bool start=false;
    for (int i=12; i>=0; i-=4) {
        uint8_t idx = (num >> i) & 0xF;
        if (start | idx | i==4) {
            *(out++) = tbl[idx];
            start=true;
        }
    }
    *out = '\0';
}

void int2dec(uint16_t num, char* out) {
    for (int i=1000; i>0; i/=10){
        *out++ = '0' + (num/i);
        num %= i;
    }
    *out = '\0';
}

/*const uint8_t etch_pat[] = {
    0x00, 0xFF, 0xFF, 0x00,
    0x00, 0xF0, 0x0F, 0x00,
    0x00, 0x0C, 0x30, 0x00,
    0x00, 0xF0, 0x0F, 0x00,
    0xFF, 0x00, 0x00, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF
};


bool etch_check() {
    
    uint16_t score = 0;
    for (int x=0; x<48; ++x)
        for (int y=2; y<8; y++) {
            if (canvas[x+y*128] == 0)
                score++;
            if (canvas[x+72+y*128] == 0)
                score++;
        }
    
    for (int x=0; x<4; ++x)
        for (int y=2; y<8; ++y) {
            uint8_t pat = etch_pat[x+(y-2)*4];
            for (int i=0; i<8; ++i){
                bool b = (pat >> (7-i)) & 1;
                uint8_t c = canvas[64+x*8+i+y*128];
                if (b && c>0)
                    score += 5;
                else if (!b && (c==0))
                    score++;
            }
        }
    char buf[10];
    int2hex(score, buf);
    canvas_drawText(0,0,buf);
    checkSolved(TASK_ETCH);
    return false;
}
 */

uint8_t etch_last[2];
void etch_init() {
    canvas_clearScreen();
    canvas_drawTitle("EtchASketch");
    checkSolved(TASK_ETCH);
     
    etch_last[0] = getPot1() / 8;
    etch_last[1] = 47-(getPot2() * 3 / 70);
}

void etch_draw() {
    uint16_t x = getPot1() >>3;
    uint16_t y = 47-(getPot2() * 3 / 70);
    
    canvas_drawLine(x, y+16, etch_last[0], etch_last[1]+16);
    if (shaken) {
        markComplete(TASK_ETCH);
        etch_init();
    }
    etch_last[0] = x;
    etch_last[1] = y;  
    canvas_blt();
}

void tone_draw() {
    uint16_t a1 = getPot1() *2;
    uint16_t a2 = getPot2() >> 2;
    
    uint16_t mult= a1 + a2 - 128;
    
    if ((mult > 0x500) && (mult < 0x566)) { //between 2500 and 2700
        if(++correct_count > 32) {
            markComplete(TASK_TONE);
        }
    }
    else
        correct_count=0;
    
    canvas_clearScreen();
    canvas_drawTitle("Captain Crunch");
    checkSolved(TASK_TONE);
    canvas_drawText(0, 8, "0ms");
    canvas_drawText(110, 8, "1ms");
    
    uint8_t lasty;
    for (int x=-1; x<128; ++x) {
        int idx = x;
        idx *= mult;
        idx >>=7;
        idx += framecount;
        idx %= 512;
        uint8_t y = 16 + sine_tbl[idx];
        if (x>=0) {
            if (y == lasty) {
                canvas_drawPixel(x, y, true);
            } else {
                canvas_drawVerticalLine(x, y, lasty);
            }
        }
        lasty=y;
    }
    
    canvas_blt();
    framecount++;
}

void digital_draw() {
    const char message[] = "L*4,R*5\0";
    uint8_t mlen = 8*8;
    
    uint16_t a1 =  getPot1() / 64;
    int a2 = getPot2() / 64;
    a2 -= 8;
    a1+=2;
    
    canvas_clearScreen();
    canvas_drawTitle("Logic Analyzer");
    checkSolved(TASK_DIGITAL);
    bool lastb;
    for (int16_t x=-1; x<128; ++x) {
        int idx = (x+framecount)/a1;
        idx %= mlen;
        uint8_t i = idx/8;
        uint8_t mod=idx%8;
        bool b = message[i] >> (7-(mod)) & 1;
        int x1=x;
        if (x1>=0) {
            if (b!=lastb) {
                canvas_drawVerticalLine(x1, 17, 63);
            } else {
                canvas_drawPixel(x1, b?17:63, true);
            } 
        }
        lastb=b;
    }
    canvas_blt();
    framecount-=a2;
}

void temp_draw() {
    //FVRCONbits.TSEN = 1; // enable temp
    
    if ((framecount++ % 256 )== 0) {
        canvas_clearScreen();
        canvas_drawTitle("Hit the Slopes");
        checkSolved(TASK_TEMP);
        uint16_t temp=0, v1=0;
        const uint8_t samples=64;

        for (int i=0; i<samples; ++i) {
            ADC_SelectChannel(channel_Temp);
            ADC_TemperatureAcquisitionDelay();
            temp += ADC_GetConversion(channel_Temp);
            v1 += ADC_GetConversion(channel_FVR);
        }
        temp /= samples;
        v1 /= samples;
        //uint32_t regu = ((uint32_t)2048000 / v1) ;
        int32_t toff = ((int32_t)-613*v1/1000) + 1017 -552;

        int16_t tt = ((temp-toff)*184/128)-725;
        /*
         * char num[5];
        int2dec(temp, num);

        char num2[5];
        int2dec(v1, num2);
        
        char num3[5];
        int2dec(temp-toff, num3);
        canvas_drawText(0,32, num);
        canvas_drawText(0,40, num2);
        canvas_drawText(0,48, num3);
         */

        if (tt < 50) {
            if(++correct_count > 32) {
                markComplete(TASK_TEMP);
            }
        }
        else
            correct_count=0;

        if (isComplete(TASK_TEMP)) {
            canvas_drawImage(32, 24, 25, 32, number_bits[tt/10]);
            canvas_drawImage(64, 24, 25, 32, number_bits[tt%10]);
            canvas_drawImage(96, 24, 16, 16, deg_bits);
        }
        else {
            canvas_drawImage(32, 24, 25, 32, q_bits);
            canvas_drawImage(64, 24, 25, 32, q_bits); 
        }
        canvas_blt();
    }
    
}

void static_draw() {
    const uint32_t tvcodes[] = {
        0x1FE02287,
        0x28D7F708,
        0x3FC02B00,
        0xA55AEE00,
        0xAD52DF20,
        0xB9467D02,
        0xBB448F80,
        0xBE412287,
        0xDF20E084,
        0xE31C55AA,
        0xE31C9900,
        0xE718CF30,
        0xEA15FB0C,
        0xED12BF40,
        0xED12EF10,
        0xF00F0586,
        0xF30CBD40,
        0xF30CF700,
        0xF30CFB04,
        0xF30CFF00,
        0xF50A7F00,
        0xF708E718,
        0xF708FB04,
        0xF807DF20,
        0xFA05F708,
        0xFA05FE01,
        0xFC039F60,
        0xFE01BD00,
        0xFE01FFFF,
        0xFF00DD20,
    };
    
    canvas_clearScreen();
    canvas_drawTitle("Turn it off");
    checkSolved(TASK_TV);
    
    uint32_t msg = IrGetMessage();    
    if (msg) {
        for (int i=0; i<30; ++i)
            if (msg == tvcodes[i]) {
                markComplete(TASK_TV);
            }
        framecount = 0;
    }
    
    if (!isComplete(TASK_TV)) {
        uint8_t *ptr=&canvas[256];
        for (int i=0; i<768; ++i) 
            *ptr++ = rnd();
    }
    else {
        if (framecount == 0)
            canvas_drawHorizontalLine(0,32,127);
        else if (framecount == 1)
            canvas_drawHorizontalLine(32,32,96);
        else if (framecount <4)
            canvas_drawPixel(64,32,true);
    }
    framecount++;
    canvas_blt();
}


void matrix_draw() {
    //dktx jnkpw var zoaoo oeqaddx
    const char message[8][3] = {
        {'D', 'K', 'T'},
        {'X', 'J', 'N'},
        {'K', 'P', 'W'},
        {'V', 'A', 'R'},
        {'Z', 'O', 'A'},
        {'O', 'O', 'O'},
        {'E', 'G', 'A'},
        {'D', 'D', 'X'}
    };
    const uint8_t xoffsets[] = {1, 15, 35, 50, 64, 77, 95, 113};
    const uint8_t yoffsets[] = {0, 58, 13, 33, 82, 25, 73, 47};
    canvas_clearScreen();
    for (int i=0; i<8; ++i) {
        uint8_t y = (yoffsets[i] + (framecount++)/4) % 96;
        uint8_t x = xoffsets[i];
        uint8_t m = xoffsets[i]%24;
        for (uint8_t j=0; j<3; ++j)
        {
            uint16_t mark = j*8+m;
            if ((y<(48+mark)) && (y>(4+mark)))
                canvas_drawChar(x, 16+mark, message[i][j]);
        }
        if (y<48)
            canvas_drawChar(x, y+16, 32 + rnd()%96);
    }
    for (uint8_t i=0; i<128; ++i)
        canvas[16+i] = 0;
    canvas_drawTitle("Matrix");
    if ((getPot1()/64) == 3)
        canvas_drawText(19, 8, "Hint: BUMNYHILL");
    checkSolved(TASK_MATRIX);
    canvas_blt();
}

void clock_draw() {
    
    canvas_clearScreen();
    canvas_drawTitle("Clock");
    checkSolved(TASK_CLOCK);
    canvas_drawImage(40, 16, 48, 48, clock_bits);
    
    uint16_t h1, h2;
    int x,y;
    
    h1 = (getPot1()/2);
    x = sine_tbl[h1]*2/3;
    y = sine_tbl[(h1+128)%512]*2/3;
    y = 55-y;
    x+=48;
    canvas_drawLine(64,40, x, y);
    
    h2 = (getPot2()/2);
    x = sine_tbl[h2] + 40;
    y = 63 - sine_tbl[(h2+128)%512];
    canvas_drawLine(64,40, x, y);
    canvas_blt();
    
    // 9:40
    if ((h1 > 390) && (h1 < 422) && (h2 > 333) && (h2 < 350)) {
        if(++correct_count > 32) {
            markComplete(TASK_CLOCK);
        }
    }
    else
        correct_count=0;
}

void radio_draw() {
    uint16_t h = getPot1() * 2 / 13;
    uint8_t x;
    char line[10] = " 00.0MHz";
    
    h += (getPot2()/32);
    
    uint16_t freq = h + 884;
    if (freq >= 1000)
        line[0] = '1';
    line[1] += (freq%1000)/100;
    line[2] += (freq%100)/10;
    line[4] += (freq%10);
    
    if (freq == 923) {
        if(++correct_count > 32) {
            markComplete(TASK_RADIO);
        }
    }
    else
        correct_count=0;
    
    canvas_clearScreen();
    canvas_drawTitle("Tower Station");
    checkSolved(TASK_RADIO);
    canvas_drawHorizontalLine(0, 31, 128);
    canvas_drawHorizontalLine(0, 48, 128);
    
    for (uint8_t i=0; i<20; ++i) {
        x = i*32/5;
        canvas_drawPixel(x, 32, true);
        canvas_drawPixel(x, 47, true);
    }
    x = h * 2 / 3;
    canvas_drawVerticalLine(x-1, 32, 48);
    canvas_drawVerticalLine(x, 32, 48);
    canvas_drawVerticalLine(x+1, 32, 48);
    
    canvas_drawText(36, 56, line);
    canvas_blt();
}

uint8_t eql_rows[8] = {5,7,7,5,3,5,2,3};
const uint8_t eql_solve[8] = {8,5,4,2,6,5,5,8};
void equal_draw() {
    const uint8_t pat[8] = {1,3,3,2,1,0,0,0};
    canvas_clearScreen();
    canvas_drawTitle("Equalizer");
    checkSolved(TASK_EQUAL);
    canvas_drawImage(0, 8, 128, 8, equal_bits);
    
    uint8_t idx = getPot1()/114;
    if (idx<8)
        eql_rows[idx] = 1 + getPot2()/114;
    
    bool correct=true;
    for (uint8_t i=0; i<8; ++i)
        if (eql_rows[i] != eql_solve[i]) {
            correct=false;
            break;
        }
    if (correct) {
        markComplete(TASK_EQUAL);
    }
    
    for (uint8_t i=0; i<8; ++i) {
        int h;
        if (i == idx)
            h = eql_rows[i];
        else
            h = pat[framecount%8] + eql_rows[i];
        uint8_t y = 7;
        while (h>0) {
            uint8_t b = 0x30, c = 0x30;
            if (h>1) {
                b = c = 0x33;
            }
            for (int j=0; j<7;++j) {
                canvas[j+i*16 + y*128] = b;
                canvas[j+i*16 +8 + y*128] = c;
            }
            h-=2;
            y--;
        }
    }
    framecount++;
    canvas_blt();
}

void slider_draw() {
    const char ct[] = "7890123";
    char v1[]="-79";
    char v2[]=" 00";
    uint16_t p1 = getPot1()/16;
    uint16_t p2 = getPot2()/16;
    
    v1[1] -= p1/10;
    v1[2] -= p1%10;
    v2[0] = p2>=30?'1':' ';
    v2[1] = ct[p2/10];
    v2[2] += p2%10;
    
    if ((p1 == 39) && (p2 == 55)) { //40 - 125
        if(++correct_count > 32) {
            markComplete(TASK_SLIDER);
        }
    }
    else
        correct_count=0;
    
    canvas_clearScreen();
    canvas_drawTitle("U3 Ta");
    checkSolved(TASK_SLIDER);
    
    
    for (uint8_t i=30; i<35; ++i)
        canvas_drawHorizontalLine(p1, i, 64+p2);
    
    canvas_drawVerticalLine(p1, 28, 37);
    canvas_drawVerticalLine(p2+64, 28, 37);
    canvas_drawText(0, 40, v1);
    canvas_drawText(110, 40, v2);
    canvas_blt();
}

extern uint32_t *social_messages;
void social_draw() {
    canvas_clearScreen();
    canvas_drawTitle("Social Retreat");
    checkSolved(TASK_SOCIAL);
    
    for (uint8_t i=0; i<8; ++i)
        if (isComplete(i+SOCIAL_BITS))
            canvas_drawImage(i*16, 16, 16, 48, art_strips[i]);
    
    uint32_t msg = IrGetMessage();
    if (msg && ((msg &0xFF) == IR_ADDR)) {
        uint8_t m = (msg >> 16);
        if ((m>=SOCIAL_BITS) && (m<(SOCIAL_BITS+8))) {
            uint8_t i = m - SOCIAL_BITS;
            if (!isComplete(m)) 
                markComplete(m);
            canvas_fillRect(i*16, 16, 16, 48);
            bool complete = (game_state >> 16) == 0xff;
            if (complete) {
                markComplete(TASK_SOCIAL);
            }
        }
    }
    
    
    canvas_blt();
}

int16_t treesx[16] = {};
uint8_t treesy[16] = {};
uint16_t ski_score;
bool ski_crashed;
uint8_t ski_x, ski_y;

void ski_game_init() {
    for (int i=0; i<16; ++i) {
        treesx[i] = i*16 + (rnd() % 16) + 120;
        treesy[i] = 8*(rnd() % 6) + 16;
    }
    ski_score = 0;
    ski_crashed = false;
}

void ski_game_draw() {
    char sc[12] = "Score:  00";
    const uint8_t fc[] = {0, 1, 2, 2};
    
    if (!ski_crashed) {
        canvas_clearScreen();
        checkSolved(TASK_SKI);

        uint8_t sx = getPot1()/16;
        uint8_t sy = 56 - (getPot2()/26);

        for (int i=0; i<16; ++i) {
            int16_t dx = treesx[i];
            treesx[i]-=3;
            uint8_t dy = treesy[i];
            if ((dx>-5) && (dx<128))
                canvas_drawChar(dx, dy, 128);
            else if(dx<=-8) {
                treesx[i] = 248 + (rnd() % 16);
                treesy[i] = 8*(rnd() % 6) + 16;
            }
            if ((sx > (dx-3)) && (sx < (dx+3)) && (sy > (dy-5)) && (sy < (dy+5))) {
                ski_crashed = true;
                ski_x = sx;
                ski_y = sy;
            }
        }
    
        if (ski_crashed)
            canvas_drawChar(ski_x, ski_y, 132);
        else
            canvas_drawChar(sx, sy, 129 + fc[(framecount/4) %4]);
        if (ski_score > 1000)
            sc[7] = '0' + ski_score/1000;
        sc[8] += (ski_score%1000)/100;
        sc[9] += (ski_score%100)/10;
        //sc[10] += ski_score%10;
        canvas_drawTitle("Ski game");
        canvas_drawText(9,8, sc);
        canvas_blt();
        
        if ((ski_score > 1000) && !isComplete(TASK_SKI)) {
            markComplete(TASK_SKI);
        }
        
        framecount++;
        ski_score++;
    }
}


void lifts_draw() {
    static int lift_idx[2]={24, 64};
    static uint8_t lift_p[2] = {1, 0};
    static int rift_idx[2]={6, 48};
    canvas_clearScreen();
    canvas_drawTitle("Ski Lifts");
    checkSolved(TASK_LIFTS);
    
    if (isComplete(TASK_LIFTS)) {
        canvas_drawLine(42, 16, 127, 58);
        for (int i=0; i<2; ++i) {
            int idx = lift_idx[i];
            if (idx>3)
                canvas_drawVerticalLine(33+idx*2, 12+idx, 18+idx);
            else if (idx>0)
                canvas_drawVerticalLine(33+idx*2, 16, 18+idx);
            canvas_drawImageSlow(22+idx*2, 18+idx, 20, 16, lifts_bits[lift_p[i]]);
            if (--lift_idx[i] < -16) {
                lift_idx[i] = 64;
                lift_p[i] = rnd() % 3;
            }
        }
        canvas_drawLine(0, 20, 87, 63);
        for (int i=0; i<2; ++i) {
            int idx = rift_idx[i];
            if (idx>3)
                canvas_drawVerticalLine(-17+idx*2, 12+idx, 16+idx);
            canvas_drawImageSlow(-25+idx*2, 16+idx, 20, 16, lifts_bits[3]);
            if (++rift_idx[i] > 64)
                rift_idx[i] = -16;
        }
    }
    else {
        if (testLogic())
            markComplete(TASK_LIFTS);
        canvas_drawText(55, 24, "Out");
        canvas_drawText(58, 32, "of");
        canvas_drawText(49, 40, "order");
        canvas_drawHorizontalLine(46, 20, 81);
        canvas_drawHorizontalLine(46, 52, 81);
        canvas_drawVerticalLine(45, 21, 52);
        canvas_drawVerticalLine(82, 21, 52);
    }
    canvas_blt();
}

void assem_draw() {
    const char* assem[6] = {
        "31 C0 89 C7 BF 1C E5",
        "1D B5 66 81 CA FF 0F",
        "42 60 8D 5A 04 B0 21", 
        "CD 80 3C    61 74 ED",
        "39 3A 75 EE 39 7A 04",
        "75 E9 83 C2 08 FF E2"
    };
    //F2
    
    char val[3]={0};
    uint8_t v = ((getPot1()/64) << 4) | (getPot2()/64);
    canvas_clearScreen();
    canvas_drawTitle("Egg Hunter");
    checkSolved(TASK_ASSEM);
    for (uint8_t i=0; i<6; ++i) {
        canvas_drawText(0, 16+i*8, assem[i]);
    }
    int2hex(v, val);
    canvas_drawText(54, 40, val);
    canvas_blt();
    
    if (v == 0xF2) {
        if(++correct_count > 32) {
            markComplete(TASK_ASSEM);
        }
    }
    else
        correct_count=0;
    
}

void pong_draw() {
    uint8_t y1 = getPot1()/18;
    uint8_t y2 = 56-getPot2()/18;
    static int px, py, vx=1, vy=1;
    static uint8_t p1_score=0, p2_score=0;
    static uint8_t hits=0;
    const uint8_t psize = 8;
    
    px += vx;
    py += vy;
    
    if (py == 0)
        vy=1;
    if (py == 63)
        vy=-1;
    
    if (px == 5) {
        if ((y1 <= py) && (y1+psize > py)) {
            vx=1;
            hits++;
        }
    }
    if (px == 122) {
        if ((y2 <= py) && (y2+psize > py)) {
            vx=-1;
            hits++;
        }
    }
    
    if (px < 0) {
        px = 64;
        py = 32;
        p2_score++;
        hits=0;
        vx = (rnd()%2)?1:-1;
    }
    
    if (px > 127) {
        px = 64;
        py = 32;
        p1_score++;
        hits=0;
        vx = (rnd()%2)?1:-1;
    }
    
    if ((p1_score >= 10) || (p2_score >= 10)) {
        p1_score = p2_score = 0;
    }
    
    if (hits >9)
        markComplete(TASK_PONG);
    canvas_clearScreen();
    
    if (isComplete(TASK_PONG))
        canvas_drawImage(60, 0, 8, 8, check_bits); 
    
    canvas_drawChar(48, 0, p1_score + 0x30);
    canvas_drawChar(74, 0, p2_score + 0x30);
    canvas_drawVerticalLine(4, y1, y1+psize);
    canvas_drawVerticalLine(123, y2, y2+psize);
    canvas_drawPixel(px, py, true);
    canvas_blt();
}