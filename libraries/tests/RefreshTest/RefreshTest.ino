/***************************************************************************
 *
 * LedDisplay Refresh Function Test Program for XIAO RP2040
 * 
 ****************************************************************************/

#include "Refresh.h"


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


Refresh *refreshPtr;


uint32_t row;
uint32_t col;
uint32_t color;

void writePattern(uint32_t bufNum, uint32_t pattNum) {
    uint32_t i;

    row = 0;
    col = 0;
    color = BLACK_COLOR;

    //// assert(pattNum < NUM_PATTERNS);
    switch (pattNum) {
    case ALL_CLEAR_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                refreshPtr->_frameBuffers[bufNum][row][col] = 0;
            }
        }
        break;
    case ALL_RED_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                refreshPtr->_frameBuffers[bufNum][row][col] = RED_COLOR;
            }
        }
        break;
    case ALL_GREEN_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                refreshPtr->_frameBuffers[bufNum][row][col] = GREEN_COLOR;
            }
        }
        break;
    case ALL_AMBER_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                refreshPtr->_frameBuffers[bufNum][row][col] = AMBER_COLOR;
            }
        }
        break;
    case STRIPES_PATTERN:
        for (col = 0; (col < NUM_COLS); col++) {
            refreshPtr->_frameBuffers[bufNum][0][col] = RED_COLOR;
            refreshPtr->_frameBuffers[bufNum][4][col] = RED_COLOR;
        }
        for (col = 0; (col < NUM_COLS); col++) {
            refreshPtr->_frameBuffers[bufNum][1][col] = GREEN_COLOR;
            refreshPtr->_frameBuffers[bufNum][5][col] = GREEN_COLOR;
        }
        for (col = 0; (col < NUM_COLS); col++) {
            refreshPtr->_frameBuffers[bufNum][2][col] = AMBER_COLOR;
            refreshPtr->_frameBuffers[bufNum][6][col] = AMBER_COLOR;
        }
        for (col = 0; (col < NUM_COLS); col++) {
            refreshPtr->_frameBuffers[bufNum][3][col] = 0;
        }
        break;
    case CHECK_PATTERN:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                refreshPtr->_frameBuffers[bufNum][row][col] = ((col + (row % NUM_COLORS)) % NUM_COLORS);
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
    delay(500);
    Serial.println("\nBEGIN: Render Loop");
    refreshPtr = new Refresh();
    Serial.println("READY: Render Loop");
};

void setup1() {
    Serial.println("\nBEGIN: Refresh Loop");
    refreshPtr->refreshInit();
    delay(1000);//300);  //// FIXME find why this is necessary
    Serial.print("READY: Refresh Loop; ");
    Serial.println("Brightness: " + String(refreshPtr->getBrightness()) + "%");
};


#define FIXED_PATTERN       0
#define CYCLE_PATTERNS      1
#define CYCLE_BRIGHTNESS    2
#define RAMP_BRIGHTNESS     3
#define ROW_PATTERNS        4
#define COLUMN_PATTERNS     5

//#define TEST_MODE           RAMP_BRIGHTNESS
//#define TEST_MODE           CYCLE_BRIGHTNESS
//#define TEST_MODE           FIXED_PATTERN
//#define TEST_MODE           CYCLE_PATTERNS
//#define TEST_MODE           ROW_PATTERNS
//#define TEST_MODE           COLUMN_PATTERNS
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
            if (rp2040.fifo.pop_nb(&renderBufNum)) {
                writePattern(renderBufNum, pattern);
                rp2040.fifo.push(renderBufNum);
            }
        }
        break;
    case CYCLE_PATTERNS:
        numLoops = 3;
        if ((loopCnt % numLoops) == 0) {
            if (rp2040.fifo.pop_nb(&renderBufNum)) {
                pNum = ((pNum + 1) % NUM_PATTERNS);
                writePattern(renderBufNum, pNum);
                rp2040.fifo.push(renderBufNum);
//                Serial.println("Pattern #: " + String(pNum));
            }
        }
        break;
    case CYCLE_BRIGHTNESS:
        numLoops = 10;
        if ((loopCnt % numLoops) == 0) {
            if (!rp2040.fifo.pop_nb(&renderBufNum)) {
                break;
            }
            pattern = ALL_AMBER_PATTERN;
            switch (refreshPtr->getBrightness()) {
            case 0:
                brightness = 22;
                break;
            case 25:
                brightness = 50;
                break;
            case 50:
                brightness = 75;
                break;
            case 75:
                brightness = 100;
                break;
            case 100:
                brightness = 0;
                break;
            default:
                break;
            }
            refreshPtr->setBrightness(brightness);
            writePattern(renderBufNum, pattern);
//            Serial.println("##: " + String(getBrightness()) + ", " + String(renderBufNum) + ", " + String(pattern));
            rp2040.fifo.push(renderBufNum);
        }
        break;
    case RAMP_BRIGHTNESS:
        numLoops = 10;
        if ((loopCnt % numLoops) == 0) {
            if (rp2040.fifo.pop_nb(&renderBufNum)) {
                pattern = ALL_AMBER_PATTERN;
                brightness = ((refreshPtr->getBrightness() + 1) % 10);
                refreshPtr->setBrightness(brightness);
                writePattern(renderBufNum, pattern);
//              Serial.println("###: " + String(getBrightness()) + ", " + String(renderBufNum) + ", " + String(pattern));
                rp2040.fifo.push(renderBufNum);
            }
        }
        break;
    case ROW_PATTERNS:
        if (loopCnt < 10) {
            pattern = ALL_AMBER_PATTERN;
            if (rp2040.fifo.pop_nb(&renderBufNum)) {
                writePattern(renderBufNum, pattern);
                rp2040.fifo.push(renderBufNum);
            }
            break;
        }
        numLoops = 10;
        if ((loopCnt % numLoops) == 0) {
            if (rp2040.fifo.pop_nb(&renderBufNum)) {
                col = 0;
                row = ((row + 1) % NUM_ROWS);
                memset(refreshPtr->_frameBuffers, 0, sizeof(refreshPtr->_frameBuffers));
                refreshPtr->_frameBuffers[renderBufNum][row][col] = AMBER_COLOR;
//                Serial.println("ROW: " + String(renderBufNum) + ", " + String(row) + ", " + String(col));
                rp2040.fifo.push(renderBufNum);
            }
        }
        break;
    case COLUMN_PATTERNS:
        if (loopCnt < 10) {
            pattern = ALL_AMBER_PATTERN;
            if (rp2040.fifo.pop_nb(&renderBufNum)) {
                writePattern(renderBufNum, pattern);
                rp2040.fifo.push(renderBufNum);
            }
            break;
        }
        numLoops = 10;
        if ((loopCnt % numLoops) == 0) {
            if (rp2040.fifo.pop_nb(&renderBufNum)) {
                row = 0;
                col = ((col + 1) % NUM_COLS);
                memset(refreshPtr->_frameBuffers, 0, sizeof(refreshPtr->_frameBuffers));
                refreshPtr->_frameBuffers[renderBufNum][row][col] = AMBER_COLOR;
//                Serial.println("COL: " + String(renderBufNum) + ", " + String(row) + ", " + String(col));
                rp2040.fifo.push(renderBufNum);
            }
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
    refreshPtr->refresh();
};
