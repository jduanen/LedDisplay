/***************************************************************************
 *
 * Test program for low-level LedDisplay Render/Refresh library
 * 
 ****************************************************************************/

#include "LedDisplay.h"


LedDisplay *ledDisplayPtr;

uint32_t loopCnt;
byte row = 0;
byte col = 0;
byte color;

void setup() {
	Serial.begin(115200);
    delay(1000);
    Serial.println("\nBEGIN: Render Loop");
    ledDisplayPtr = new LedDisplay(BPP, NUM_COLS, NUM_ROWS);
    Serial.println("READY: Render Loop");
};

void setup1() {
    // N.B. the delay values are necessary for correct function
    delay(1500);
    Serial.println("\nBEGIN: Refresh Loop");
    ledDisplayPtr->refreshInit();
    delay(500);
    Serial.print("READY: Refresh Loop; Brightness: " + String(ledDisplayPtr->getBrightness()));
};

#define TEST_PAT_NUM    7

void loop() {
    switch (TEST_PAT_NUM) {
    case 0:
        for (row = 0; (row < NUM_ROWS); row++) {
            for (col = 0; (col < NUM_COLS); col++) {
                ledDisplayPtr->drawPixel(col, row, (loopCnt % NUM_COLORS));
            }
        }
        break;
    case 1:
        ledDisplayPtr->fillScreen(loopCnt % NUM_COLORS);
        break;
    case 2:
        int x, y, color;
        x = 0; y = 0;
        ledDisplayPtr->drawPixel(x, y, GREEN_COLOR);
        x = (NUM_COLS / 2); y = (NUM_ROWS / 2);
        ledDisplayPtr->drawPixel(x, y, AMBER_COLOR);
        x = (NUM_COLS - 2); y = (NUM_ROWS - 1);
        ledDisplayPtr->drawPixel(x, y, RED_COLOR);
        break;
    case 3:
        //// FIXME last column broken
        ledDisplayPtr->drawFastHLine(0, 0, NUM_COLS, GREEN_COLOR);
        ledDisplayPtr->drawFastHLine(0, (NUM_ROWS / 2), NUM_COLS, GREEN_COLOR);
        ledDisplayPtr->drawFastHLine(0, (NUM_ROWS - 1), NUM_COLS, GREEN_COLOR);

        ledDisplayPtr->drawFastVLine(0, 0, NUM_ROWS, RED_COLOR);
        ledDisplayPtr->drawFastVLine((NUM_COLS / 4), 0, NUM_ROWS, RED_COLOR);
        ledDisplayPtr->drawFastVLine((NUM_COLS / 2), 0, NUM_ROWS, RED_COLOR);
        ledDisplayPtr->drawFastVLine(((NUM_COLS * 3) / 4), 0, NUM_ROWS, RED_COLOR);
        ledDisplayPtr->drawFastVLine((NUM_COLS - 2), 0, NUM_ROWS, RED_COLOR);

        ledDisplayPtr->drawPixel((NUM_COLS / 2), (NUM_ROWS / 2), AMBER_COLOR);
        break;
    case 4:
        ledDisplayPtr->drawRect(0, 0, 16, 4, RED_COLOR);
        ledDisplayPtr->fillRect(32, 1, 16, 3, GREEN_COLOR);
        ledDisplayPtr->fillRect(64, 0, 16, NUM_ROWS, RED_COLOR);
        ledDisplayPtr->fillRect(66, 2, 12, (NUM_ROWS - 2), GREEN_COLOR);
        ledDisplayPtr->fillRect(67, 3, 8, (NUM_ROWS - 3), AMBER_COLOR);
        break;
    case 5:
        ledDisplayPtr->drawTriangle(1, 6, 7, 0, 13, 6, RED_COLOR);
        ledDisplayPtr->fillTriangle(3, 5, 7, 1, 11, 5, GREEN_COLOR);
        ledDisplayPtr->fillCircle((NUM_COLS / 4), (NUM_ROWS / 2), 3, AMBER_COLOR);
        ledDisplayPtr->fillCircle((NUM_COLS / 4), (NUM_ROWS / 2), 2, GREEN_COLOR);
        ledDisplayPtr->fillCircle((NUM_COLS / 4), (NUM_ROWS / 2), 1, RED_COLOR);
        break;
    case 6:
        ledDisplayPtr->drawChar(2, 0, 'A', RED_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(2 + 6, 0, 'B', RED_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(2 + 12, 0, 'C', RED_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(20, 0, 'D', BLACK_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(20 + 6, 0, 'E', BLACK_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(20 + 12, 0, 'F', BLACK_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(38, 0, 'g', RED_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(38 + 6, 0, 'h', RED_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(38 + 12, 0, 'i', RED_COLOR, GREEN_COLOR, 1);
        ledDisplayPtr->drawChar(56, 0, '0', RED_COLOR, BLACK_COLOR, 1);
        ledDisplayPtr->drawChar(56 + 6, 0, '1', RED_COLOR, BLACK_COLOR, 1);
        ledDisplayPtr->drawChar(56 + 12, 0, '2', RED_COLOR, BLACK_COLOR, 1);
        break;
    case 7:
        //// 14-digit, zero-padded, counter that counts up (>1K counts/sec with delay=0)
        char sbuf[20];
        sprintf(sbuf, "%014d", loopCnt);
        row = 0;
        for (int i = 0; (i < strlen(sbuf)); i++) {
            ledDisplayPtr->drawChar(row, 0, sbuf[i], RED_COLOR, BLACK_COLOR, 1);
            row += 6;
        }
        break;
    case 8:
        ledDisplayPtr->setCursor(0, 0);
        ledDisplayPtr->setTextColor(RED_COLOR, BLACK_COLOR);
        ledDisplayPtr->setTextSize(1);
        ledDisplayPtr->print("Hello World!");
        break;
    default:
        Serial.println("ERROR: invalid test pattern number - " + String(TEST_PAT_NUM));
        break;
    }
    ledDisplayPtr->show();
	delay(1000);
    loopCnt++;
};

void loop1() {
	ledDisplayPtr->refresh();
};
