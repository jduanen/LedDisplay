/***************************************************************************
 *
 * Driver for using LedDisplay with the Adafruit_GFX Library
 * 
 ****************************************************************************/

#pragma once

#include <Adafruit_GFX.h>
#include "Render.h"


// for backwards client compatability
#define BLACK       BLACK_COLORLIGHTS     ///< Draw 'off' pixels
#define WHITE       AMBER_COLORLIGHTS     ///< Draw 'on' pixels


class LedDisplay: public Adafruit_GFX, public Render {
public:
    // NEW CONSTRUCTORS -- recommended for new projects
    LedDisplay(uint8_t bpp=BPP, uint8_t w=NUM_COLS, uint8_t h=NUM_ROWS);
    ~LedDisplay(void);

    void drawPixel(int16_t x, int16_t y, uint16_t color);

protected:
    uint8_t _bpp = BPP;  // Bits per pixel color for this display
};
