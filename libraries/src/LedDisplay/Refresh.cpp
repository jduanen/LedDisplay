/***************************************************************************
 *
 * LedDisplay Refresh Function for XIAO RP2040
 * 
 ****************************************************************************/

#include <Arduino.h>
#include "Refresh.h"


// N.B. run this in loop1
Refresh::Refresh() {
    init();
};

// N.B. run this in loop1
Refresh::Refresh(uint8_t row0, uint8_t row1, uint8_t row2,
                 uint8_t greenLedsEnable, uint8_t redLedsEnable,
                 uint8_t columnData, uint8_t columnStrobe, uint8_t columnClock) {
    _row0 = row0;
    _row1 = row1;
    _row2 = row2;
    _greensEnable = greenLedsEnable;
    _redsEnable = redLedsEnable;
    _columnData = columnData;
    _columnStrobe = columnStrobe;
    _columnClock = columnClock;
    init();
};

void Refresh::init() {
    pinMode(_row0, OUTPUT);
    pinMode(_row1, OUTPUT);
    pinMode(_row2, OUTPUT);
    pinMode(_greensEnable, OUTPUT);
    digitalWrite(_greensEnable, HIGH);
    pinMode(_redsEnable, OUTPUT);
    digitalWrite(_redsEnable, HIGH);

    pinMode(_columnData, OUTPUT);
    digitalWrite(_columnData, LOW);
    pinMode(_columnStrobe, OUTPUT);
    digitalWrite(_columnStrobe, HIGH);
    pinMode(_columnClock, OUTPUT);
    digitalWrite(_columnClock, LOW);

    setBrightness(DEF_BRIGHTNESS);
    Serial.println("Refresh starting");
};

Refresh::~Refresh() {
    //// give buffer back
    Serial.println("Refresh stopped"); //// TMP TMP TMP
};

void Refresh::setBrightness(byte percent) {
    // int <newvalue> = map(<value>, <original_min>, <original_max>, <new_min>, <new_max>);
    assert((percent >= 0) && (percent <= 100));
    _ledsOnDelay = map(percent, 0, 100, MAX_LEDS_ON_DELAY, MIN_LEDS_ON_DELAY);
};

byte Refresh::getBrightness() {
    // int <newvalue> = map(<value>, <original_min>, <original_max>, <new_min>, <new_max>);
    return map(_ledsOnDelay, MAX_LEDS_ON_DELAY, MIN_LEDS_ON_DELAY, 0, 100);
};

void Refresh::enableRow(int rowColor, int rowNum) {
    //// assert(rowNum < NUM_ROWS);
    digitalWrite(_row0, (rowNum & 0x01));
    digitalWrite(_row1, (rowNum & 0x02));
    digitalWrite(_row2, (rowNum & 0x04));

    //// assert(rowColor < NUM_COLS);
    switch (rowColor) {
    case (GREEN_COLOR):
        digitalWrite(_greensEnable, LOW);
        break;
    case (RED_COLOR):
        digitalWrite(_redsEnable, LOW);
        break;
    default:
        Serial.println("ERROR: invalid row color (" + String(rowColor) + ")");
        break;
    }
};

void Refresh::disableRows() {
    digitalWrite(_greensEnable, HIGH);
    digitalWrite(_redsEnable, HIGH);
    digitalWrite(_row0, LOW);
    digitalWrite(_row1, LOW);
    digitalWrite(_row2, LOW);
};

void Refresh::shiftInPixels(int bufNum, int row, int color) {
    //// FIXME change the row & column directions -- origins: SW=upper left, HW=upper right
    for (uint32_t col = 0; (col < NUM_COLS); col++) {
        // clock in data
        digitalWrite(_columnClock, LOW);
        digitalWrite(_columnData, (_frameBuffers[bufNum][row][col] & color));
        digitalWrite(_columnClock, HIGH);
    }

    // strobe to latch data
    digitalWrite(_columnStrobe, LOW);
    digitalWrite(_columnStrobe, HIGH);
    digitalWrite(_columnClock, LOW);
    digitalWrite(_columnData, LOW);
};

// N.B. this must be run on the Refresh core as it pushes all buffers to the Render core
void Refresh::refreshInit() {
    // free all buffers by pushing them to the render core
    for (uint32_t bufNum = 0; (bufNum < NUM_BUFS); bufNum++) {
        rp2040.fifo.push(bufNum);
    }
};

// N.B. run this in loop1
void Refresh::refresh() {
    while (rp2040.fifo.available() > 0) {
//        Serial.print("Refresh: avail=" + String(rp2040.fifo.available()) + ", ");
        if (BUF_NUM_VALID(_refreshBufNum)) {
            rp2040.fifo.push(_refreshBufNum);
//            Serial.print("returned: " + String(_refreshBufNum) + ", ");
        }
        _refreshBufNum = rp2040.fifo.pop();  // blocking
//        Serial.println("new refreshBufNum: " + String(_refreshBufNum));
    }

    if (BUF_NUM_VALID(_refreshBufNum)) {
        for (uint32_t color = 0; (color < NUM_LED_COLORS); color++) {
            assert((_refreshBufNum >= 0) && (_refreshBufNum < NUM_BUFS));
            shiftInPixels(_refreshBufNum, _curRow, LED_COLORS(color));
            enableRow(LED_COLORS(color), _curRow);
            delayMicroseconds(_ledsOnDelay);
            disableRows();
        }
        _curRow = ((_curRow + 1) % NUM_ROWS);
        delayMicroseconds(INTER_ROW_DELAY);
    }
};
