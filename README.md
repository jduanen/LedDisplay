# LedDisplay
Tri-Color (Red/Amber/Green) 17x85 LED Display with RP2040 Controller

## Notes
* ColorLights by GeoGraphics Model SW-14CR
* Write Adafruit driver so it works with the Adafruit Gfx library
* Power consumption measurements
  - All LEDs on, 8.5V supply voltage
** TODO make this a table **
  Brightness    Amps    Watts
  0%            .2A      ~2W
  25%          1.4A     ~12W
  50%          2.3A     ~20W
  75%          2.9A     ~25W
  100%         3.4A     ~30W

# Rendering Interfaces

Currently, either the Adafruit Gfx library or an HD44780 LCD controller emulator
can be used to render to the LedDisplay.

## Adafruit Gfx Library

This interface provides a ????

## HD44780 Emulator

This interface provides an HD44780 LCD controller interface to ESPHome's rendering
engine and an interface to a communications link (e.g., serial, SPI, I2C) connected
to an RP2040 that drives the LedDisplay.

### HD44780 Emulator Backend Protocol

* Features
  - communication-link independent
    * serial, SPI, I2C
  - high-performance/low-overhead
    * ignore 4-bit mode, always operate in 8-bit mode
  - works with LedDisplay
    * only one line of seventeen 5x7 characters
    * only 7 rows (as opposed to the 8 expected by the controller)
      - no descenders -- clip or shift up
      - no underline cursor -- XOR blink the character position
  - Display Data RAM (DDRAM), 640 Bytes
    * depends on size of display -- e.g., 80 bytes (i.e., 40 per display row)
    * address gap between rows
      - first row: 0-39 (0x00-0x27), second row: 64-103 (0x40-0x67)
    * read/write by using a Set DDRAM Address command
      - selects the starting address within the DDRAM
  - Character Generator RAM (CGRAM), 64 Bytes
    * used to encode up to eight characters
    * 64 fields, addresses 0x00-0x3F
    * each field is 5-bits mapping to a row of pixels in each character
    * lower 3 bits of codes 0-17 and 8-15 select groups of 8 fields
  - Character Generator ROM (read-only), 1,240 Bytes
    * 208 5x8 characters and 32 5x10 characters
    * different kinds of fonts, including higher resolution ones
      - e.g, Japanese, Greek letters, math symbols, etc.
    * 

* links
  - Custom Character generation for HD47780 LCD modules
    * https://omerk.github.io/lcdchargen/

* Protocol
  - two ASCII characters
    * first byte contains two bits: Register Select (RS) and Read/Not-Write (R/W)
      - RS: 0 = Command access; 1 = Data access
      - R/W: 0 = write to display; 1 = read from display
      - two bits represented by ASCII characters '0'-'3'
    * second byte contains command-specific bits
      - there are eight commands
      - each command starts with a 1 in a bit positions 0-7, with leading 0s
    * message formats
      - '0': Write Commands
        * b00000001: Clear Display
          - clear display and set cursor to home position (address 0)
        * b0000001x: Cursor Home
          - set cursor to home position, return display being shifted to original position
          - DDRAM content is unchanged
        * b000001IS: Entry Mode Set
          - I: set cursor move direction
            * 0 = decrement cursor position; 1 = increment cursor position
          - S: display shift control
            * 0 = do not shift display; 1 = shift display
        * b00001DCB: Display On/Off Control
          - D: turn display on/off
            * 0 = turn display off; 1 = turn display on
          - C: turn cursor on/off
            * 0 = turn cursor off; 1 = turn cursor on
          - B: blink character at cursor position
            * 0 = no blinking cursor; 1 = blink character at cursor position
        * b0001SRxx: Cursor/Display Shift
          - S: select cursor move or display shift
            * 0 = move cursor; 1 = shift display
          - R: direction to shift display
            * 0 = shift display left; 1 = shift display right
        * b001LNFxx: Function Set
          - L: set interface data length
            * 0 = 4-bit interface; 1 = 8-bit interface
          - N: number of display lines
            * 0 = one line; 1 = two lines
          - F: character font selection
            * 0 = 5x8 characters; 1 = 5x10 characters
        * b01aaaaaa: Set CGRAM Address
          - aaaaaa: address in CGRAM where the next read/write begins
        * b1aaaaaaa: Set DDRAM Address
          - aaaaaaa: address in DDRAM where the next read/write begins
      - '1': Read Command
        * bBaaaaaaa: returns busy flag and CGRAM/DDRAM address counter contents
          - B: busy flag
            * 0 = not busy, can accept commands; 1 = busy, operation in progress
          - aaaaaaa: CGRAM/DDRAM address counter (depending on previous instruction)
      - '2': Write Data
        * Write to CGRAM/DDRAM at address in counter (depending on previous instruction)
      - '3': Read Data
        * Read from CGRAM/DDRAM at address in counter (depending on previous instruction)
