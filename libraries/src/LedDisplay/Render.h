/***************************************************************************
 *
 * LedDisplay Render Function for XIAO RP2040
 * 
 ****************************************************************************/

#ifndef _LED_DISPLAY_RENDER_H_
#define _LED_DISPLAY_RENDER_H_

#include "Refresh.h"

//// TMP TMP TMP
#define NUM_BUFS            3
#define NUM_COLS            ((8 + 8 + 1) * 5)
#define NUM_ROWS            7

#define BLACK_COLORLIGHTS   0b00000000
#define GREEN_COLORLIGHTS   0b00000001
#define RED_COLORLIGHTS     0b00000010
#define AMBER_COLORLIGHTS   (GREEN_COLOR | RED_COLOR)


class Render: public Refresh {
public:
    Render();

    void setPixel(uint8_t col, uint8_t row, uint8_t color);
    void show();

private:
    uint8_t _renderBufNum = -1;
};


#include "Render.hpp"

#endif // _LED_DISPLAY_RENDER_H_
