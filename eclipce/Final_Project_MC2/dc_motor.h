/*
 * dc_motor.h
 *
 *  Created on: Oct 5, 2021
 *      Author: Mohamed Emad
 */

#include"std_types.h"

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum {
	STOP , ANTI_CLK_WISE, CLK_WISE
}DcMotor_State;

/*******************************************************************************
 *                         Definitions                                   *
 *******************************************************************************/
#define MOTOR_PORT PORTD_ID
#define MOTOR_INPUT1 PIN5_ID
#define MOTOR_INPUT2 PIN6_ID



/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for initialize the ADC driver.
 */
void DcMotor_Init(void) ;
/*
 * Description :
 * Function responsible for the rotation of the motor the ADC driver.
 * Inputs :
 * the state of the motor (clk_wise , anti-clk_wise , stop)
 * the speed in precent
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);
/*
 * Description :
 * Function responsible for generating the wave of pwm.
 * Inputs :
 * The duty cycle in precent
 */
void Timer0_PWM_Init(uint8 duty_cycle);


#endif /* DC_MOTOR_H_ */
