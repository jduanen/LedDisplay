# LedDisplay
Tri-Color (Red/Amber/Green) 17x85 LED Display with ESPHome/Home Assistant Integration

## Notes

## Implementation Details

### Display Hardware

The display HW has two components -- the original LED Sign (with the original 8047-based controller removed) and the (ESP32-S3 and ESPHome based) Display Controller.

#### LED Sign

The LED Sign is/was a ColorLights by GeoGraphics Model SW-14CR.

The LED Sign is an array of two-color (red and green) LEDs which is composed of 17 5x7 two-color end-stackable modules, resulting in an array with seven rows and 85 columns.
Each LED module consists of a 5x7 array of Green and Red LED pairs.
Each LED module's columns are driven by a chain of 11x MC14094 8-bit shift registers.

All of the LED anodes are connected to the seven rows (selected by the decoded output of a 74LS145 decoder), and the cathodes are connected to the 85 columns (which are connected to the shift registers' outputs). To illuminate an LED, its row has to be selected by the decoded one-hot value of a three-bit Row Selector value, and its cathode must be pulled down by the integrated transistor arrays. The pull-down units are TB62706 and TD62783 (16x and 8x, respectively) arrays of open-collector darlington drivers.

##### Display Interface Signals

| Pin |  Name    | Function           |
|:---:|:--------:|:------------------:|
|  1  | Vcc      | +5VDC              |
|  2  | GND      | Ground             |
|  3  | Row_0    | Row Selector bit 0 |
|  4  | Row_1    | Row Selector bit 1 |
|  5  | Row_2    | Row Selector bit 2 |
|  6  | RedLED   | Red LED Enable     |
|  7  | GreenLED | Green LED Enable   |
|  8  | Data     | Shift Data Input   |
|  9  | Strobe   | Latch SR Ouputs    |
| 10  | Clock    | SR Clock           |
| 11  | GND      | Ground             |

##### Power Supply

The display is powered by 9VAC @ ?A power brick through a cannon plug on the back of the unit. There is an on-board 5VDC regulator that provides Vcc to the logic and the LEDs.

[????I think I'm going to modify it to take a 5VDC@6A input instead????]

###### Power consumption measurements

- Measured using the Adafruit GFX Brightness values with all LEDs on, and using an 8.5V supply voltage.

| Brightness | Amps | Watts |
|:----------:|-----:|-----: |
| 0%         |  .2A |   ~2W |
| 25%        | 1.4A |  ~12W |
| 50%        | 2.3A |  ~20W |
| 75%        | 2.9A |  ~25W |
| 100%       | 3.4A |  ~30W |

#### Display Controller

A XIAO ESP32-S3 (dual-core 240MHz Tensilica microcontroller) ????
????maybe use a XIAO ESP32-C3 (160MHz RISC-V) instead -- might not need to run Refresh function on second core with the new driver????

### Display Software

The primary use of this display will be to generate output from Home Assistant, so the LVGL-based HA External Component is the main driver for this display.

In addition to this, there's another driver that supports rendering with the Adafruit GFX library. This is intended for standalone display use, and is not being developed any further.

#### ESPHome External Display Driver

????

#### Light and Versatile Graphics Library (LVGL)

????

### Adafruit Display Driver

In the ColorLights directory, there's a library that lets the display be driven by programs that use the Adafruit GFX library. This is done by sub-classing Adafruit_GFX and providing implementations for the constructor/destructor methods and the drawPixel() method. Some amount of hackery is involved to make a two-bits-per-pixel display work with this library, but otherwise things seem to work as expected for a 7x85 2-bit pixel display.

This code is designed to run on a dual-core processor (e.g., RP2040), where the first core runs the Render loop, and the second core runs the Refresh loop.
To use this library, start by including "LedDisplay.h" and instantiate an LedDisplay object (which inherits from the Adafruit_GFX class) in the first core, and call its refreshInit() method in the second core.
Rendering is done to the display via calls to the LedDisplay's inherited Adafruit GFX methods from within the first core's main loop. Methods like drawPixel(), fillRect(), and drawFastHline() can be called, along with a color argument that selects one of the four possible colors for each pixel (i.e., Black, Red, Green, and Amber). When the show() method is invoked, the refresh function (running on the other core) will display the newly rendered pixels.
The rate at which the render loop illuminates the pixels determines the brightness of the display. This value can be set to values between 0-100%. Brightness is defined for the entire display and so the LEDs will always have the same brightness.

### ESPHome MAX7219digit Emulator

**[This approach has been abandoned, using ESPHome Custom Display Driver and LVGL instead]**

This interface emulates a set of MAX7219 Digit "Matrix" Display controllers
connected over a SPI bus.
It uses a modified ESPHome MAX7219digit component that emits commands over the ESP32-C3's UART, as opposed to the SPI bus.

* Features
  - looks like two separate 7x85 LED arrays
    * different SPI addresses for each color array
  - can combine the two color arrays (logical OR, Red on Top, Yellow on Top)
    * ????how to issue the commands from ESPHome????
  - each color array looks like 11x MAX7219s, each MAX7219 driving 8x8 LEDs
    * first 10x characters: only 8x5 visible (in framebuffer, but not visible)
    * last character: only 5x5 visible (in framebuffer, but not visible)
