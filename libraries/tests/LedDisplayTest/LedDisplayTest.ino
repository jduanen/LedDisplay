/***************************************************************************
 *
 * Test program for low-level LedDisplay Render/Refresh library
 * 
 ****************************************************************************/

#include <Adafruit_GFX.h>
#include "Render.h"


#define STRIPES_PAT 0
#define LED_PAT     STRIPES_PAT
#define NUM_PATS    1


Render *ledDisplayPtr;

uint32_t loopCnt;
byte row = 0;
byte col = 0;
byte color;


void setup() {
	Serial.begin(115200);
    delay(500);
    Serial.println("\nBEGIN: Render Loop");
    ledDisplayPtr = new Render();
    Serial.println("READY: Render Loop");
};

void setup1() {
    // N.B. the delay values are necessary for correct function
    Serial.println("\nBEGIN: Refresh Loop");
    ledDisplayPtr->refreshInit();
    delay(500);
    Serial.print("READY: Refresh Loop; Brightness: " + String(ledDisplayPtr->getBrightness()));
}

void loop() {
    /*
    color = colors[loopCnt % sizeof(colors)];
	ledDisplayPtr->setPixel(row, col, color);
    ledDisplayPtr->show();

    row = (loopCnt % NUM_ROWS);
    col = (loopCnt % NUM_COLS);
    */

    switch (LED_PAT) {
    case STRIPES_PAT:
        if (loopCnt & 1) {
            for (col = 0; (col < NUM_COLS); col++) {
                ledDisplayPtr->setPixel(col, 0, RED_COLOR);
                ledDisplayPtr->setPixel(col, 4, RED_COLOR);
            }
            for (col = 0; (col < NUM_COLS); col++) {
                ledDisplayPtr->setPixel(col, 1, GREEN_COLOR);
                ledDisplayPtr->setPixel(col, 5, GREEN_COLOR);
            }
            for (col = 0; (col < NUM_COLS); col++) {
                ledDisplayPtr->setPixel(col, 2, AMBER_COLOR);
                ledDisplayPtr->setPixel(col, 6, AMBER_COLOR);
            }
            for (col = 0; (col < NUM_COLS); col++) {
                ledDisplayPtr->setPixel(col, 3, BLACK_COLOR);
            }
        } else {
            for (row = 0; (row < NUM_ROWS); row++) {
                for (col = 0; (col < NUM_COLS); col++) {
                    ledDisplayPtr->setPixel(col, row, BLACK_COLOR);
                }
            }
        }
        break;
    default:
        Serial.println("ERROR: invalid pattern");
        break;
    }
    ledDisplayPtr->show();

	delay(1000);
	loopCnt++;
};

void loop1() {
	ledDisplayPtr->refresh();
};
