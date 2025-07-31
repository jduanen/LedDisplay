/***************************************************************************
 *
 * LedDisplay Test Program for XIAO RP2040
 * 
 ****************************************************************************/

#define STROBE_DELAY        100    // 100 usec
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


byte colors[] = {GREEN_COLOR, RED_COLOR};
byte frameBuffers[NUM_BUFS][NUM_ROWS][NUM_COLS] = {};
byte curRow = 0;
byte refreshBufNum = 0;
byte renderBufNum = 0;
unsigned int loopCnt = 0;
unsigned int loop1Cnt = 0;


void patternBuf(unsigned int bufNum, unsigned int pattNum) {
    unsigned int i;
    //// assert(pattNum < NUM_PATTERNS);
    switch (pattNum) {
    case ALL_CLEAR_PATTERN:
        for (unsigned int row = 0; (row < NUM_ROWS); row++) {
            for (unsigned int col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = 0;
            }
        }
        break;
    case ALL_RED_PATTERN:
        for (unsigned int row = 0; (row < NUM_ROWS); row++) {
            for (unsigned int col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = RED_COLOR;
            }
        }
        break;
    case ALL_GREEN_PATTERN:
        for (unsigned int row = 0; (row < NUM_ROWS); row++) {
            for (unsigned int col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = GREEN_COLOR;
            }
        }
        break;
    case ALL_AMBER_PATTERN:
        for (unsigned int row = 0; (row < NUM_ROWS); row++) {
            for (unsigned int col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = AMBER_COLOR;
            }
        }
        break;
    case STRIPES_PATTERN:
        for (unsigned int col = 0; (col < NUM_COLS); col++) {
            frameBuffers[bufNum][0][col] = RED_COLOR;
            frameBuffers[bufNum][4][col] = RED_COLOR;
        }
        for (unsigned int col = 0; (col < NUM_COLS); col++) {
            frameBuffers[bufNum][1][col] = GREEN_COLOR;
            frameBuffers[bufNum][5][col] = GREEN_COLOR;
        }
        for (unsigned int col = 0; (col < NUM_COLS); col++) {
            frameBuffers[bufNum][2][col] = AMBER_COLOR;
            frameBuffers[bufNum][6][col] = AMBER_COLOR;
        }
        for (unsigned int col = 0; (col < NUM_COLS); col++) {
            frameBuffers[bufNum][3][col] = 0;
        }
        break;
    case CHECK_PATTERN:
        for (unsigned int row = 0; (row < NUM_ROWS); row++) {
            i = row;
            for (unsigned int col = 0; (col < NUM_COLS); col++) {
                frameBuffers[bufNum][row][col] = ((i + 1) % (NUM_COLORS + 1));
            }
        }
        break;
    default:
        Serial.println("ERROR: unknown pattern number (" + String(pattNum) + ")");
        break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\nBEGIN");

    patternBuf(renderBufNum, CHECK_PATTERN);
    renderBufNum = (renderBufNum++ % NUM_BUFS);

    Serial.println("READY: Render Loop");
};

void setup1() {
    pinMode(ROW_0, OUTPUT);
    pinMode(ROW_1, OUTPUT);
    pinMode(ROW_2, OUTPUT);
    pinMode(GREENS_ENB, OUTPUT);
    digitalWrite(GREENS_ENB, LOW);
    pinMode(REDS_ENB, OUTPUT);
    digitalWrite(REDS_ENB, LOW);

    pinMode(COL_DATA, OUTPUT);
    digitalWrite(COL_DATA, LOW);
    pinMode(COL_STROBE, OUTPUT);
    digitalWrite(COL_STROBE, LOW);
    pinMode(COL_CLOCK, OUTPUT);
    digitalWrite(COL_CLOCK, LOW);

    delay(2000);
    Serial.println("\nBEGIN: Refresh Loop");
}


void enableRow(int rowColor, int rowNum) {
    //// assert(rowNum < NUM_ROWS);
    digitalWrite(ROW_0, (rowNum & 0x01));
    digitalWrite(ROW_1, (rowNum & 0x02));
    digitalWrite(ROW_2, (rowNum & 0x04));

    //// assert(rowColor < NUM_COLS);
    switch (rowColor) {
    case (GREEN_COLOR):
        digitalWrite(GREENS_ENB, HIGH);
        break;
    case (RED_COLOR):
        digitalWrite(REDS_ENB, HIGH);
        break;
    default:
        Serial.println("ERROR: invalid row color (" + String(rowColor) + ")");
        break;
    }
};

void disableRows() {
    digitalWrite(GREENS_ENB, LOW);
    digitalWrite(REDS_ENB, LOW);
    digitalWrite(ROW_0, LOW);
    digitalWrite(ROW_1, LOW);
    digitalWrite(ROW_2, LOW);
};

void shiftInPixels(byte fbufPtr[NUM_ROWS][NUM_COLS], int curRow, int color) {
    //// FIXME
}

// Render loop
void loop() {
    //// TMP TMP TMP
    loopCnt++;
    if ((loopCnt % 5) == 0) {
        byte pNum = (loopCnt % NUM_PATTERNS);
        Serial.print("<" + String(pNum) + ">;");
        rp2040.fifo.push(renderBufNum);
        renderBufNum = ((renderBufNum + 1) % NUM_BUFS);
        patternBuf(renderBufNum, pNum);
    }

    delay(1000);
};

// Refresh loop
void loop1() {
    for (unsigned int i = 0; (i < NUM_COLORS); i++) {
        shiftInPixels(frameBuffers[refreshBufNum], curRow, colors[i]);
        enableRow(colors[i], curRow);
        delayMicroseconds(STROBE_DELAY);
        disableRows();
    }
    curRow = (curRow + 1) % NUM_ROWS;
    delayMicroseconds(INTER_ROW_DELAY);

    while (rp2040.fifo.available() > 0) {
        refreshBufNum = rp2040.fifo.pop();
        Serial.print("(" + String(refreshBufNum) + ")");
    }

    //// TMP TMP TMP
    loop1Cnt++;
    if ((loop1Cnt % 100) == 0) {
        Serial.print(".");
    }
};
