/***************************************************************************
 *
 * LedDisplay Render Function for XIAO RP2040
 * 
 ****************************************************************************/

#ifndef _LED_DISPLAY_RENDER_H_
#define _LED_DISPLAY_RENDER_H_

#define BLACK_COLORLIGHTS   0b00000000
#define GREEN_COLORLIGHTS   0b00000001
#define RED_COLORLIGHTS     0b00000010
#define AMBER_COLORLIGHTS   (GREEN_COLOR | RED_COLOR)

#define NUM_BUFS            3

#define NONE                -1


class LedDisplay_Render: public {
public:
    void show();
    void refresh();
    byte getBrightness();
    void setBrightness(byte percent);

    byte _frameBuffers[NUM_BUFS][NUM_ROWS][NUM_COLS] = {};
    byte _bufferNum = NONE;
protected:
    ?
private:
    ?
};

#endif // _LED_DISPLAY_RENDER_H_
