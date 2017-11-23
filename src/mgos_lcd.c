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

#include "mgos_lcd.h"

enum mgos_app_init_result mgos_lcd_init() {
  return MGOS_APP_INIT_SUCCESS;
}

/* columns is never actually used in the Arduino library but it seems to be
  convention to have it anyway, so we'll keep it regardless */
struct mgos_lcd *mgos_lcd_create(uint8_t columns, uint8_t lines,
        uint8_t dotsize) {
  if (!mgos_i2c_init()) return NULL;

  struct mgos_lcd *lcd = calloc(1, sizeof(*lcd));
  if (lcd == NULL) return NULL;

  lcd->conn = mgos_i2c_get_global();
  lcd->num_lines = lines;
  // the following is technically redundant due to calloc call zeroing memory
  lcd->curr_line = 0;
  lcd->display_fuction = LCD_5x8DOTS;
  lcd->display_control = 0;
  lcd->display_mode = 0;

  if (lines > 1) {
    lcd->display_fuction |= LCD_2LINE
  }

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != 0) && (lines == 1)) {
    lcd->display_fuction |= LCD_5x10DOTS;
  }

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above
  // 2.7V before sending commands.
  mgos_usleep(50000);

  // this is according to the hitachi HD44780 datasheet
  // page 45 figure 23

  // send function set command sequence
  mgos_lcd_command(lcd, LCD_FUNCTIONSET | lcd->display_fuction);
  mgos_usleep(4500);  // wait more than 4.1ms
  // second try
  mgos_lcd_command(lcd, LCD_FUNCTIONSET | lcd->display_fuction);
  mgos_usleep(150);
  // third go
  mgos_lcd_command(lcd, LCD_FUNCTIONSET | lcd->display_fuction);
  // finally, set # lines, font size, etc.
  mgos_lcd_command(lcd, LCD_FUNCTIONSET | lcd->display_fuction);

  // turn the display on with no cursor or blinking by default
  lcd->display_control = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  mgos_lcd_display(lcd);

  // clear the displau
  mgos_lcd_clear(lcd);

  // Initialize to default text direction (for romance languages)
  lcd->display_mode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  // set the entry mode
  mgos_lcd_command(lcd, LCD_ENTRYMODESET | lcd->display_mode);

  // initialise backlight
  mgos_i2c_write_reg_b(lcd->conn, REG_MODE1, 0x00);
  // set LEDs controllable by both PWM and GRPPWM registers
  mgos_i2c_write_reg_b(lcd->conn, REG_OUTPUT, 0xFF);
  // set MODE2 values
  // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
  mgos_i2c_write_reg_b(lcd->conn, REG_MODE2, 0x20);

  // set the RGB backlight to white (for some reason)
  mgos_lcd_set_gdb(lcd, 255, 255, 255);
}

void mgos_lcd_print_char(struct mgos_lcd *lcd, char c) {
  if (lcd == NULL) return;
  mgos_lcd_write(lcd, (uint8_t) c);
}

void mgos_lcd_println_char(struct mgos_lcd *lcd, char c) {
  if (lcd == NULL) return;
  mgos_lcd_write(lcd, (uint8_t) c);
  mgos_lcd_write(lcd, (uint8_t) '\n');
}

void mgos_lcd_print_string(struct mgos_lcd *lcd, char* str, int len) {
  if (lcd == NULL) return;
  for (int i = 0; i < len; i++) {
    mgos_lcd_write(lcd, (uint8_t) str[i]);
  }
}

void mgos_lcd_println_string(struct mgos_lcd *lcd, char *str, int len) {
  if (lcd == NULL) return;
  mgos_lcd_print_string(lcd, str, len);
  mgos_lcd_write(lcd, (uint8_t) '\n');
}

void mgos_lcd_print_int(struct mgos_lcd *lcd, int i) {
  if (lcd == NULL) return;
  char str[20 + 1];   // 64 bit ints are at most 20 digits long, plus \0
  int len = sprintf(str, "%d", i);
  mgos_lcd_print_string(lcd, str, len);
}

void mgos_lcd_println_int(struct mgos_lcd *lcd, int i) {
  if (lcd == NULL) return;
  mgos_lcd_print_int(lcd, i);
  mgos_lcd_write(lcd, (uint8_t) '\n');
}

void mgos_lcd_print_double(struct mgos_lcd *lcd, double d) {
  if (lcd == NULL) return;
  // see https://stackoverflow.com/questions/1701055 regarding string size
  char str[3 + DBL_MANT_DIG - DBL_MIN_EXP + 1];
  int len = sprintf(str, "%f", d);
  mgos_lcd_print_string(lcd, str, len);
}

void mgos_lcd_println_double(struct mgos_lcd *lcd, double d) {
  if (lcd == NULL) return;
  mgos_lcd_print_double(lcd, d);
  mgos_lcd_write(lcd, (uint8_t) '\n');
}

void mgos_lcd_clear(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  mgos_lcd_command(lcd, LCD_CLEARDISPLAY);  // clear and zero cursor position
  mgos_usleep(2000);  // takes a while to run
}

void mgos_lcd_cursor_home(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  mgos_lcd_command(lcd, LCD_RETURNHOME);  // zero cursor position
  mgos_usleep(2000);  // takes a while to run
}

void mgos_lcd_set_cursor(struct mgos_lcd *lcd, uint8_t col, uint8_t row) {
  if (lcd == NULL) return;
  uint8_t data[2] = {0x80, (row == 0 ? col | 0x80 : col | 0xc0)};
  mgos_i2c_write(lcd->conn, LCD_ADDRESS, data, 2, true);
}

void mgos_lcd_display_off(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_control &= ~LCD_DISPLAYON;
  mgos_lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->display_control);
}

void mgos_lcd_display_on(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_control |= LCD_DISPLAYON;
  mgos_lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->display_control);
}

void mgos_lcd_cursor_off(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_control &= ~LCD_CURSORON;
  mgos_lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->display_control);
}

void mgos_lcd_cursor_on(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_control |= LCD_CURSORON;
  mgos_lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->display_control);
}

void mgos_lcd_blink_cursor_off(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_control &= ~LCD_BLINKON;
  mgos_lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->display_control);
}

void mgos_lcd_blink_cursor_on(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_control |= LCD_BLINKON;
  mgos_lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->display_control);
}

void mgos_lcd_scroll_display_left(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  mgos_lcd_command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void mgos_lcd_scroll_display_right(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  mgos_lcd_command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void mgos_lcd_set_ltr(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_mode |= LCD_ENTRYLEFT;
  mgos_lcd_command(lcd, LCD_ENTRYMODESET | lcd->display_mode);
}

void mgos_lcd_set_rtl(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_mode &= ~LCD_ENTRYLEFT;
  mgos_lcd_command(lcd, LCD_ENTRYMODESET | lcd->display_mode);
}

void mgos_lcd_right_justify(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_mode |= LCD_ENTRYSHIFTINCREMENT;
  mgos_lcd_command(lcd, LCD_ENTRYMODESET | lcd->display_mode);
}

void mgos_lcd_left_justify(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  lcd->display_mode &= ~LCD_ENTRYSHIFTINCREMENT;
  mgos_lcd_command(lcd, LCD_ENTRYMODESET | lcd->display_mode);
}

/* Allows us to fill the first 8 CGRAM locations with custom characters
  Note: ensure charmap is of length 8 */
void mgos_lcd_create_char(struct mgos_lcd *lcd, uint8_t location,
        uint8_t charmap[]) {
  if (lcd == NULL) return;
  location &= 0x7; // we only have 8 locations 0-7
  mgos_lcd_command(lcd, LCD_SETCGRAMADDR | (location << 3));

  uint8_t data[9];
  data[0] = 0x40;
  for (int i = 0; i < 8; i++) {
      data [i + 1] = charmap[i];
  }
  mgos_i2c_write(lcd->conn, LCD_ADDRESS, data, 9, true);
}

void mgos_lcd_blink_led_on(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  // blink period in seconds = (<reg 7> + 1) / 24
  // on/off ratio = <reg 6> / 256
  mgos_i2c_write_reg_b(lcd->conn, 0x07, 0x17);  // blink every second
  mgos_i2c_write_reg_b(lcd->conn, 0x06, 0x7f);  // half on, half off
}

void mgos_lcd_blink_led_off(struct mgos_lcd *lcd) {
  if (lcd == NULL) return;
  mgos_i2c_write_reg_b(lcd->conn, 0x07, 0x00);
  mgos_i2c_write_reg_b(lcd->conn, 0x06, 0xff);
}

void mgos_lcd_close(struct mgos_lcd **lcd) {
  free(*lcd);
  *lcd = NULL;
}

void mgos_lcd_set_rgb(struct mgos_lcd *lcd, uint8_t r, uint8_t g, uint8_t b) {
  if (lcd == NULL) return;
  mgos_i2c_write_reg_b(lcd->conn, REG_RED, r);
  mgos_i2c_write_reg_b(lcd->conn, REG_GREEN, g);
  mgos_i2c_write_reg_b(lcd->conn, REG_BLUE, b);
}

void mgos_lcd_set_pwm(struct mgos_lcd *lcd, uint8_t color_reg, uint8_t value) {
  if (lcd == NULL) return;
  mgos_i2c_write_reg_b(lcd->conn, color_reg, value);
}

/****** Lower level library commands, advanced users only ******/

bool mgos_lcd_command(struct mgos_lcd *lcd, uint8_t value) {
  if (lcd == NULL) return false;
  uint8_t data[2] = {0x80, value};
  return mgos_i2c_write(lcd->conn, LCD_ADDRESS, data, 2, true);
}

bool mgos_lcd_write(struct mgos_lcd *lcd, uint8_t value) {
  if (lcd == NULL) return false;
  uint8_t data[2] = {0x40, value};
  return mgos_i2c_write(lcd->conn, LCD_ADDRESS, data, 2, true);
}
