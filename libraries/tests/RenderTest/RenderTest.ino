/***************************************************************************
 *
 * LedDisplay Render Function Test Program for XIAO RP2040
 * 
 ****************************************************************************/

#include "Render.h"


#define PIXEL_DEPTH         2

#define BLACK_COLOR         0b00000000
#define GREEN_COLOR         0b00000001
#define RED_COLOR           0b00000010
#define AMBER_COLOR         (GREEN_COLOR | RED_COLOR)

#define NUM_COLORS          4
#define NUM_LED_COLORS      2

#define ALL_CLEAR_PATTERN   0
#define ALL_RED_PATTERN     1
#define ALL_GREEN_PATTERN   2
#define ALL_AMBER_PATTERN   3
#define STRIPES_PATTERN     4
#define CHECK_PATTERN       5
#define NUM_PATTERNS        6

#define NONE                -1


uint32_t renderBufNum = NONE;

uint32_t loopCnt = 0;
uint32_t loop1Cnt = 0;


Render *renderPtr;


uint32_t row;
uint32_t col;
uint32_t color;

void writePattern(uint32_t pattNum) {
    uint32_t i;

    row = 0;
    col = 0;
    color = BLACK_COLOR;

    //// assert(pattNum < NUM_PATTERNS);
    switch (pattNum) {
    case ALL_CLEAR_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                renderPtr->setPixel(col, row, BLACK_COLOR);
            }
        }
        break;
    case ALL_RED_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                renderPtr->setPixel(col, row, RED_COLOR);
            }
        }
        break;
    case ALL_GREEN_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                renderPtr->setPixel(col, row, GREEN_COLOR);
            }
        }
        break;
    case ALL_AMBER_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                renderPtr->setPixel(col, row, AMBER_COLOR);
            }
        }
        break;
    case STRIPES_PATTERN:
        for (col = 0; (col < NUM_COLS); col++) {
            renderPtr->setPixel(col, 0, RED_COLOR);
            renderPtr->setPixel(col, 4, RED_COLOR);
        }
        for (col = 0; (col < NUM_COLS); col++) {
            renderPtr->setPixel(col, 1, GREEN_COLOR);
            renderPtr->setPixel(col, 5, GREEN_COLOR);
        }
        for (col = 0; (col < NUM_COLS); col++) {
            renderPtr->setPixel(col, 2, AMBER_COLOR);
            renderPtr->setPixel(col, 6, AMBER_COLOR);
        }
        for (col = 0; (col < NUM_COLS); col++) {
            renderPtr->setPixel(col, 3, BLACK_COLOR);
        }
        break;
    case CHECK_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                renderPtr->setPixel(col, row, ((col + (row % NUM_COLORS)) % NUM_COLORS));
            }
        }
        break;
    default:
        Serial.println("ERROR: unknown pattern number (" + String(pattNum) + ")");
        break;
    }
};


void setup() {
    // N.B. the delay values are necessary for correct function
    Serial.begin(115200);
    delay(500); // TMP TMP TMP
    Serial.println("\nBEGIN: Render Loop");
    renderPtr = new Render();
    Serial.println("READY: Render Loop");
};

void setup1() {
    Serial.println("\nBEGIN: Refresh Loop");
    renderPtr->refreshInit();
    delay(500);
    Serial.println("Brightness: " + String(renderPtr->getBrightness()) + "%");
    Serial.print("READY: Refresh Loop; ");
};


#define FIXED_PATTERN       0
#define CYCLE_PATTERNS      1
#define CYCLE_BRIGHTNESS    2
#define RAMP_BRIGHTNESS     3

//#define TEST_MODE           RAMP_BRIGHTNESS
//#define TEST_MODE           CYCLE_BRIGHTNESS
//#define TEST_MODE           FIXED_PATTERN
#define TEST_MODE           CYCLE_PATTERNS

uint32_t pNum = 0;

// Render loop
void loop() {
    uint32_t pattern;
    uint32_t numLoops;
    uint32_t brightness;

    switch (TEST_MODE) {
    case FIXED_PATTERN:
        // at the start, fill one buf with chosen pattern
        pattern = ALL_AMBER_PATTERN; // CHECK_PATTERN
        if (loopCnt == 0) {
            writePattern(pattern);
            renderPtr->show();
        }
        break;
    case CYCLE_PATTERNS:
        numLoops = 3;
        if ((loopCnt % numLoops) == 0) {
            pNum = ((pNum + 1) % NUM_PATTERNS);
            writePattern(pNum);
            renderPtr->show();
            Serial.println("Pattern #: " + String(pNum));
        }
        break;
    default:
        // no pattern
        break;
    }

    loopCnt++;
    delay(100);
}

// Refresh loop
void loop1() {
    renderPtr->refresh();
};
