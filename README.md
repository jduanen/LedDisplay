# LedDisplay
Tri-Color (Red/Amber/Green) 17x85 LED Display with ESPHome/Home Assistant Integration

## Notes
* Power consumption measurements
  - All LEDs on, 8.5V supply voltage

| Brightness | Amps | Watts |
|:----------:|-----:|-----: |
| 0%         |  .2A |   ~2W |
| 25%        | 1.4A |  ~12W |
| 50%        | 2.3A |  ~20W |
| 75%        | 2.9A |  ~25W |
| 100%       | 3.4A |  ~30W |

## Implementation Details

### Display Hardware

The display HW has two components -- the original LED Sign (with the original 8047-based controller removed) and the (ESP32-S3 and ESPHome based) Display Controller.

#### LED Sign

?ColorLights by GeoGraphics Model SW-14CR?

The LED Sign is an array of two-color (red and green) LEDs which is composed of 17 5x7 two-color end-stackable modules, resulting in an array with seven rows and 85 columns.
Each LED module's columns are driven by a chain of 11x MC14094 8-bit shift registers, whose signals are latched and buffered by ???? units.
Each LED module consists of a 5x7 array of Green and Red LED pairs.
All of the LED anodes are connected to the seven rows, and the cathodes are connected to the 85 columns. To illuminate an LED, its row has to be selected, and its cathode must be
pulled down. 

 by transistors in TB62706 and TD62783 (16x and 8x, respectively) arrays of open-collector darlington drivers.

* Display Interface Signals
  - Reset:  reset ????
  - CLK:    clock (????)
  - Sin1:   vertical data in
  - Sin2:   horizontal data in LED1
  - Sin3:   horizontal data in LED2
  - Latch:  ????
  - Strobe: ????
  - Sout1:  delayed data ????
  - Sout2:  delayed data ????
  - Sout3:  delayed data ????

#### Display Controller

A XIAO ESP32-S3 (dual-core 240MHz Tensilica microcontroller) ????
????maybe use a XIAO ESP32-C3 (160MHz RISC-V) instead -- might not need to run Refresh function on second core with the new driver????

### Display Software

????

#### ESPHome External Display Driver

????

#### Light and Versatile Graphics Library (LVGL)

????

### Adafruit Display Driver

?Adafruit driver so it works with the Adafruit Gfx library?


### ESPHome MAX7219digit Emulator

**Abandoned this approach, using ESPHome Custom Display Driver and LVGL instead**

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
