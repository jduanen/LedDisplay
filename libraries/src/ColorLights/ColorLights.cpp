/***************************************************************************
 *
 * Driver for using LedDisplay with the Adafruit_GFX Library
 * 
 ****************************************************************************/

#include <Adafruit_GFX.h>
#include "ColorLights.h"


ColorLights::ColorLights(uint8_t w, uint8_t h): Adafruit_GFX(w, h), _bpp(bpp) {
    Serial.println("Create");
};

ColorLights::~ColorLights(void) {
    Serial.println("Destroy");
};

void ColorLights::drawPixel(int16_t x, int16_t y, uint16_t color) {
    _frameBuffers[_bufferNum][y][x] = color;
};
