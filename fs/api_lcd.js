/* 2017 Copyright (c) Tekt Industries. All rights reserved.

  Mongoose OS Grove LCD with RGB Backlight library API.
  Author: Jonathon Grigg, November 2017
*/

let LCD = {
  // keep these variable names under 5 chars to minimise memory usage
  _crt: ffi('void *mgos_lcd_create(int, int, int)'),
  _cls: ffi('void mgos_lcd_close(void *)'),
  _p:   ffi('void mgos_lcd_print_string(void *, char *, int)');
  _clr: ffi('void mgos_lcd_clear(void *)');
  _ch:  ffi('void mgos_lcd_cursor_home(void *)');
  _sc:  ffi('void mgos_lcd_set_cursor(void *, int, int)');
  _dof: ffi('void mgos_lcd_display_off(void *)');
  _don: ffi('void mgos_lcd_display_on(void *)');
  _cof: ffi('void mgos_lcd_cursor_off(void *)');
  _con: ffi('void mgos_lcd_cursor_on(void *)');
  _bcof: ffi('void mgos_lcd_blink_cursor_off(void *)');
  _bcon: ffi('void mgos_lcd_blink_cursor_on(void *)');
  _sdl: ffi('void mgos_lcd_scroll_display_left(void *)');
  _sdr: ffi('void mgos_lcd_scroll_display_right(void *)');
  _ltr: ffi('void mgos_lcd_set_ltr(void *)');
  _rtl: ffi('void mgos_lcd_set_rtl(void *)');
  _rjst: ffi('void mgos_lcd_right_justify(void *)');
  _ljst: ffi('void mgos_lcd_left_justify(void *)');
  _blon: ffi('void mgos_lcd_blink_led_on(void *)');
  _blof: ffi('void mgos_lcd_blink_led_off(void *)');
  _rgb: ffi('void mgos_lcd_set_rgb(void *, int, int, int)');
  _pwm: ffi('void mgos_lcd_set_pwm(void *, int, int)');

  // Define constants
  LCD_5x8DOTS: 0,
  LCD_5x10DOTS: 4,
  REG_RED: 4,
  REG_GREEN: 3,
  REG_BLUE: 2,

  // **`LCD.create(columns, lines)`**
  // Create an LCD object.
  // Return value: an object with the methods described below, or
  // 'null' in case of a failure.
  // Example:
  // ```javascript
  // let lcd = LCD.create(16, 2);
  // lcd.setRgb(64, 0, 64);
  // lcd.print("Hello, world!");
  // ```
  create: function(columns, lines) {
    let obj = Object.create(LCD._proto);
    obj.lcd = DHT._crt(columns, lines, LCD_5x8DOTS);
    return obj;
  },

  // **`LCD.create(columns, lines, dotsize)`**
  // Create an LCD object with dotsize (eg LCD.LCD_5x10DOTS)
  create: function(columns, lines, dotsize) {
    let obj = Object.create(LCD._proto);
    obj.lcd = DHT._crt(columns, lines, dotsize);
    return obj;
  },

  _proto: {
    // **`lcd.print(value)`**
    // Print a string representation of value (if not a string already)
    print: function(value) {
      let str = JSON.stringify(value);
      LCD._p(this.lcd, str, str.length);
    },
    // **`lcd.clear()`**
    // Clear the screen
    clear: function() {
      LCD._clr(this.lcd);
    },
    // **`lcd.cursorHome()`**
    // Return the LCD cursor to the zero position
    cursorHome: function() {
      LCD._ch(this.lcd);
    }
    // **`lcd.setCursor(column, row)`**
    // Set the cursor position to the desired location
    setCursor: function(column, row) {
      LCD._sc(this.lcd, column, row);
    },
    // **`lcd.displayOff()`**
    // Turn the display off
    displayOff: function() {
      LCD._dof(this.lcd);
    },
    // **`lcd.displayOn()`**
    // Turn the display back on having turned it off
    displayOff: function() {
      LCD._don(this.lcd);
    },
    // **`lcd.cursorOff()`**
    // Turn the cursor display off
    cursorOff: function() {
      LCD._cof(this.lcd);
    },
    // **`lcd.cursorOn()`**
    // Turn the cursor display back on having turned it off
    cursorOn: function() {
      LCD._con(this.lcd);
    },
    // **`lcd.blinkCursorOff()`**
    // Blink the cursor
    blinkCursorOff: function() {
      LCD._bcof(this.lcd);
    },
    // **`lcd.blinkCursorOn()`**
    // Do not blink the cursor
    blinkCursorOn: function() {
      LCD._bcon(this.lcd);
    },
    // **`lcd.scrollLeft()`**
    // Scroll the contents of the display left one position
    scrollLeft: function() {
      LCD._sdl(this.lcd);
    },
    // **`lcd.scrollRight()`**
    // Scroll the contents of the display right one position
    scrollRight: function() {
      LCD._sdr(this.lcd);
    },
    // **`lcd.setRtl()`**
    // Display text from right to left
    setRtl: function() {
      LCD._rtl(this.lcd);
    },
    // **`lcd.setLtr()`**
    // Display text from left to right
    setLtr: function() {
      LCD._ltr(this.lcd);
    },
    // **`lcd.rightJustify()`**
    // Right justify text on the display
    rightJustify: function() {
      LCD._rjst(this.lcd);
    },
    // **`lcd.leftJustify()`**
    // Left justify text on the display
    leftJustify: function() {
      LCD._ljst(this.lcd);
    },
    // **`lcd.blinkLedOff()`**
    // Blink the LED backlight
    blinkLedOff: function() {
      LCD._blof(this.lcd);
    },
    // **`lcd.blinkLedOn()`**
    // Do not blink the LED backlight
    blinkLedOn: function() {
      LCD._blon(this.lcd);
    },
    // **`lcd.setRgb(red, green, blue)`**
    // Set the backlight to the desired colours (0-255 for each colour)
    setRgb: function(red, green, blue) {
      LCD._rgb(this.lcd, red, green, blue);
    },
    // **`lcd.setPwm(colourReg, value)`**
    // Sets the PWM value for the given colour register.
    // Use LCD.REG_RED, LCD.REG_GREEN or LCD.REG_BLUE
    setPwm: function(colourReg, value) {
      LCD._pwm(this.lcd, colourReg, value);
    },
    // **`lcd.close()`**
    // Close LCD object
    close: function() {
      LCD._cls(this.lcd);
    },
  },
};
