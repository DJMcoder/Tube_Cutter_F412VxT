#include <stdint.h>
#include <stdlib.h>
#include "main.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_it.h"
#include "cutter.h"
#include "cut_code.h"

/*
 * Library for using the Plasma Cutter CNC Device.
 *
 * The Plasma Cutter CNC device designed to cut the ends of metal tubes by
 * rotating and translating it through a plasma ray, cutting a defined shape.
 *
 * The Plasma Cutter CNC has two outputs, a stepper motor and a servo.
 * The servo rotates the tube (defined as the "x" direction) and the
 * stepper motor is attached to a lead screw to translate the tube
 * (defined as the "y" direction). The cutout path is defined in flat
 * x-y coordinates, which "wrap" around the tube.
 *
 * cutter.c
 *
 *   Created on: Apr 13, 2024
 *       Author: David Melisso
 * Organization: Caltech Racing
 */

/**
 * Indicate that the PCNC has finished the cut
 */
void PCNC_Final_State(struct PCNC_Setup *setup) {
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	HAL_TIM_Base_Stop_IT(&htim7);
//	__disable_irq();
	while(1==1) {
		__NOP();
	}
}

void PCNC_Enable_Servo(struct PCNC_Setup *setup) {
	HAL_TIM_PWM_Start(setup->Servo->PWM_TIM, setup->Servo->PWM_TIM_CH);
}

void PCNC_Go_To_Origin(struct PCNC_Setup *setup) {
	setup->accepting_instructions = 0;
	PCNC_Rotate_To_X(setup, 0);
	PCNC_Translate_To_Y(setup, 0);
}

void PCNC_Startup(struct PCNC_Setup *setup) {
	HAL_GPIO_WritePin(setup->Stepper->Dir_Port, setup->Stepper->Dir_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(setup->Stepper->Pulse_Port, setup->Stepper->Pulse_Pin, GPIO_PIN_SET);
	PCNC_Enable_Servo(setup);
}

void PCNC_Begin_Cut(struct PCNC_Setup *setup) {

	setup->accepting_instructions = 1;
	HAL_TIM_Base_Start_IT(&htim7);
	PCNC_Execute_Next_Instruction(setup);
}

/**
 * Executes the next instruction in the cut code
 */
void PCNC_Execute_Next_Instruction(struct PCNC_Setup *setup) {
	if (setup->accepting_instructions <= 0) {
		return;
	}

	// do not execute until both are done executing
	if (setup->Servo->Lock > 0 || setup->Stepper->Lock > 0) {
		return;
	}

	if (setup->cur_instruction >= setup->instructions_length) {
			PCNC_Final_State(setup);
			return;
		}

	double x = setup->instructions[setup->cur_instruction][0];
	double y = setup->instructions[setup->cur_instruction][1];

	PCNC_Rotate_To_X(setup, x);
	PCNC_Translate_To_Y(setup, y);

	setup->cur_instruction++;
}


void PCNC_Servo_Unlock(struct PCNC_Setup *setup) {
	HAL_TIM_Base_Stop_IT(setup->Servo->Lock_TIM);
	setup->Servo->Lock = 0;

	PCNC_Execute_Next_Instruction(setup);
}

/**
 * Rotates the tube to a given x position. Assumes servo is not locked.
 *
 * @param setup the PCNC Servo configuration parameters
 *
 * @param x the position in inches (on the circumference) to rotate to
 */
void PCNC_Rotate_To_X(struct PCNC_Setup *setup, double x){
	// TODO: add input validation
	// lock servo to prevent two commands from running at the same time
	setup->Servo->Lock = 1;

	uint32_t old_CCR = *(setup->Servo->CCR);

	// set the servo PWM to the fraction x is of the total circumference,
	// multiplied by the max PWM
	double x_fraction = x / setup->Servo->circumference;
	double ccr_range = (double)(setup->Servo->CCR_MAX - setup->Servo->CCR_MIN);
	uint32_t new_ccr = (uint32_t)(ccr_range * x_fraction) + setup->Servo->CCR_MIN;

	if (new_ccr < setup->Servo->CCR_MIN) {
		new_ccr = setup->Servo->CCR_MIN;
	}
	else if (new_ccr > setup->Servo->CCR_MAX) {
		new_ccr = setup->Servo->CCR_MAX;
	}

	 *(setup->Servo->CCR) = new_ccr;

	 uint32_t ccr_diff = abs((int32_t)old_CCR - (int32_t)new_ccr);

	// set servo lock timer
	uint32_t lock_time = setup->Servo->Lock_Multiplier * ccr_diff;
	setup->Servo->Lock_TIM->Instance->ARR = setup->Servo->Lock_Buffer + lock_time;

	// reset timer
	__HAL_TIM_SET_COUNTER(setup->Servo->Lock_TIM, 0);
	HAL_TIM_Base_Start_IT(setup->Servo->Lock_TIM);
}

/**
 * Commits stepper action on repeating timer. Only should commit
 * at most one action at a time.
 *
 * @param setup the configuration of the stepper motor
 */
void PCNC_Stepper_ISR(struct PCNC_Setup *setup) {
	int32_t step_diff;
	if (setup->Stepper->manual_move == 0) {
		step_diff = setup->Stepper->target_steps - setup->Stepper->current_steps;
	}
	else {
		step_diff = setup->Stepper->manual_move;
	}

	if (setup->Stepper->manual_move != 0 &&
		HAL_GPIO_ReadPin(setup->Stepper->Move_Down_Port, setup->Stepper->Move_Down_Pin) != GPIO_PIN_SET &&
		HAL_GPIO_ReadPin(setup->Stepper->Move_Up_Port, setup->Stepper->Move_Up_Pin) != GPIO_PIN_SET	) {
			HAL_TIM_Base_Stop_IT(setup->Stepper->htim);
			setup->Stepper->Lock = 0;
			setup->Stepper->manual_move = 0;
			return;
	}

	// if we are at the target, don't move
	if (step_diff == 0) {
		HAL_TIM_Base_Stop_IT(setup->Stepper->htim);
		setup->Stepper->Lock = 0;

		// start next instruction
		PCNC_Execute_Next_Instruction(setup);
		return;
	}

	GPIO_PinState cur_dir = HAL_GPIO_ReadPin(setup->Stepper->Dir_Port, setup->Stepper->Dir_Pin);

	// if not going the right direction, switch direction
	// wait a delay period before pulsing
	if ((step_diff > 0 && cur_dir == GPIO_PIN_RESET) ||
		(step_diff < 0 && cur_dir == GPIO_PIN_SET)) {
		HAL_GPIO_TogglePin(setup->Stepper->Dir_Port, setup->Stepper->Dir_Pin);
		return;
	}

	// if we are moving the correct direction
	// toggle the pulse pin
	HAL_GPIO_TogglePin(setup->Stepper->Pulse_Port, setup->Stepper->Pulse_Pin);

	// if we finished the pulse, and we are not in manual mode, record the step
	if (HAL_GPIO_ReadPin(setup->Stepper->Pulse_Port, setup->Stepper->Pulse_Pin) == GPIO_PIN_SET &&
			setup->Stepper->manual_move == 0) {
		if (cur_dir == GPIO_PIN_SET) {
			setup->Stepper->current_steps++;
		}
		else {
			setup->Stepper->current_steps--;
		}
	}
}

/**
 * Manually move the tube up or down
 */
void PCNC_Manual_Move_Y(struct PCNC_Setup *setup, int8_t dir) {
	setup->Stepper->Lock = 1;
	setup->Stepper->manual_move = dir;
	HAL_TIM_Base_Start_IT(setup->Stepper->htim);
}

/**
 * Translates the tube to a given y position. Assumes stepper is not locked.
 *
 * @param setup the PCNC Stepper configuration parameters
 *
 * @param y the absolute position to translate to
 */
void PCNC_Translate_To_Y(struct PCNC_Setup *setup, double y) {
	// TODO: add input validation
	setup->Stepper->Lock = 1;
	double new_target = y/setup->Stepper->steps_to_inches;
	setup->Stepper->target_steps = (int32_t)(new_target);
	HAL_TIM_Base_Start_IT(setup->Stepper->htim);
}



/**
 * Creates a setup object from constants
 */
struct PCNC_Setup* PCNC_Get_Setup() {
	struct PCNC_Setup *setup = malloc(sizeof(struct PCNC_Setup));
	struct PCNC_Servo_Setup *servo_setup = malloc(sizeof(struct PCNC_Servo_Setup));
	struct PCNC_Stepper_Setup *stepper_setup = malloc(sizeof(struct PCNC_Stepper_Setup));

	// system setup
	setup->Servo 		 			= servo_setup;
	setup->Stepper 					= stepper_setup;
	setup->cur_instruction 			= 0;
	setup->instructions_length 		= num_instructions;
	setup->instructions 			= cut_instructions;
	setup->accepting_instructions	= 0;

	// servo setup
	setup->Servo->CCR 				= CCR;
	setup->Servo->PWM_TIM    		= PWM_TIM;
	setup->Servo->PWM_TIM_CH 		= PWM_TIM_CH;
	setup->Servo->CCR_MAX 			= CCR_MAX;
	setup->Servo->CCR_MIN 			= CCR_MIN;
	setup->Servo->Lock				= 0;
	setup->Servo->Lock_Buffer   	= Lock_Buffer;
	setup->Servo->Lock_Multiplier 	= Lock_Multiplier;
	setup->Servo->circumference 	= circumference;
	setup->Servo->Lock_TIM			= Lock_TIM;

	// stepper setup
	setup->Stepper->Pulse_Pin			= Pulse_Pin;
	setup->Stepper->Pulse_Port		= Pulse_Port;
	setup->Stepper->Lock			= 0;
	setup->Stepper->Dir_Pin		= Dir_Pin;
	setup->Stepper->Dir_Port  	= Dir_Port;
	setup->Stepper->Move_Down_Pin = Move_Down_Pin;
	setup->Stepper->Move_Down_Port = Move_Down_Port;
	setup->Stepper->Move_Up_Pin = Move_Up_Pin;
	setup->Stepper->Move_Up_Port = Move_Up_Port;
	setup->Stepper->current_steps 	= 0;
	setup->Stepper->target_steps 	= 0;
	setup->Stepper->manual_move 	= 0;
	setup->Stepper->htim			= step_htim;
	setup->Stepper->steps_to_inches = steps_to_inches;

	return setup;
}
