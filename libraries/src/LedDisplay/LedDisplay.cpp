/***************************************************************************
 *
 * Driver for using LedDisplay with the Adafruit_GFX Library
 * 
 ****************************************************************************/

#include <Arduino.h>
#include "LedDisplay.h"


LedDisplay::LedDisplay(uint8_t bpp, uint8_t w, uint8_t h): Adafruit_GFX(w, h), _bpp(bpp) {
    Serial.println("Create");
};

LedDisplay::~LedDisplay(void) {
    Serial.println("Destroy");
};

void LedDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
    Serial.println("drawPixel");
};
