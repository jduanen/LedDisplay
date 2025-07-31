/***************************************************************************
 *
 * Example of using LedDisplay with the Adafruit_GFX Library
 * 
 ****************************************************************************/

// N.B. This can't use the second core as it's being used by LedDisplay_Refresh

#include <Adafruit_GFX.h>
#include <ColorLights.h>


ColorLights ledDisplay();

uint32_t loopCnt;
byte colors[] = {GREEN_COLOR, RED_COLOR, AMBER_COLOR};

void setup() {
	Serial.begin(115200);
    delay(500);
    Serial.println("\nBEGIN: Render Loop");
    delay(500);
    Serial.println("READY: Render Loop");
};

void setup1() {
    // N.B. the delay values are necessary for correct function
    delay(500);
    Serial.println("\nBEGIN: Refresh Loop");
    delay(1000);
    Serial.print("READY: Refresh Loop; Brightness: " + String(getBrightness()));
}

void loop() {
	ledDisplay.fillScreen(colors[loopCnt % sizeof(colors)]);
	ledDisplay.show();
	delay(1000);
	loopCnt++;
};

void loop1() {
	ledDisplay.refresh();
};
