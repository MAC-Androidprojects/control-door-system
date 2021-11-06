/*
 * timer0.c
 *
 *  Created on: Nov 2, 2021
 *      Author: Mohamed Emad
 */

#include"timer0.h"
#include "avr/io.h" /* To use the UART Registers */
#include <avr/interrupt.h>
#include "std_types.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application*/
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}

}
ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application*/
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}

}




/*
 * Description : This function is used to initialize the timer0
 * params : The structure with the initialize specifications
 */
void TIMER0_init(timer0_config *config_ptr){
	TCNT0 = config_ptr->init_val; /* Set Timer initial value to 0 */
	TCCR0 |= (1<<FOC0) ; /* non-pwm mode */
	if (config_ptr->mode == NORMAL){
		TIMSK = (1<<TOIE0); /* Enable Timer0 Overflow Interrupt */
	}
	else {
		TIMSK = (1 << OCIE0);/* Enable Timer0 compare mode Interrupt */
		OCR0 = config_ptr->comp_val ;
	}
	TCCR0 = (TCCR0 & 0xF8) | (config_ptr->prescaler & 0x07); /*Set the prescaler in first 3 bits of TCCR0*/

}

/*
 * Description : This function is used to deinit the timer0
 * params : none
 */
void TIMER0_Deinit(){
	TCNT0 = 0 ;
	OCR0 = 0;
	TIMSK = 0 ;
	TCCR0 = 0;
}

/*
 * Description : This function is used to set the call back which is going to be implemented in the ISR
 * params : address the function that would be implemented in ISR
 */
void TIMER0_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}


