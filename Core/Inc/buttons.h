/*
 * buttons.h
 *
 *  Created on: Apr 17, 2024
 *      Author: Caltech Racing
 */

extern TIM_HandleTypeDef htim3;
extern struct PCNC_Setup *cutter;

typedef struct {
	GPIO_TypeDef *Port;
	uint16_t Pin;
} Button;

void Debounce_Buttons();
void Button_Debounced(uint16_t Pin, GPIO_PinState state);
