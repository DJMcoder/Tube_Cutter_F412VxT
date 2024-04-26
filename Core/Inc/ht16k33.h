/**
 * @file ht16k33.h
 *
 * @brief Driver for any HT16K33 chip
 */

#ifndef HT16K33_H
#define HT16K33_H

#include <stdint.h>
#include "i2c.h"

typedef enum {
  HT16K33_BLINKON0_5HZ  = 0x06,
  HT16K33_BLINKON1HZ    = 0x04,
  HT16K33_BLINKON2HZ    = 0x02,
  HT16K33_BLINKOFF      = 0x00
} HT16K33_BLINK;

typedef struct {
  uint8_t min_reg;
  uint8_t max_reg;
  uint8_t status;
} HT16K33_DISPLAY;

typedef struct {
  uint8_t addr;
  I2C_HandleTypeDef *hi2c;
  HT16K33_DISPLAY *Display;
} HT16K33;


HT16K33 *HT16K33_init_display(uint8_t addr,
                             I2C_HandleTypeDef *hi2c,
                             uint8_t min_reg,
                             uint8_t max_reg);
HAL_StatusTypeDef HT16K33_write_display(HT16K33 *mux, uint8_t buffer[]);

HAL_StatusTypeDef HT16K33_set_brightness(HT16K33 *display, uint8_t brightness);
HAL_StatusTypeDef HT16K33_set_blink(HT16K33 *mux, HT16K33_BLINK rate);
HAL_StatusTypeDef HT16K33_display_on(HT16K33 *mux);
HAL_StatusTypeDef HT16K33_display_off(HT16K33 *mux);
HAL_StatusTypeDef HT16K33_on(HT16K33 *mux);
HAL_StatusTypeDef HT16K33_off(HT16K33 *mux);


#endif /* HT16K33_H */
