/***************************************************************************
 *
 * Driver for using LedDisplay with the Adafruit_GFX Library
 * 
 ****************************************************************************/

#ifndef _ColorLights_H_
#define _ColorLights_H_

#include <Adafruit_GFX.h>
#include "Render.h"


// for backwards client compatability
#define BLACK               BLACK_COLORLIGHTS     ///< Draw 'off' pixels
#define WHITE               AMBER_COLORLIGHTS     ///< Draw 'on' pixels


class ColorLights: public Adafruit_GFX, Render {
public:
    // NEW CONSTRUCTORS -- recommended for new projects
    ColorLights(uint8_t w=NUM_ROWS, uint8_t h=NUM_COLS);
    ~ColorLights(void);

protected:
    uint8_t _bpp = 2;  // Bits per pixel color for this display
};


#include "ColorLights.hpp"

#endif // _ColorLights_H_
