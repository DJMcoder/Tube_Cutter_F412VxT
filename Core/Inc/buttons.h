/*
 * buttons.h
 *
 *  Created on: Apr 17, 2024
 *      Author: Caltech Racing
 */
#include "cut_code.h"

extern TIM_HandleTypeDef htim3;
extern struct PCNC_Setup *cutter;
extern uint32_t cur_code;
extern uint32_t num_codes;
extern Cut_Code codes[];

typedef struct {
	GPIO_TypeDef *Port;
	uint16_t Pin;
	GPIO_PinState last_state;
} Button;

void Debounce_Buttons();
void Button_Debounced(uint16_t Pin, GPIO_PinState state);
