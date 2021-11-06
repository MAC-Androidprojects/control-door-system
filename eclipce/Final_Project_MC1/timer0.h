/*
 * timer0.h
 *
 *  Created on: Nov 2, 2021
 *      Author: Mohamed Emad
 */

#ifndef TIMER0_H_
#define TIMER0_H_
#include"std_types.h"

/*******************************************************************************
 *                      Definitions                                            *
 *******************************************************************************/

/*
 * The type that is responisoble of choosing the prescaler
 */
typedef enum {
	NO_CLK , NO_PRESCALER , EIGHT , SIX_FOUR , TWO_FIVE_SIX , ONE_ZERO_TWO_FOUR , EXT_FALLING , EXT_RISING
}timer0_prescale;

/*
 * The type that is responisoble of choosing the mode
 */
typedef enum{
	NORMAL , CTC=2
}timer0_mode;
/*
 * Description : This structure is used to configure the timer0
 * 1-Set the initial value of timer
 * 2-Set the prescaler
 * 3-Set the mode of operation (only 2 modes : overflow and compare match)
 * 4-Set the value to compare in ctc mode only
 */
typedef struct {
	uint8 init_val ;
	timer0_mode mode;
	uint8 comp_val ;
	timer0_prescale prescaler ;
}timer0_config;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description : This function is used to initialize the timer0
 * params : The structure with the initialize specifications
 */
void TIMER0_init(timer0_config *config_ptr);


/*
 * Description : This function is used to deinit the timer0
 * params : none
 */
void TIMER0_Deinit();

/*
 * Description : This function is used to set the call back which is going to be implemented in the ISR
 * params : address the function that would be implemented in ISR
 */
void TIMER0_setCallBack(void(*a_ptr)(void));

#endif /* TIMER0_H_ */
