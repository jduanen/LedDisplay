/***************************************************************************
 *
 * Driver for using LedDisplay with the Adafruit_GFX Library
 * 
 ****************************************************************************/

#ifndef _ColorLights_H_
#define _ColorLights_H_

#include <Adafruit_GFX.h>

#include "LedDisplay_Render.h"


#define NUM_COLS            ((8 + 8 + 1) * 5)
#define NUM_ROWS            7

// for backwards client compatability
#define BLACK               BLACK_COLORLIGHTS     ///< Draw 'off' pixels
#define WHITE               AMBER_COLORLIGHTS     ///< Draw 'on' pixels


class ColorLights: public Adafruit_GFX, LedDisplay_Render {
public:
    // NEW CONSTRUCTORS -- recommended for new projects
    ColorLights(uint8_t w=NUM_ROWS, uint8_t h=NUM_COLS);

    ~ColorLights(void);

    void drawPixel(int16_t x, int16_t y, uint16_t color);

protected:
    uint8_t _bpp = 2;  // Bits per pixel color for this display
};

#endif // _ColorLights_H_
