/*
 * dc_motor.c
 *
 *  Created on: Oct 5, 2021
 *      Author: Mohamed Emad
 */
#include"dc_motor.h"
#include"gpio.h"
#include"std_types.h"
#include<avr/io.h>
/*
 * Description :
 * A function to initialize the dc motor
 */
void DcMotor_Init(void){
	GPIO_setupPinDirection(MOTOR_PORT, MOTOR_INPUT1, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_PORT, MOTOR_INPUT2, PIN_OUTPUT);
	GPIO_writePin(MOTOR_PORT, MOTOR_INPUT1,0);
	GPIO_writePin(MOTOR_PORT, MOTOR_INPUT2,0);

}
/*
 * Description :
 * Function responsible for the rotation of the motor the ADC driver.
 * Inputs :
 * the state of the motor (clk_wise , anti-clk_wise , stop)
 * the speed in precent
 */

void DcMotor_Rotate(DcMotor_State state,uint8 speed){
	Timer0_PWM_Init(speed);
	switch(state){
	case STOP :
		GPIO_writePin(MOTOR_PORT, MOTOR_INPUT1,LOGIC_LOW);
		GPIO_writePin(MOTOR_PORT, MOTOR_INPUT2,LOGIC_LOW);
		break;
	case ANTI_CLK_WISE :
		GPIO_writePin(MOTOR_PORT, MOTOR_INPUT1,LOGIC_HIGH);
		GPIO_writePin(MOTOR_PORT, MOTOR_INPUT2,LOGIC_LOW);
		break;
	case CLK_WISE :
		GPIO_writePin(MOTOR_PORT, MOTOR_INPUT1,LOGIC_LOW);
		GPIO_writePin(MOTOR_PORT, MOTOR_INPUT2,LOGIC_HIGH);
		break;

	}
}

/*
 * Description :
 * Function responsible for generating the wave of pwm.
 * Inputs :
 * The duty cycle in precent
 */
void Timer0_PWM_Init(uint8 duty_cycle)
{
	TCNT0 = 0; // Set Timer Initial Value to 0
	//Get the compare value
	uint8 comp_val = (uint8)((duty_cycle*0.01) * 255);
	OCR0  = comp_val; // Set Compare Value

	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);
	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}
