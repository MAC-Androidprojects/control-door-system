/*
 * buzzer.c
 *
 *  Created on: Oct 26, 2021
 *      Author: Mohamed Emad
 */
#include "buzzer.h"
#include"gpio.h"


/*Description :
 * Function to initialize the buzzer
*/
void BUZZER_init(){
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);
}

/*Description :
 * Function to turn on the buzzer
*/
void BUZZER_on(){
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_HIGH);
}

/*Description :
 * Function to turn off the buzzer
*/
void BUZZER_off(){
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, LOGIC_LOW);
}
