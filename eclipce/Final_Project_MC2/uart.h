 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"


/*******************************************************************************
 *                      Definitions                                            *
 *******************************************************************************/
/*Determine the parity bit*/
typedef enum {
	DISABLED , EVEN = 2 , ODD
}uart_parity_mode;


/*Determine the number of stop bits per frame*/
typedef enum {
	ONE_BIT , TWO_BIT
}uart_stop_bit;

/*Determine the number of data bits per frame*/
typedef enum {
	FIVE_BIT , SIX_BIT , SEVEN_BIT , EIGHT_BIT , NINE_BIT = 7
}uart_data_range;

/*Description :
 * This structure is to make the configurations for the uart
 * 1-Determine the parity bit
 * 2-Determine the number of stop bits per frame
 * 3-Determine the number of data bits per frame
 * 4-Determine the baud rate*/
typedef struct
{
	uart_parity_mode parity ;
	uart_stop_bit stopBit ;
	uart_data_range dataRange;
	uint32 baud_rate;

}uart_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const uart_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
