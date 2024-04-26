#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_it.h"
#include "cut_setup.h"
#include "main.h"
extern TIM_HandleTypeDef htim5;

/* SERVO PARAMETERS */
uint32_t CCR_MIN        = 1125;       // number of ticks associated with 0deg
uint32_t CCR_MAX        = 5625;       // number of ticks associated with 360deg
volatile uint32_t *CCR    = &(TIM5->CCR1);  // timer selection for PWM
TIM_HandleTypeDef *PWM_TIM  = &htim5;
uint32_t PWM_TIM_CH     = TIM_CHANNEL_1;
TIM_HandleTypeDef *Lock_TIM   = &htim2;     // timer selection for lockout
uint32_t Lock_Multiplier  = 1;        // tick conversion from PWM tick to lockout tick
uint32_t Lock_Buffer      = 100;        // extra time for lockout

/* STEPPER PARAMETERS */
double steps_to_inches    = 0.00001230314*8;  // translation of lead screw due to one pulse, in inches
uint16_t    Pulse_Pin   = STEP_PULSE_Pin; // pin connection for PUL+
GPIO_TypeDef *Pulse_Port    = STEP_PULSE_GPIO_Port;

uint16_t    Dir_Pin   = STEP_DIR_Pin;   // pin connection for DIR+
GPIO_TypeDef *Dir_Port    = STEP_DIR_GPIO_Port;

uint16_t    Move_Down_Pin   = STEPPER_DOWN_Pin; // pin connection for PUL+
GPIO_TypeDef *Move_Down_Port    = STEPPER_DOWN_GPIO_Port;

uint16_t    Move_Up_Pin   = STEPPER_UP_Pin;   // pin connection for DIR+
GPIO_TypeDef *Move_Up_Port    = STEPPER_UP_GPIO_Port;
TIM_HandleTypeDef *step_htim = &htim4;      //  timer for coordinating stepper pulses
