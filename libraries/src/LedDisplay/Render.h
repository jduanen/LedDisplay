/***************************************************************************
 *
 * LedDisplay Render Function for XIAO RP2040
 * 
 ****************************************************************************/

#pragma once

#include "Refresh.h"

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
