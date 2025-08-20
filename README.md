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
|:---:|---------:|:-------------------|
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

The display was originally powered by 9VAC @ 4A power brick through a cannon plug on the back of the unit. There is an on-board 5VDC regulator that provides Vcc to the logic and the LEDs.

I modified the board to use a 12VDC @ 4A power brick instead.

###### Power consumption measurements

I measured the display using the Adafruit GFX Brightness values with all LEDs on, and using an 8.5VDC supply voltage.

| Brightness | Amps | Watts |
|:----------:|-----:|-----: |
| 0%         |  .2A |   ~2W |
| 25%        | 1.4A |  ~12W |
| 50%        | 2.3A |  ~20W |
| 75%        | 2.9A |  ~25W |
| 100%       | 3.4A |  ~30W |

#### Display Controller

I replaced the original 8047 controller with a XIAO ESP32-C3 (160MHz RISC-V processor) that controls the display and runs ESPHome code (which allows it to be driven by Home Assistant).

### Display Software

The primary use of this display will be to generate output from Home Assistant, so the Home Assistant External Component is the main driver for this display.

By creating a driver for this display that inherits from the ESPHome Display Component, it is possible to render (through the use of lamdas) to the display with either the ESPHome native Rendering Engine, or LVGL (being careful not to mix them simultaneously on the same display or bad things will happen).

The ESPHome External Components documentation can be found in: https://esphome.io/components/external_components.html.
The ESPHome LVGL Component's interface is documented in: https://esphome.io/components/lvgl/.

In addition to this, there's another driver that supports rendering with the Adafruit GFX library. This is intended for standalone display use, and is not being developed any further.

#### ESPHome External Component Display Driver

This display is integrated into Home Assistant by way of an External Component known as 'ledDisplay', and is found in 'git@github.com:jduanen/ESPHomeComponents.git'.

This component can be included in an ESPHome config file by adding these lines:
```
external_components:
  - source:
      type: git
      url: https://github.com/jduanen/ESPHomeComponents.git
    components: [ ledDisplay ]
    refresh: always

display:
  - platform: ledDisplay
    id: leds
    rotation: 0
    auto_clear_enabled: false
    update_interval: never

display:
  - platform: led_display
    id: leds
    intensity: 75
    scroll_enable: true
    scroll_mode: STOP
    scroll_speed: 90ms
    scroll_dwell: 400ms
    scroll_delay: 1500ms
    flip_x: false
    update_interval: 5s
    fonts:
      - MT_Pixel_5x7
      - MatrixLight6
      - MatrixLight6X

font:
  - file: "fonts/5x7_MT_Pixel.ttf"
    id: MT_Pixel_5x7
    size: 7
  - file: "fonts/MatrixLight6.ttf"
    id: MatrixLight6
    size: 6
  - file: "fonts/MatrixLight6X.ttf"
    id: MatrixLight6X
    size: 6
  - file: "fonts/MatrixLight8.ttf"
    id: MatrixLight8
    size: 8
  - file: "gfonts://Roboto"
    id: roboto
    size: 7

text_sensor:
  - platform: homeassistant
    name: "LED_DisplayText"
    id: led_display_text
    entity_id: input_text.led_display_text_input
```

This configuration allows the display to be set with a string given in (either mobile or desktop) Home Assistant GUIs.

By default, the 5x7, caps-only, font and red color are used to print whatever string is entered into the text input box on the GUI.
Other colors and fonts can be used by including special characters within the string to be printed. The special characters are similar to the ANSI control sequences used to define colors and styles for character-oriented terminals.

##### Rendering with Control Characters

The custom display component offers a 'printLED(char \*str)' method that looks for escape sequences of the form -- "\\[<fontNum>;<colorNum>m" -- and makes the following characters use the font and color indicated by the single numeric digits given in the escape sequence.
Note that Home Assistant Text Helpers do not recognize escape characters, so the '/e' or '/033' character is dropped from the ANSI escape sequence analogy used here, and just a plain backslash is used.

The font number digit refers to one of (up to ten) fonts defined in the config file. For the example yaml config file given above, the following mapping is defined:

| Font Number |    Font Id    |
|:-----------:|:--------------|
| 0           | MT_Pixel_5x7  |
| 1           | MatrixLight6  |
| 2           | MatrixLight6X |
| 3           | MatrixLight8  |
| 4           | roboto        |

The color digit maps to the display's colors in the following manner:

| Color Number |    Color    |
|:------------:|:------------|
| 0            | Black       |
| 1            | Red         |
| 2            | Green       |
| 3            | Amber       |

Invalid font and color digits are ignored.


##### Rendering with LVGL

This configuration picks up the display's driver code from github, and disables the auto_clear and update functions so that LVGL can handle the rendering and refresh functions itself.

In this example, the LVGL library is included and a simple text string is written to the display. In addition, an `it` object can be used within lambdas in the config code to perform additional rendering operations on the display.

Because the ``LedDisplay`` External Component implement is a subclass of the Display Rendering Engine interface (i.e., `display::DisplayBuffer`), it supports the LVGL rendering functionality and widgets, along with other rendering functions (related to the generation of QR Codes, Images, Animations, Graphs, etc.).

In order for this display to be used with LGVL it must override the following methods:
  * `draw_absolute_pixel_internal(x, y, color)`: to write individual pixels to the frame buffer
  * `setup()`: initializes the HW and logs
  * `dump_config()`: ????
  * `update()`: draw or refresh the pixels and push them to the display with `flush()`
  * `flush()`: LVGL uses to write the changed pixels within the given region to the display by looping over the given region of the display buffer and bit-banging the pixels onto the display HW

In order to assist in generating the desired displays with the various render commands available, an ESPHome Display Simulator is available in https://github.com/Mat931/mat931.github.io. This provides a browser-based simulator of displays for ESPHome, that takes the display dimensions (in terms of its number of pixels in height and width) and provides a panel with the simulated display and a panel for entering rendering operations using an `it` object (which refers to the current display buffer's Rendering Engine object in ESPHome).

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
