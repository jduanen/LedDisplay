/***************************************************************************
 *
 * LedDisplay Render Function for XIAO RP2040
 * 
 ****************************************************************************/

#include "LedDisplay_Render.h"


void LedDisplay_Render::setBrightness(byte percent) {
    // int <newvalue> = map(<value>, <original_min>, <original_max>, <new_min>, <new_max>);
    assert((percent >= 0) && (percent <= 100));
    ledsOnDelay = map(percent, 0, 100, MAX_LEDS_ON_DELAY, MIN_LEDS_ON_DELAY);
}

byte LedDisplay_Render::getBrightness() {
    // int <newvalue> = map(<value>, <original_min>, <original_max>, <new_min>, <new_max>);
    return map(ledsOnDelay, MAX_LEDS_ON_DELAY, MIN_LEDS_ON_DELAY, 0, 100);
}
