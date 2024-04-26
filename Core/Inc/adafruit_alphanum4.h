/*
 * adafruit_alphanum4.h
 *
 *  Created on: Apr 23, 2024
 *      Author: Caltech Racing
 */

#include "ht16k33.h"
#include "i2c.h"

#define ALPHANUM4_DIGITS 4
#define ALPHANUM4_DEFAULT_ADDR 0x70

// https://learn.adafruit.com/14-segment-alpha-numeric-led-featherwing/usage
// Name: -- DP  N  M  L  K  J  H G2 G1  F  E  D  C  B  A
// Bit:  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0

/*
Segment names for 14-segment alphanumeric displays.
See https://learn.adafruit.com/14-segment-alpha-numeric-led-featherwing/usage

    -------A-------
    |\     |     /|
    | \    J    / |
    |   H  |  K   |
    F    \ | /    B
    |     \|/     |
    |--G1--|--G2--|
    |     /|\     |
    E    / | \    C
    |   L  |   N  |
    | /    M    \ |
    |/     |     \|
    -------D-------  DP
*/

#define ALPHANUM_SEG_A 0b0000000000000001  ///< Alphanumeric segment A
#define ALPHANUM_SEG_B 0b0000000000000010  ///< Alphanumeric segment B
#define ALPHANUM_SEG_C 0b0000000000000100  ///< Alphanumeric segment C
#define ALPHANUM_SEG_D 0b0000000000001000  ///< Alphanumeric segment D
#define ALPHANUM_SEG_E 0b0000000000010000  ///< Alphanumeric segment E
#define ALPHANUM_SEG_F 0b0000000000100000  ///< Alphanumeric segment F
#define ALPHANUM_SEG_G1 0b0000000001000000 ///< Alphanumeric segment G1
#define ALPHANUM_SEG_G2 0b0000000010000000 ///< Alphanumeric segment G2
#define ALPHANUM_SEG_H 0b0000000100000000  ///< Alphanumeric segment H
#define ALPHANUM_SEG_J 0b0000001000000000  ///< Alphanumeric segment J
#define ALPHANUM_SEG_K 0b0000010000000000  ///< Alphanumeric segment K
#define ALPHANUM_SEG_L 0b0000100000000000  ///< Alphanumeric segment L
#define ALPHANUM_SEG_M 0b0001000000000000  ///< Alphanumeric segment M
#define ALPHANUM_SEG_N 0b0010000000000000  ///< Alphanumeric segment N
#define ALPHANUM_SEG_DP 0b0100000000000000 ///< Alphanumeric segment DP


typedef struct {
  HT16K33 *mux;
  uint8_t buffer[ALPHANUM4_DIGITS*2];
} Adafruit_AlphaNum4;

Adafruit_AlphaNum4 *Adafruit_AlphaNum4_Init(uint8_t addr, I2C_HandleTypeDef *hi2c);

void Adafruit_AlphaNum4_load_digit_raw(Adafruit_AlphaNum4 *display, uint8_t digit, uint16_t raw);
void Adafruit_AlphaNum4_set_digit_dp(Adafruit_AlphaNum4 *display, uint8_t digit);
void Adafruit_AlphaNum4_unset_digit_dp(Adafruit_AlphaNum4 *display, uint8_t digit);
void Adafruit_AlphaNum4_load_digit_ascii(Adafruit_AlphaNum4 *display, uint8_t digit, char ascii);
void Adafruit_AlphaNum4_load_digit_integer(Adafruit_AlphaNum4 *display, uint8_t digit, uint8_t num);
void Adafruit_AlphaNum4_load_raw(Adafruit_AlphaNum4 *display, uint16_t raw[ALPHANUM4_DIGITS]);
void Adafruit_AlphaNum4_load_text(Adafruit_AlphaNum4 *display, char text[ALPHANUM4_DIGITS]);
void Adafruit_AlphaNum4_load_numbers(Adafruit_AlphaNum4 *display, uint8_t nums[ALPHANUM4_DIGITS]);
void Adafruit_AlphaNum4_load_integer(Adafruit_AlphaNum4 *display, int16_t number);
void Adafruit_AlphaNum4_load_decimal(Adafruit_AlphaNum4 *display, float number);

HAL_StatusTypeDef Adafruit_AlphaNum4_write_display(Adafruit_AlphaNum4 *display);
