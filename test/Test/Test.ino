/***************************************************************************
 *
 * LedDisplay Test Program for XIAO RP2040
 * 
 ****************************************************************************/

#define ROW_DELAY           1000    // 1 msec (1KHz refresh rate)
#define INTER_ROW_DELAY     (ROW_DELAY - ledsOnDelay)

#define MAX_LEDS_ON_DELAY   0                 //  0 msec (0% brightness)
#define DEF_LEDS_ON_DELAY   (ROW_DELAY / 2)   // .5 msec (50% brightness)
#define MIN_LEDS_ON_DELAY   ROW_DELAY         //  1 msec (100% brightness)

#define ROW_0               D0
#define ROW_1               D1
#define ROW_2               D2
#define GREENS_ENB          D3
#define REDS_ENB            D4

#define COL_DATA            D5
#define COL_STROBE          D6
#define COL_CLOCK           D7

#define NUM_COLS            ((8 + 8 + 1) * 5)
#define NUM_ROWS            7

#define PIXEL_DEPTH         2

#define GREEN_COLOR         0b00000001
#define RED_COLOR           0b00000010
#define AMBER_COLOR         (GREEN_COLOR | RED_COLOR)

#define NUM_COLORS          2

#define NUM_BUFS            3

#define ALL_CLEAR_PATTERN   0
#define ALL_RED_PATTERN     1
#define ALL_GREEN_PATTERN   2
#define ALL_AMBER_PATTERN   3
#define STRIPES_PATTERN     4
#define CHECK_PATTERN       5
#define NUM_PATTERNS        6

#define NONE                -1


byte colors[] = {GREEN_COLOR, RED_COLOR};
byte frameBuffers[NUM_BUFS][NUM_ROWS][NUM_COLS] = {};
byte curRow = 0;
byte refreshBufNum = NONE;
byte renderBufNum = NONE;
uint32_t ledsOnDelay = DEF_LEDS_ON_DELAY;
uint32_t loopCnt = 0;
uint32_t loop1Cnt = 0;


void patternBuf(uint32_t bufNum, uint32_t pattNum) {
    uint32_t i;
    //// assert(pattNum < NUM_PATTERNS);
    switch (pattNum) {
    case ALL_CLEAR_PATTERN:
        for (uint32_t row = 0; (row < NUM_ROWS); row++) {
            for (uint32_t col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = 0;
            }
        }
        break;
    case ALL_RED_PATTERN:
        for (uint32_t row = 0; (row < NUM_ROWS); row++) {
            for (uint32_t col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = RED_COLOR;
            }
        }
        break;
    case ALL_GREEN_PATTERN:
        for (uint32_t row = 0; (row < NUM_ROWS); row++) {
            for (uint32_t col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = GREEN_COLOR;
            }
        }
        break;
    case ALL_AMBER_PATTERN:
        for (uint32_t row = 0; (row < NUM_ROWS); row++) {
            for (uint32_t col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = AMBER_COLOR;
            }
        }
        break;
    case STRIPES_PATTERN:
        for (uint32_t col = 0; (col < NUM_COLS); col++) {
            frameBuffers[bufNum][0][col] = RED_COLOR;
            frameBuffers[bufNum][4][col] = RED_COLOR;
        }
        for (uint32_t col = 0; (col < NUM_COLS); col++) {
            frameBuffers[bufNum][1][col] = GREEN_COLOR;
            frameBuffers[bufNum][5][col] = GREEN_COLOR;
        }
        for (uint32_t col = 0; (col < NUM_COLS); col++) {
            frameBuffers[bufNum][2][col] = AMBER_COLOR;
            frameBuffers[bufNum][6][col] = AMBER_COLOR;
        }
        for (uint32_t col = 0; (col < NUM_COLS); col++) {
            frameBuffers[bufNum][3][col] = 0;
        }
        break;
    case CHECK_PATTERN:
        for (uint32_t row = 0; (row < NUM_ROWS); row++) {
            for (uint32_t col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = ((col + (row % (NUM_COLORS + 2))) % (NUM_COLORS + 2));
            }
        }
        break;
    default:
        Serial.println("ERROR: unknown pattern number (" + String(pattNum) + ")");
        break;
    }
};

void setBrightness(byte percent) {
    // int <newvalue> = map(<value>, <original_min>, <original_max>, <new_min>, <new_max>);
    assert((percent >= 0) && (percent <= 100));
    ledsOnDelay = map(percent, 0, 100, MAX_LEDS_ON_DELAY, MIN_LEDS_ON_DELAY);
}

byte getBrightness() {
    // int <newvalue> = map(<value>, <original_min>, <original_max>, <new_min>, <new_max>);
    return map(ledsOnDelay, MAX_LEDS_ON_DELAY, MIN_LEDS_ON_DELAY, 0, 100);
}

void setup() {
    Serial.begin(115200);
    delay(1000);  // N.B. this delay value is necessary for correct function
    Serial.println("\nBEGIN: Render Loop");
    Serial.println("READY: Render Loop");
};

void setup1() {
    Serial.println("\nBEGIN: Refresh Loop");
    pinMode(ROW_0, OUTPUT);
    pinMode(ROW_1, OUTPUT);
    pinMode(ROW_2, OUTPUT);
    pinMode(GREENS_ENB, OUTPUT);
    digitalWrite(GREENS_ENB, HIGH);
    pinMode(REDS_ENB, OUTPUT);
    digitalWrite(REDS_ENB, HIGH);

    pinMode(COL_DATA, OUTPUT);
    digitalWrite(COL_DATA, LOW);
    pinMode(COL_STROBE, OUTPUT);
    digitalWrite(COL_STROBE, HIGH);
    pinMode(COL_CLOCK, OUTPUT);
    digitalWrite(COL_CLOCK, LOW);

    // all FB bufs start off as free
    for (uint32_t bufNum = 0; (bufNum < NUM_BUFS); bufNum++) {
        rp2040.fifo.push(bufNum);
    }
    setBrightness(50);
    delay(1500);  // N.B. this delay has to be here and in this amount

    Serial.print("READY: Refresh Loop; ");
    Serial.print("Brightness: " + String(getBrightness()) + "%, ");
    Serial.println(String(ledsOnDelay) + " / " + String(INTER_ROW_DELAY));
};

void enableRow(int rowColor, int rowNum) {
    //// assert(rowNum < NUM_ROWS);
    digitalWrite(ROW_0, (rowNum & 0x01));
    digitalWrite(ROW_1, (rowNum & 0x02));
    digitalWrite(ROW_2, (rowNum & 0x04));

    //// assert(rowColor < NUM_COLS);
    switch (rowColor) {
    case (GREEN_COLOR):
        digitalWrite(GREENS_ENB, LOW);
        break;
    case (RED_COLOR):
        digitalWrite(REDS_ENB, LOW);
        break;
    default:
        Serial.println("ERROR: invalid row color (" + String(rowColor) + ")");
        break;
    }
};

void disableRows() {
    digitalWrite(GREENS_ENB, HIGH);
    digitalWrite(REDS_ENB, HIGH);
    digitalWrite(ROW_0, LOW);
    digitalWrite(ROW_1, LOW);
    digitalWrite(ROW_2, LOW);
};

void shiftInPixels(int bufNum, int row, int color) {
    for (uint32_t col = 0; (col < NUM_COLS); col++) {
        // clock in data
        digitalWrite(COL_CLOCK, LOW);
        digitalWrite(COL_DATA, (frameBuffers[bufNum][row][col] & color));
        digitalWrite(COL_CLOCK, HIGH);
    }

    // strobe to latch data
    digitalWrite(COL_STROBE, LOW);
    digitalWrite(COL_STROBE, HIGH);
    digitalWrite(COL_CLOCK, LOW);
    digitalWrite(COL_DATA, LOW);
};

uint32_t pNum = 0;

#define TEST_MODE       FIXED_PATTERN
//#define TEST_MODE       CYCLE_PATTERNS
#define FIXED_PATTERN   0
#define CYCLE_PATTERNS  1

// Render loop
void loop() {
    switch (TEST_MODE) {
    case FIXED_PATTERN:
        // fill all bufs with chosen pattern at the start
        if (loopCnt == 0) {
            for (renderBufNum = 0; (renderBufNum < NUM_BUFS); renderBufNum++) {
//                patternBuf(renderBufNum, CHECK_PATTERN);
                patternBuf(renderBufNum, ALL_AMBER_PATTERN);
                rp2040.fifo.push(renderBufNum);
            }
        }
        break;
    case CYCLE_PATTERNS:
        if ((loopCnt % 3) == 0) {  // try to generate new pattern every third loop
            uint32_t bNum;
            if (rp2040.fifo.pop_nb(&bNum)) {
                renderBufNum = bNum;
                pNum = ((pNum + 1) % NUM_PATTERNS);
                patternBuf(renderBufNum, pNum);
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
    if (curRow == 0) {
        while (rp2040.fifo.available() > 0) {
//            Serial.print("Refresh: avail=" + String(rp2040.fifo.available()) + ", ");
            if ((refreshBufNum >= 0) && (refreshBufNum < NUM_BUFS)) {
                rp2040.fifo.push(refreshBufNum);
//                Serial.print("returned: " + String(refreshBufNum) + ", ");
            }
            refreshBufNum = rp2040.fifo.pop();  // blocking
//            Serial.println("new refreshBufNum: " + String(refreshBufNum));
        }
    }

    for (uint32_t color = 0; (color < NUM_COLORS); color++) {
        assert((refreshBufNum >= 0) && (refreshBufNum < NUM_BUFS));
        shiftInPixels(refreshBufNum, curRow, colors[color]);
        enableRow(colors[color], curRow);
        delayMicroseconds(ledsOnDelay);
        disableRows();
    }
    curRow = (curRow + 1) % NUM_ROWS;
    delayMicroseconds(INTER_ROW_DELAY);
};
