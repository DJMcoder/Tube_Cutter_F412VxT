/*
 * buttons.c
 *
 *  Created on: Apr 17, 2024
 *      Author: Caltech Racing
 */
#include "main.h"
#include "buttons.h"
#include "cutter.h"
#include "main.h"

#define BUTTON_TIMER &htim3
#define NUM_BUTTONS 3
Button buttons[] = {
		//{ Port: TUBE_SELECT_GPIO_Port, Pin: TUBE_SELECT_Pin},
		//{ Port: RESET_GPIO_Port,  Pin: RESET_UP_Pin},
		{ Port: B1_GPIO_Port, Pin: B1_Pin}
};

void Debounce_Buttons() {
	static GPIO_PinState last_state[NUM_BUTTONS] = { GPIO_PIN_RESET };
	HAL_TIM_Base_Stop_IT(BUTTON_TIMER);
	for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
		GPIO_PinState new_state = HAL_GPIO_ReadPin(buttons[button].Port, buttons[button].Pin);
		if(new_state != last_state[button]){
			Button_Debounced(buttons[button].Pin, new_state);
			last_state[button] = new_state;
		}
	}
}

void Button_Debounced(uint16_t Pin, GPIO_PinState state) {
	static uint8_t button_stage = 0;

	switch (button_stage) {
		case 0:
			if (Pin == B1_Pin) {
				HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
				PCNC_Go_To_Origin(cutter);
			}
			break;
		case 1:
			if (Pin == B1_Pin) {
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
				PCNC_Begin_Cut(cutter);
			}
			/*
			else if (Pin == STEPPER_DOWN_Pin) {
				if (state == GPIO_PIN_SET) {
					PCNC_Manual_Move_Y(cutter, 1);
				}
				else {
					PCNC_Manual_Move_Y(cutter, 0);
				}
			}
			else if (Pin == STEPPER_UP_Pin) {
				if (state == GPIO_PIN_SET) {
					PCNC_Manual_Move_Y(cutter, -1);
				}
				else {
					PCNC_Manual_Move_Y(cutter, 0);
				}
			}
			*/
			break;
		default:
			__NOP();
			break;
	}
	if (Pin == B1_Pin && state == GPIO_PIN_RESET) {
		button_stage++;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	__HAL_TIM_SET_COUNTER(BUTTON_TIMER, 0);
	HAL_TIM_Base_Start_IT(BUTTON_TIMER);
}



