/* 2013 Copyright (c) Seeed Technology Inc.  All rights reserved.
   2017 Copyright (c) Tekt Industries. All rights reserved.

  Author: Loovee 2013-9-18 (RGB backlight added 2013-10-15)
  Modified for Mongoose OS by Jonathon Grigg, November 2017

  The MIT License (MIT)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef __RGB_LCD_H__
#define __RGB_LCD_H__

#include <stdio.h>
#include <float.h>
#include "fw/src/mgos.h"
#include "mgos_system.h"
#include "mgos_i2c.h"

// Device I2C Arress
#define LCD_ADDRESS     (0x7c>>1)
#define RGB_ADDRESS     (0xc4>>1)

// backlight color define
#define BACKLIGHT_WHITE           0
#define BACKLIGHT_RED             1
#define BACKLIGHT_GREEN           2
#define BACKLIGHT_BLUE            3

#define REG_RED         0x04        // pwm2
#define REG_GREEN       0x03        // pwm1
#define REG_BLUE        0x02        // pwm0

#define REG_MODE1       0x00
#define REG_MODE2       0x01
#define REG_OUTPUT      0x08

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

struct mgos_lcd {
  uint8_t num_lines;
  uint8_t curr_line;
  uint8_t display_fuction;
  uint8_t display_control;
  uint8_t display_mode;
  uint8_t initialised;
  struct mgos_i2c *conn;
};

struct mgos_lcd *mgos_lcd_create(uint8_t columns, uint8_t lines,
        uint8_t dotsize);

/****** Arduino print library replacements ******/
void mgos_lcd_print_char(struct mgos_lcd *lcd, char c);
void mgos_lcd_println_char(struct mgos_lcd *lcd, char c);
void mgos_lcd_print_string(struct mgos_lcd *lcd, char* str, int len);
void mgos_lcd_println_string(struct mgos_lcd *lcd, char *str, int len);
void mgos_lcd_print_int(struct mgos_lcd *lcd, int i);
void mgos_lcd_println_int(struct mgos_lcd *lcd, int i);
void mgos_lcd_print_double(struct mgos_lcd *lcd, double d);
void mgos_lcd_println_double(struct mgos_lcd *lcd, double d);

void mgos_lcd_clear(struct mgos_lcd *lcd);
void mgos_lcd_cursor_home(struct mgos_lcd *lcd);
void mgos_lcd_set_cursor(struct mgos_lcd *lcd, uint8_t col, uint8_t row);
void mgos_lcd_display_off(struct mgos_lcd *lcd);
void mgos_lcd_display_on(struct mgos_lcd *lcd);
void mgos_lcd_cursor_off(struct mgos_lcd *lcd);
void mgos_lcd_cursor_on(struct mgos_lcd *lcd);
void mgos_lcd_blink_cursor_off(struct mgos_lcd *lcd);
void mgos_lcd_blink_cursor_on(struct mgos_lcd *lcd);
void mgos_lcd_scroll_display_left(struct mgos_lcd *lcd);
void mgos_lcd_scroll_display_right(struct mgos_lcd *lcd);
void mgos_lcd_set_ltr(struct mgos_lcd *lcd);
void mgos_lcd_set_rtl(struct mgos_lcd *lcd);
void mgos_lcd_right_justify(struct mgos_lcd *lcd);
void mgos_lcd_left_justify(struct mgos_lcd *lcd);
/* Allows us to fill the first 8 CGRAM locations with custom characters
  Note: ensure charmap is of length 8 */
void mgos_lcd_create_char(struct mgos_lcd *lcd, uint8_t location,
        uint8_t charmap[]);
void mgos_lcd_blink_led_on(struct mgos_lcd *lcd);
void mgos_lcd_blink_led_off(struct mgos_lcd *lcd);
void mgos_lcd_close(struct mgos_lcd **lcd);
void mgos_lcd_set_rgb(struct mgos_lcd *lcd, uint8_t r, uint8_t g, uint8_t b);
void mgos_lcd_set_pwm(struct mgos_lcd *lcd, uint8_t color_reg, uint8_t value);

/****** Lower level library commands, advanced users only ******/
bool mgos_lcd_command(struct mgos_lcd *lcd, uint8_t value);
bool mgos_lcd_write(struct mgos_lcd *lcd, uint8_t value);

#endif  // __RGB_LCD_H__
