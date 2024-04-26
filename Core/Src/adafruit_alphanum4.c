/*
 * adafruit_alphanum4.c
 *
 * references:
 * https://github.com/adafruit/Adafruit_LED_Backpack/blob/master/Adafruit_LEDBackpack.cpp
 *
 *
 *  Created on: Apr 23, 2024
 *      Author: Caltech Racing
 */


#include "ht16k33.h"
#include "adafruit_alphanum4.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DP_MASK 0b01000000
#define NEGATIVE_SYMBOL ALPHANUM_SEG_G2
#define FLOAT_PRECISION_BITS 7
#define EXPONENT_MAX (float)ALPHANUM4_DIGITS - log10f(1 + pow(2.0, -FLOAT_PRECISION_BITS))

static const uint16_t alphafonttable[] = {

    0b0000000000000001, 0b0000000000000010, 0b0000000000000100,
    0b0000000000001000, 0b0000000000010000, 0b0000000000100000,
    0b0000000001000000, 0b0000000010000000, 0b0000000100000000,
    0b0000001000000000, 0b0000010000000000, 0b0000100000000000,
    0b0001000000000000, 0b0010000000000000, 0b0100000000000000,
    0b1000000000000000, 0b0000000000000000, 0b0000000000000000,
    0b0000000000000000, 0b0000000000000000, 0b0000000000000000,
    0b0000000000000000, 0b0000000000000000, 0b0000000000000000,
    0b0001001011001001, 0b0001010111000000, 0b0001001011111001,
    0b0000000011100011, 0b0000010100110000, 0b0001001011001000,
    0b0011101000000000, 0b0001011100000000,
    0b0000000000000000, //
    0b0000000000000110, // !
    0b0000001000100000, // "
    0b0001001011001110, // #
    0b0001001011101101, // $
    0b0000110000100100, // %
    0b0010001101011101, // &
    0b0000010000000000, // '
    0b0010010000000000, // (
    0b0000100100000000, // )
    0b0011111111000000, // *
    0b0001001011000000, // +
    0b0000100000000000, // ,
    0b0000000011000000, // -
    0b0100000000000000, // .
    0b0000110000000000, // /
    0b0000110000111111, // 0
    0b0000000000000110, // 1
    0b0000000011011011, // 2
    0b0000000010001111, // 3
    0b0000000011100110, // 4
    0b0010000001101001, // 5
    0b0000000011111101, // 6
    0b0000000000000111, // 7
    0b0000000011111111, // 8
    0b0000000011101111, // 9
    0b0001001000000000, // :
    0b0000101000000000, // ;
    0b0010010000000000, // <
    0b0000000011001000, // =
    0b0000100100000000, // >
    0b0001000010000011, // ?
    0b0000001010111011, // @
    0b0000000011110111, // A
    0b0001001010001111, // B
    0b0000000000111001, // C
    0b0001001000001111, // D
    0b0000000011111001, // E
    0b0000000001110001, // F
    0b0000000010111101, // G
    0b0000000011110110, // H
    0b0001001000001001, // I
    0b0000000000011110, // J
    0b0010010001110000, // K
    0b0000000000111000, // L
    0b0000010100110110, // M
    0b0010000100110110, // N
    0b0000000000111111, // O
    0b0000000011110011, // P
    0b0010000000111111, // Q
    0b0010000011110011, // R
    0b0000000011101101, // S
    0b0001001000000001, // T
    0b0000000000111110, // U
    0b0000110000110000, // V
    0b0010100000110110, // W
    0b0010110100000000, // X
    0b0001010100000000, // Y
    0b0000110000001001, // Z
    0b0000000000111001, // [
    0b0010000100000000, //
    0b0000000000001111, // ]
    0b0000110000000011, // ^
    0b0000000000001000, // _
    0b0000000100000000, // `
    0b0001000001011000, // a
    0b0010000001111000, // b
    0b0000000011011000, // c
    0b0000100010001110, // d
    0b0000100001011000, // e
    0b0000000001110001, // f
    0b0000010010001110, // g
    0b0001000001110000, // h
    0b0001000000000000, // i
    0b0000000000001110, // j
    0b0011011000000000, // k
    0b0000000000110000, // l
    0b0001000011010100, // m
    0b0001000001010000, // n
    0b0000000011011100, // o
    0b0000000101110000, // p
    0b0000010010000110, // q
    0b0000000001010000, // r
    0b0010000010001000, // s
    0b0000000001111000, // t
    0b0000000000011100, // u
    0b0010000000000100, // v
    0b0010100000010100, // w
    0b0010100011000000, // x
    0b0010000000001100, // y
    0b0000100001001000, // z
    0b0000100101001001, // {
    0b0001001000000000, // |
    0b0010010010001001, // }
    0b0000010100100000, // ~
    0b0011111111111111,

};


Adafruit_AlphaNum4 *Adafruit_AlphaNum4_Init(uint8_t addr,
                                            I2C_HandleTypeDef *hi2c) {
  Adafruit_AlphaNum4 *display = malloc(sizeof(Adafruit_AlphaNum4));

  if (display == NULL) {
      return NULL;
  }

  display->mux = HT16K33_init_display(addr, hi2c, 0, 2*ALPHANUM4_DIGITS - 1);

  if (display->mux == NULL) {
      return NULL;
  }

  for (int i = 0; i < ALPHANUM4_DIGITS*2; i++) {
      display->buffer[i] = 0;
  }

  return display;
}

void Adafruit_AlphaNum4_load_digit_raw(Adafruit_AlphaNum4 *display, uint8_t digit, uint16_t raw) {
  display->buffer[digit * 2]     = raw & 0xFF;
  display->buffer[digit * 2 + 1] = raw >> 8;
}

void Adafruit_AlphaNum4_set_digit_dp(Adafruit_AlphaNum4 *display, uint8_t digit) {
  display->buffer[digit * 2 + 1]     |= DP_MASK;
}

void Adafruit_AlphaNum4_unset_digit_dp(Adafruit_AlphaNum4 *display, uint8_t digit) {
  display->buffer[digit * 2 + 1]     &= ~DP_MASK;
}

void Adafruit_AlphaNum4_load_digit_ascii(Adafruit_AlphaNum4 *display, uint8_t digit, char ascii) {
  Adafruit_AlphaNum4_load_digit_raw(display, digit, alphafonttable[(uint8_t)ascii]);
}

void Adafruit_AlphaNum4_load_digit_integer(Adafruit_AlphaNum4 *display, uint8_t digit, uint8_t num) {
  char ascii = num + '0';
  Adafruit_AlphaNum4_load_digit_ascii(display, digit, ascii);
}

void Adafruit_AlphaNum4_load_raw(Adafruit_AlphaNum4 *display, uint16_t raw[ALPHANUM4_DIGITS]) {
  for (uint8_t i = 0; i < ALPHANUM4_DIGITS; i++) {
      Adafruit_AlphaNum4_load_digit_raw(display, i, raw[i]);
  }
}

void Adafruit_AlphaNum4_load_text(Adafruit_AlphaNum4 *display, char text[ALPHANUM4_DIGITS]) {
  for (uint8_t i = 0; i < ALPHANUM4_DIGITS; i++) {
      Adafruit_AlphaNum4_load_digit_ascii(display, i, text[i]);
  }
}

void Adafruit_AlphaNum4_load_numbers(Adafruit_AlphaNum4 *display, uint8_t nums[ALPHANUM4_DIGITS]) {
  for (uint8_t i = 0; i < ALPHANUM4_DIGITS; i++) {
      Adafruit_AlphaNum4_load_digit_integer(display, i, nums[i]);
  }
}

void Adafruit_AlphaNum4_load_integer(Adafruit_AlphaNum4 *display, int16_t number) {
  if (number < 0) {

  }
  uint8_t i = ALPHANUM4_DIGITS - 1;
  int16_t num = number;
  while (num != 0 && i >= 0) {
      Adafruit_AlphaNum4_load_digit_integer(display, i, num);
      num /= 10;
      i--;
  }

  // add minus sign
  if (i >= 0 && number < 0) {
      Adafruit_AlphaNum4_load_digit_raw(display, i, NEGATIVE_SYMBOL);
      i--;
  }

  for (; i >= 0; i--) {
      Adafruit_AlphaNum4_load_digit_ascii(display, i, ' ');
  }
}

void Adafruit_AlphaNum4_load_decimal(Adafruit_AlphaNum4 *display, float number) {
  float log_num = log10f(fabs(number));
  int8_t exponent = (int8_t)floorf(log_num);

  if (number == 0.0) {
      Adafruit_AlphaNum4_load_digit_integer(display, ALPHANUM4_DIGITS - 1, 0);
      Adafruit_AlphaNum4_load_digit_integer(display, ALPHANUM4_DIGITS - 2, 0);
      Adafruit_AlphaNum4_set_digit_dp(display, ALPHANUM4_DIGITS - 2);
      if (signbit(number)) {
          Adafruit_AlphaNum4_load_digit_raw(display, ALPHANUM4_DIGITS - 3, NEGATIVE_SYMBOL);
      }
  }
  else if (isinf(number)) {
      Adafruit_AlphaNum4_load_text(display, " inf");
      if (signbit(number)) {
          Adafruit_AlphaNum4_load_digit_raw(display, ALPHANUM4_DIGITS - 4, NEGATIVE_SYMBOL);
      }
  }
  else if (isnan(number)) {
      Adafruit_AlphaNum4_load_text(display, " NaN");
  }


  else if ((log_num < EXPONENT_MAX     && log_num >= -(EXPONENT_MAX - 1) && number >= 0) ||
           (log_num < EXPONENT_MAX - 1 && log_num >= -(EXPONENT_MAX - 2) && number < 0)) {
      uint8_t left_index = 0;
      if (number < 0) {
          Adafruit_AlphaNum4_load_digit_raw(display, left_index, NEGATIVE_SYMBOL);
          left_index++;
      }
      // sig figs
      int8_t digit_index = exponent;
      if (digit_index < 0) {
          digit_index = 0;
      }
      while (left_index < ALPHANUM4_DIGITS) {
          uint8_t cur_digit = (uint8_t)fmod((fabs(number) / pow(10.0, digit_index)), 10.0);
          Adafruit_AlphaNum4_load_digit_integer(display, left_index, cur_digit);
          if (digit_index == 0) {
              Adafruit_AlphaNum4_set_digit_dp(display, left_index);
          }
          digit_index--;
          left_index++;
      }
  }
  else {
      // scientific notation

      uint8_t abs_exponent = abs(exponent);
      uint8_t right_index = ALPHANUM4_DIGITS - 1;
      while (abs_exponent != 0 && right_index >= 0) {
          Adafruit_AlphaNum4_load_digit_integer(display, right_index, abs_exponent % 10);
          abs_exponent /= 10;
          right_index--;
      }

      // handle exponent character
      if (exponent >= 0) {
          Adafruit_AlphaNum4_load_digit_ascii(display, right_index, 'e');
      }
      else {
          Adafruit_AlphaNum4_load_digit_raw(display, right_index, alphafonttable['e'] | ALPHANUM_SEG_A);
      }
      right_index--;

      uint8_t left_index = 0;
      if (number < 0) {
          Adafruit_AlphaNum4_load_digit_raw(display, left_index, NEGATIVE_SYMBOL);
          left_index++;
      }

      // handle base
      float mantissa = fabs(number) / pow(10.0, exponent);
      if (left_index <= right_index) {
          Adafruit_AlphaNum4_load_digit_integer(display, left_index, ((uint8_t)mantissa) % 10);
          Adafruit_AlphaNum4_set_digit_dp(display, left_index);
          mantissa = fmod(mantissa*10, 10);
          left_index++;
      }
      while (left_index <= right_index) {
          Adafruit_AlphaNum4_load_digit_integer(display, left_index, (uint8_t)mantissa);
          mantissa = fmod(mantissa*10, 10);
          left_index++;
      }

  }
}

HAL_StatusTypeDef Adafruit_AlphaNum4_write_display(Adafruit_AlphaNum4 *display) {
  return HT16K33_write_display(display->mux, display->buffer);
}
