/**
 * @brief Defines the setup configuration for the Plasma Cutter CNC Device
 */
struct PCNC_Setup {
	struct PCNC_Servo_Setup *Servo;
	struct PCNC_Stepper_Setup *Stepper;
	double (*instructions)[2];
	uint32_t instructions_length;
	uint32_t cur_instruction;
	uint8_t  accepting_instructions;
};



/**
 * @brief Defines the setup configuration for the rotational servo
 */
struct PCNC_Servo_Setup {
	/* The circumference of the tube in the device (inches) */
	double circumference;

	/*
	 * Servo PWM compare register, controls the duty cycle.
	 * Assumed that each tick of CCR corresponds to one tick of precision on
	 * the Servo.
	 */
	volatile uint32_t *CCR;

	TIM_HandleTypeDef *PWM_TIM;
	uint32_t PWM_TIM_CH;

	/* The CCR equivalent to 360 degree rotation */
	uint32_t CCR_MAX;

	/* The CCR equivalent to 0 degree rotation */
	uint32_t CCR_MIN;

	/* Timer which locks out the Servo, allowing one command to finish
	 * before starting the next */
	TIM_HandleTypeDef *Lock_TIM;

	/*
	 * Number of Lock_TIM ticks equivalent to one Servo_CCR PWM tick
	 * If timers are set up properly, this value should be one
	 */
	uint32_t Lock_Multiplier;

	/*
	 * Number of extra ticks to keep the servo locked
	 */
	uint32_t Lock_Buffer;

	/* 1 if servo is still performing an operation, 0 otherwise */
	volatile uint8_t Lock;
};

/**
 * @brief Defines the setup configuration for the translational stepper motor
 * attatched to a lead screw
 */
struct PCNC_Stepper_Setup {
	/* Current number of rotational steps (corresponds to number of pulses) */
	volatile int32_t current_steps;

	/* Rotational step position that the stepper will move to */
	volatile int32_t target_steps;

	/* -1 if manually moving down, 1 if manually moving up, 0 otherwise */
	volatile int8_t manual_move;

	/* Number of inches that one rotational step translates the tube */
	double steps_to_inches;

	/* 1 if stepper is still performing an operation, 0 otherwise */
	volatile uint8_t Lock;

	/* Timer which coordinates the pulses */
	TIM_HandleTypeDef *htim;

	/* Pin for doing stepper pulses */
	uint16_t 		Pulse_Pin;
	GPIO_TypeDef *Pulse_Port;

	/* Pin for setting stepper direction */
	uint16_t 		Dir_Pin;
	GPIO_TypeDef *Dir_Port;

	/* Pin for moving up */
	uint16_t 		Move_Up_Pin;
	GPIO_TypeDef *Move_Up_Port;

	/* Pin for moving up */
	uint16_t 		Move_Down_Pin;
	GPIO_TypeDef *Move_Down_Port;
};

void PCNC_Final_State(struct PCNC_Setup *setup);
void PCNC_Go_To_Origin(struct PCNC_Setup *setup);
void PCNC_Startup(struct PCNC_Setup *setup);
void PCNC_Begin_Cut(struct PCNC_Setup *setup);
void PCNC_Execute_Next_Instruction(struct PCNC_Setup *setup);
void PCNC_Servo_Unlock(struct PCNC_Setup *setup);
void PCNC_Rotate_To_X(struct PCNC_Setup *setup, double x);
void PCNC_Stepper_ISR(struct PCNC_Setup *setup);
void PCNC_Manual_Move_Y(struct PCNC_Setup *setup, int8_t dir);
void PCNC_Translate_To_Y(struct PCNC_Setup *setup, double y);
struct PCNC_Setup* PCNC_Get_Setup();
