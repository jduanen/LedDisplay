/***************************************************************************
 *
 * LedDisplay Test Program for XIAO RP2040
 * 
 ****************************************************************************/

#define LEDS_ON_DELAY       100   // 100 usec
#define INTER_ROW_DELAY     1000  // 1 msec

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
                frameBuffers[bufNum][row][col] = ((col + (row % (NUM_COLORS + 1))) % (NUM_COLORS + 1));
            }
            Serial.println("");
        }
        break;
    default:
        Serial.println("ERROR: unknown pattern number (" + String(pattNum) + ")");
        break;
    }
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\nBEGIN: Render Loop");

    Serial.println("READY: Render Loop");
    Serial.println(">" + String(rp2040.fifo.available()));
};

void setup1() {
    delay(1500);
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

    /*
    // all FB bufs start off as free
    for (uint32_t bufNum = 0; (bufNum < NUM_BUFS); bufNum++) {
        rp2040.fifo.push(bufNum);
    }
    */

    Serial.println("READY: Refresh Loop");
    Serial.println(">>" + String(rp2040.fifo.available()));
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

// Render loop
void loop() {
    //// TMP TMP TMP just use check pattern
    if (loopCnt == 0) {
        for (renderBufNum = 0; (renderBufNum < NUM_BUFS); renderBufNum++) {
            patternBuf(renderBufNum, CHECK_PATTERN);
            rp2040.fifo.push(renderBufNum);
        }
    }

    //// TMP TMP TMP
/*
    if ((loopCnt % 5) == 0) {
//        Serial.print("Render: Pattern: " + String(pNum) + ", ");
//        Serial.print("avail: " + String(rp2040.fifo.available()) + ", ");
        renderBufNum = rp2040.fifo.pop();  // blocking
//        Serial.println("renderBufNum: " + String(renderBufNum));
        pNum = (pNum + 1) % NUM_PATTERNS;
        patternBuf(renderBufNum, pNum);
        rp2040.fifo.push(renderBufNum);
    }
*/
    loopCnt++;
    delay(1000);
}

// Refresh loop
void loop1() {
    if (curRow == 0) {
        while (rp2040.fifo.available() > 0) {
//            Serial.print("Refresh: avail: " + String(rp2040.fifo.available()) + ", ");
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
        delayMicroseconds(LEDS_ON_DELAY);
        disableRows();
    }
    curRow = (curRow + 1) % NUM_ROWS;
    delayMicroseconds(INTER_ROW_DELAY);

/*
    //// TMP TMP TMP
    loop1Cnt++;
    if ((loop1Cnt % 1000) == 0) {
        Serial.println("Refresh: refreshBufNum:: " + String(refreshBufNum));
    }
*/
};

/*
// can't toggle wires faster than 667nsec, so don't need any of this
unsigned long hrTime;

#define LOOP_CNT    1000000.0

// N.B. This isn't all that accurate, especially with smaller input values
#define nsecToLoopCnt(n)     ((n - 100) / 84)
// measured time: ((l * 84) + 100) nsec
#define spinLoop(l)             { volatile int _i; for(_i=0; _i<l; ++_i) {} }
#define spinNanoseconds(nsec)   spinLoop(nsecToLoopCnt(nsec))

    // Measurements
    digitalWrite(COL_DATA, LOW);
    for (int i = 0; (i < 200); i += 10) {
        digitalWrite(COL_STROBE, LOW);  // 683nsec to get here from start of loop
        spinLoop(i);    //
        spinNanoseconds(i); // cnt: usec; 100: .950, 200: 1.0, 300: 1.1, 400: 1.67, 500: 1.27,... 900: 1.67
                            // cnt: nsec; 0: 925, 40: 933..., 150: 933, 160: ?, 170: 916, 180: 925, 190: 1000
        digitalWrite(COL_STROBE, HIGH); // loop overhead: ~700nsec
    }
    digitalWrite(COL_DATA, HIGH);
    digitalWrite(COL_CLOCK, LOW); // 666.7nsec from above low to this high
    delayMicroseconds(4);  // 1: 72.5usec, 2: 2.4usec, 3: 3.6usec
    digitalWrite(COL_CLOCK, HIGH);
};

//        hrTime = micros();
//        for (int j = 0; (j < LOOP_CNT); j++) {
//        }
//        float hrt = (micros() - hrTime) / LOOP_CNT;
//        Serial.print(String(i) + ": ");
//        Serial.println(hrt);
*/
