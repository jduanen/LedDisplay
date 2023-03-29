/***************************************************************************
 *
 * LedDisplay Refresh Function for XIAO RP2040
 * 
 ****************************************************************************/

#ifndef _LED_DISPLAY_REFRESH_H_
#define _LED_DISPLAY_REFRESH_H_


#define DEF_BRIGHTNESS      50  // 50% brightness

#define DEF_ROW_0           D0
#define DEF_ROW_1           D1
#define DEF_ROW_2           D2
#define DEF_GREENS_ENB      D3
#define DEF_REDS_ENB        D4

#define DEF_COL_DATA        D5
#define DEF_COL_STROBE      D6
#define DEF_COL_CLOCK       D7

#define NUM_BUFS            3
#define NUM_COLS            ((8 + 8 + 1) * 5)
#define NUM_ROWS            7

#define ROW_DELAY           1000    // 1 msec (1KHz refresh rate)
#define INTER_ROW_DELAY     (ROW_DELAY - _ledsOnDelay)

#define MAX_LEDS_ON_DELAY   0                 //  0 msec (0% brightness)
#define DEF_LEDS_ON_DELAY   (ROW_DELAY / 2)   // .5 msec (50% brightness)
#define MIN_LEDS_ON_DELAY   ROW_DELAY         //  1 msec (100% brightness)

#define BLACK_COLOR         0b00000000
#define GREEN_COLOR         0b00000001
#define RED_COLOR           0b00000010
#define AMBER_COLOR         (GREEN_COLOR | RED_COLOR)

#define NUM_COLORS          4
#define NUM_LED_COLORS      2


uint8_t colors[] = {GREEN_COLOR, RED_COLOR};


class Refresh {
  public:
    typedef uint8_t (*frameBuffersPtr)[NUM_BUFS][NUM_ROWS][NUM_COLS];

    Refresh();
    Refresh(uint8_t row0, uint8_t row1, uint8_t row2,
            uint8_t greenLedsEnable, uint8_t redLedsEnable,
            uint8_t columnData, uint8_t columnStrobe, uint8_t columnClock);
    ~Refresh();

    void refresh();

    uint8_t getBrightness();
    void setBrightness(byte percent);
    frameBuffersPtr getFrameBuffersPtr();

  protected:
    uint8_t _frameBuffers[NUM_BUFS][NUM_ROWS][NUM_COLS] = {};
    uint32_t _ledsOnDelay = DEF_LEDS_ON_DELAY;
    uint8_t _curRow = 0;
    uint8_t _refreshBufNum = -1;  // unallocated
    uint8_t _row0 = DEF_ROW_0;
    uint8_t _row1 = DEF_ROW_1;
    uint8_t _row2 = DEF_ROW_2;
    uint8_t _greensEnable = DEF_GREENS_ENB;
    uint8_t _redsEnable = DEF_REDS_ENB;
    uint8_t _columnData = DEF_COL_DATA;
    uint8_t _columnStrobe = DEF_COL_STROBE;
    uint8_t _columnClock = DEF_COL_CLOCK;

    void init();
    void shiftInPixels(int bufNum, int row, int color);
    void disableRows();
    void enableRow(int rowColor, int rowNum);
};

#include "Refresh.hpp"


#endif // _LED_DISPLAY_REFRESH_H_
