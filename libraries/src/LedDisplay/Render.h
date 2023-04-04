/***************************************************************************
 *
 * LedDisplay Render Function for XIAO RP2040
 * 
 ****************************************************************************/

#pragma once

#include "Refresh.h"


class Render: public Refresh {
public:
    Render();

    void setPixel(uint8_t col, uint8_t row, uint8_t color);
    void show();

private:
    uint8_t _renderBufNum = -1;
};
