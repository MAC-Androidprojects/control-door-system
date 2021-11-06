/*
 * buzzer.h
 *
 *  Created on: Oct 26, 2021
 *      Author: Mohamed Emad
 */

#ifndef BUZZER_H_
#define BUZZER_H_
#include"gpio.h"

/*******************************************************************************
 *                      Definitions                                            *
 *******************************************************************************/

#define BUZZER_PORT PORTD_ID
#define BUZZER_PIN PIN7_ID


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*Description :
 * Function to initialize the buzzer
*/
void BUZZER_init();
/*Description :
 * Function to turn on the buzzer
*/
void BUZZER_on();
/*Description :
 * Function to turn off the buzzer
*/
void BUZZER_off();


#endif /* BUZZER_H_ */
