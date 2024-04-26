/*
 * cut_setup.h
 *
 *  Created on: Apr 25, 2024
 *      Author: Caltech Racing
 */

#ifndef INC_CUT_SETUP_H_
#define INC_CUT_SETUP_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_it.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim7;

extern uint32_t CCR_MIN;        // number of ticks associated with 360deg
extern uint32_t CCR_MAX;        // number of ticks associated with 360deg
extern volatile uint32_t *CCR ;   // timer selection for PWM
extern uint32_t PWM_TIM_CH;
extern TIM_HandleTypeDef *PWM_TIM;
extern TIM_HandleTypeDef *Lock_TIM; // timer selection for lockout
extern uint32_t Lock_Multiplier;  // tick conversion from PWM tick to lockout tick
extern uint32_t Lock_Buffer;    // extra time for lockout

extern double steps_to_inches;  // translation of lead screw due to one pulse, in inches
extern uint16_t     Pulse_Pin;  // pin connection for PUL+
extern GPIO_TypeDef *Pulse_Port;
extern uint16_t     Dir_Pin;    // pin connection for DIR+
extern GPIO_TypeDef *Dir_Port;
extern uint16_t     Move_Up_Pin;    // pin connection for moving up
extern GPIO_TypeDef *Move_Up_Port;
extern uint16_t     Move_Down_Pin;    // pin connection for moving down
extern GPIO_TypeDef *Move_Down_Port;
extern TIM_HandleTypeDef *step_htim;

#endif /* INC_CUT_SETUP_H_ */
