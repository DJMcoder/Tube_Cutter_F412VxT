/*
 * ht16k33.c
 *
 * references:
 * https://github.com/LennartKoepper/STM32-HT16K33-7Segment/blob/main/HT16K33/ht16k33.c
 * https://github.com/adafruit/Adafruit_LED_Backpack/blob/master/Adafruit_LEDBackpack.cpp
 *
 *  Created on: Apr 23, 2024
 *      Author: Caltech Racing
 */

#include "ht16k33.h"
#include <stdlib.h>

#define BRIGHTNESS   0xE0
#define DISPLAYCMD   0x80
#define DISPCMDMASK  0x07
#define BLINKMASK    0x06
#define DISPSTATUS   0x01
#define ONCMD        0x21
#define OFFCMD       0x20

#define DISPREGS     16

static HAL_StatusTypeDef HT16K33_cmd(HT16K33 *mux, uint8_t cmd);
static HAL_StatusTypeDef HT16K33_display_cmd(HT16K33 *mux);

// TODO: non-blocking
static HAL_StatusTypeDef HT16K33_cmd(HT16K33 *mux, uint8_t cmd) {
  return HAL_I2C_Master_Transmit(mux->hi2c, mux->addr << 1, &cmd, 1, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef HT16K33_display_cmd(HT16K33 *mux) {
  uint8_t display_cmd = mux->Display->status & DISPCMDMASK;
  return HT16K33_cmd(mux, DISPLAYCMD | display_cmd );
}
// TODO: non-blocking
HAL_StatusTypeDef HT16K33_write_display(HT16K33 *mux, uint8_t buffer[]) {
  return HAL_I2C_Mem_Write(mux->hi2c,
                             mux->addr << 1,
                             mux->Display->min_reg, I2C_MEMADD_SIZE_8BIT,
                             buffer, mux->Display->max_reg - mux->Display->min_reg + 1,
                             HAL_MAX_DELAY);
}

HAL_StatusTypeDef HT16K33_set_brightness(HT16K33 *display, uint8_t brightness) {
  uint8_t brightness_cmd = (brightness & 0x0F);
  if (brightness_cmd == 0) {
      brightness_cmd = 1;
  }
  return HT16K33_cmd(display, BRIGHTNESS | brightness_cmd);
}

HAL_StatusTypeDef HT16K33_set_blink(HT16K33 *mux, HT16K33_BLINK rate) {
  mux->Display->status &= ~BLINKMASK;
  mux->Display->status |= (rate & BLINKMASK);
  return HT16K33_display_cmd(mux);
}

HAL_StatusTypeDef HT16K33_display_on(HT16K33 *mux) {
  mux->Display->status |= DISPSTATUS;
  return HT16K33_display_cmd(mux);
}

HAL_StatusTypeDef HT16K33_display_off(HT16K33 *mux) {
  mux->Display->status &= ~DISPSTATUS;
  return HT16K33_display_cmd(mux);
}

HAL_StatusTypeDef HT16K33_on(HT16K33 *display) {
  return HT16K33_cmd(display, ONCMD);
}

HAL_StatusTypeDef HT16K33_off(HT16K33 *display) {
  return HT16K33_cmd(display, OFFCMD);
}

HT16K33 *HT16K33_init_display(uint8_t addr,
                              I2C_HandleTypeDef *hi2c,
                              uint8_t min_reg,
                              uint8_t max_reg) {
  HT16K33 *mux = malloc(sizeof(HT16K33));
  mux->Display = malloc(sizeof(HT16K33_DISPLAY));

  mux->addr = addr;
  mux->hi2c = hi2c;

  if (HT16K33_on(mux) != HAL_OK) {
      return NULL;
  }
  if (HT16K33_display_on(mux) != HAL_OK) {
      return NULL;
  }
  if (HT16K33_set_brightness(mux, 15) != HAL_OK) {
      return NULL;
  }
  if (HT16K33_set_blink(mux, HT16K33_BLINKOFF) != HAL_OK) {
      return NULL;
  }

  if (min_reg < DISPREGS) {
    mux->Display->min_reg = min_reg;
  }
  else {
      mux->Display->min_reg = DISPREGS;
  }
  if (max_reg < DISPREGS) {
    mux->Display->max_reg = max_reg;
  }
  else {
      mux->Display->max_reg = DISPREGS;
  }

  return mux;
}
