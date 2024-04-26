/*
 * buttons.c
 *
 *  Created on: Apr 17, 2024
 *      Author: Caltech Racing
 */
#include "main.h"
#include "buttons.h"
#include "cutter.h"
#include "cut_code.h"
#include "main.h"

#define BUTTON_TIMER &htim3
#define NUM_BUTTONS 3
Button buttons[] = {
		//{ Port: TUBE_SELECT_GPIO_Port, Pin: TUBE_SELECT_Pin},
		{ Port: RST_BTN_GPIO_Port,  Pin: RST_BTN_Pin, last_state: GPIO_PIN_SET },
		{ Port: RUN_BTN_GPIO_Port, Pin: RUN_BTN_Pin, last_state: GPIO_PIN_SET },
    { Port: SEL_BTN_GPIO_Port, Pin: SEL_BTN_Pin, last_state: GPIO_PIN_SET }
};

void Debounce_Buttons() {
	HAL_TIM_Base_Stop_IT(BUTTON_TIMER);
	for (uint8_t button = 0; button < NUM_BUTTONS; button++) {
		GPIO_PinState new_state = HAL_GPIO_ReadPin(buttons[button].Port, buttons[button].Pin);
		if(new_state != buttons[button].last_state){
			Button_Debounced(buttons[button].Pin, new_state);
			buttons[button].last_state = new_state;
		}
	}
}

void Button_Debounced(uint16_t Pin, GPIO_PinState state) {
	static uint8_t button_stage = 0;

	if (Pin == RST_BTN_Pin && state == GPIO_PIN_RESET) {
	    NVIC_SystemReset();
	    return;
	}

	switch (button_stage) {
		case 0:
		  // prime
			if (Pin == RUN_BTN_Pin && state == GPIO_PIN_RESET) {
			  cutter = PCNC_Get_Setup(codes[cur_code]);
			  PCNC_Startup(cutter);
			  PCNC_Go_To_Origin(cutter);
			  HAL_GPIO_WritePin(SEL_LED_GPIO_Port, SEL_LED_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(PRIME_LED_GPIO_Port, PRIME_LED_Pin, GPIO_PIN_SET);

			}
			// change tube
			else if (Pin == SEL_BTN_Pin && state == GPIO_PIN_RESET) {
			    cur_code++;
			    if (cur_code >= num_codes) {
			        cur_code = 0;
			    }
			    if (disp_code() != HAL_OK) {
			        Error_Handler();
			    }
			}
			break;
		case 1:
			if (Pin == RUN_BTN_Pin && state == GPIO_PIN_RESET) {
				HAL_GPIO_WritePin(PRIME_LED_GPIO_Port, PRIME_LED_Pin, GPIO_PIN_RESET);
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
	if (Pin == RUN_BTN_Pin && state == GPIO_PIN_RESET) {
		button_stage++;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	__HAL_TIM_SET_COUNTER(BUTTON_TIMER, 0);
	HAL_TIM_Base_Start_IT(BUTTON_TIMER);
}



