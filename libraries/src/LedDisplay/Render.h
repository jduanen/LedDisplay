/***************************************************************************
 *
 * LedDisplay Render Function for XIAO RP2040
 * 
 ****************************************************************************/

#ifndef _LED_DISPLAY_RENDER_H_
#define _LED_DISPLAY_RENDER_H_

#include "Refresh.h"


#define BLACK_COLORLIGHTS   0b00000000
#define GREEN_COLORLIGHTS   0b00000001
#define RED_COLORLIGHTS     0b00000010
#define AMBER_COLORLIGHTS   (GREEN_COLOR | RED_COLOR)

#define NUM_BUFS            3

#define NONE                -1


class Render: Refresh {
public:
    void setPixel(byte col, byte row, byte color);
    void show();
protected:
    byte _frameBuffers[NUM_BUFS][NUM_ROWS][NUM_COLS] = {};
    ?
private:
    byte _bufferNum = NONE;
    ?
};

#endif // _LED_DISPLAY_RENDER_H_
