/*
 * main.c
 *
 *  Created on: Oct 26, 2021
 *      Author: Mohamed Emad
 */

#define F_CPU 8000000

#include"lcd.h"
#include"keypad.h"
#include<util/delay.h>
#include<avr/io.h>
#include"std_types.h"
#include"uart.h"
#include"timer0.h"


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
uint8 get_passChar();
void state_one();
void state_two();
void state_three(uint8 next_state_flag);
void check_pass(uint8 *pass1 , uint8 *pass2);
void repeat_pass(uint8 *pass1);
void transmit_pass(uint8 state_flag);
void door_display_control();
void timer_normal_processing_mc1();
void breakdown_state_lcd();
void timer_normal_processing_error_mc1();
void changePass_state();


/*******************************************************************************
 *                          Global variables                                    *
 *******************************************************************************/
uint8 pass1[5] ;//This array holds the first user's input
uint8 pass2[5] ;//This array holds the second user's input
uint8 pressed_key;//This variable holds the pressed key
uint16 timer_flag_mc1 = 0 ;//This variable counts the ticks of the timer
uint16 timer_error_flag = 0 ;//This variable holds the ticks of the timer in err message(when a thief tries to break in)
uint8 err_stop = 0 ;//Tells us when to stop sending the err message
uint8 stop_open_mc1 = 0;//Tells us when to stop opening the door
uint8 stop_waiting_mc1 = 0 ;//Tells us when to stop waiting and start closing the door
uint8 stop_close_mc1 = 0 ;//Tells us when to stop closing the door
//This structure holds the properties of timer0 init.
timer0_config config = {0 , NORMAL , 0 ,ONE_ZERO_TWO_FOUR};



int main(void){
	//This structure holds the properties of uart init.
	uart_ConfigType conifg_uart = {DISABLED , ONE_BIT , EIGHT_BIT , 9600};
	UART_init(&conifg_uart);//Start init the uart
	LCD_init();//lcd start
	SREG |= (1 << 7);//Enable global interrupts
	while(1){
		state_one();
	}
}

/*
 * Description :
 * This function is to get the pressed key from the user and display * on screen
 */
uint8 get_passChar(){
	pressed_key = KEYPAD_getPressedKey();
	_delay_ms(400);
	LCD_displayCharacter('*');
	return pressed_key;
}

/*
 * Description :
 * This is the first state of the program when the user must enter a new password
 */
void state_one(){
	LCD_clearScreen();
	LCD_moveCursor(0, 0);
	LCD_displayString("Enter new pass.");
	LCD_moveCursor(1, 0);
	for (int i = 0 ; i<5 ; i++){
		pass1[i] = get_passChar();
	}
	repeat_pass(pass1);
}

/*
 * Description :
 * This is the first state of the program when the user must renter the password
 */
void repeat_pass(uint8 *pass1){
	LCD_clearScreen();
	LCD_moveCursor(0, 0);
	LCD_displayString("Renter the pass.");
	LCD_moveCursor(1, 0);
	for (int i =0 ; i<5 ; i++){
		pass2[i] = get_passChar();

	}
	check_pass(pass1, pass2);
}

/*
 * Description :
 * This function is to check that the first pass and the repeated are equal
 */
void check_pass(uint8 *pass1 , uint8 *pass2){
	uint8 is_matched = 0 ;
	LCD_clearScreen();
	for(int i = 0 ; i<5 ; i++){
		if(pass1[i] != pass2[i]){
			is_matched = 1 ;
		}
	}
	if (is_matched){//If they are not equal
		LCD_moveCursor(0, 0);
		LCD_displayString("Mismatch");//display mismatch for one second
		_delay_ms(1000);
		state_one();//repeat sate 1 again
	}
	else{
		transmit_pass(2);
		state_two();
	}
}
/*
 * Description :
 * This function is to transmit the password to mc2
 * params:
 * state_flag : This flag is to determine what to do after receiving the pass in mc2
 * state_flag == 0 : this means that the user wants to control the door
 * state_flag == 1 : this means that the user wants to change password
 * state_flag == 2 : Continue the program without entering a state of (open door or change pass)
 *
 */
void transmit_pass(uint8 state_flag){
	for (int i = 0 ;i<5 ; i++){
		UART_sendByte(pass1[i]);
		_delay_ms(20);
	}
	UART_sendByte(state_flag);

}
/*
 * Description :
 * This function goes to state 2 in which the user decieds wheather he wants to
 * open door or changes pass
 */
void state_two(){
	LCD_clearScreen();
	LCD_moveCursor(0,1);//To start writing on screen from begaining.
	LCD_displayString("+ : Open door");
	LCD_moveCursor(1,1);//To start writing on screen from row2.
	LCD_displayString("- : Change pass");
	pressed_key = KEYPAD_getPressedKey();//wait until user press a key and get the val
	_delay_ms(400);//wait until user release the key
	if (pressed_key == '+'){
		state_three(0);
	}
	else if(pressed_key == '-'){
		changePass_state();
	}
	else {
		state_two();//Repeat the state if the user pressed anything but the + or -
	}

}

/*
 * Description :
 * This is the state 3 of the program
 * params :
 * next_state_flag : to deciede what is the next state of the program where
 * next_state_flag == 0 : means the user wants to open the door
 *next_state_flag == 1 : means the user wants to change password
 */
void state_three(uint8 next_state_flag){
	LCD_clearScreen();
	LCD_moveCursor(0, 0);
	LCD_displayString("Enter the pass.");
	LCD_moveCursor(1, 0);
	//This loop to get the password
	for (int i = 0 ; i<5 ; i++){
		pass1[i] = get_passChar();
	}
	//Here we transmit the password and next state flag for mc2
	transmit_pass(next_state_flag);
	uint8 is_matched =UART_recieveByte();//Wait until mc2 sends the respond
	//This condition means that the 2 passwords (The entered and the one in eeprom) are not equal
	if (is_matched == 1){
		state_three(next_state_flag);//repeat state 3  with same flag
	}
	//This condition means that the 2 passwords (The entered and the one in eeprom) are equal
	else if (is_matched == 0) {
		//if next_state_flag ==0 go to door control
		if(next_state_flag == 0)door_display_control();
		//if next_state_flag ==1 go to state one again to get new password
		else if (next_state_flag ==1)state_one();
	}
	else if(is_matched == 3){
		//If the response from mc2 == 3 this means that there is a thief wants to break in
		//This function shows the err message
		breakdown_state_lcd();
	}
}

/*
 * Description :
 * This function prints err message for 1 minute then return to state 2
 */
void breakdown_state_lcd(){
	LCD_clearScreen();
	LCD_displayString("Go away !!!!");
	TIMER0_init(&config);//Start the timer
	//Set the call back when an interrupts occur
	TIMER0_setCallBack(timer_normal_processing_error_mc1);
	while(!err_stop){}//Print the message until err_stop becomes 1 (1 minute passes in timer0)
	err_stop = 0 ;//reset the flag
	state_two();
}

/*
 * Description :
 * This function prints the messages while the door is unlocking and locking again
 */
void door_display_control(){
	TIMER0_init(&config);//Start the timer
	//Set the call back when an interrupts occur
	TIMER0_setCallBack(timer_normal_processing_mc1);
	LCD_clearScreen();
	LCD_displayString("Door unlocking");
	while(!stop_open_mc1){};//Print the message until stop_open_mc1 becomes 1 (15 seconds passes in timer0)
	UART_sendByte(1);//Confirm that 15 seconds has passed to mc2
	LCD_clearScreen();//clear the screen until stop_waiting_mc1 becomes 1 (3 seconds passes in timer0)
	while(!stop_waiting_mc1){};
	UART_sendByte(2);//Confirm that 3 seconds has passed to mc2
	LCD_displayString("Door locking");
	while(!stop_close_mc1){};//Print the message until stop_close_mc1 becomes 1 (15 seconds passes in timer0)
	UART_sendByte(3);//Confirm that 15 seconds has passed to mc2
	//Reset all flags to use again
	stop_open_mc1 = 0 ;
	stop_waiting_mc1 = 0 ;
	stop_close_mc1 = 0 ;
	//Go to step 2
	state_two();
}

/*
 * Description :
 * This function implements when the user tries to change password
 */
void changePass_state(){
	LCD_clearScreen();
	LCD_moveCursor(0, 0);
	LCD_displayString("Enter the pass.");
	LCD_moveCursor(1, 0);
	for (int i = 0 ; i<5 ; i++){
		pass1[i] = get_passChar();
	}
	transmit_pass(1);//transmit the pass to mc2 with state_flag =1
	uint8 is_matched =UART_recieveByte();
	if (is_matched == 1){
		state_three(1);//If the 2 passwords didn't match go to sate 3
	}
	else if (is_matched == 0) {
		state_one();//If the 2 passwords matched go to sate one again to set new passwords
	}
	else if(is_matched == 3){
		//If the response from mc2 == 3 this means that there is a thief wants to break in
		//This function shows the err message
		breakdown_state_lcd();
	}
}


/*
 * Description :
 * This function is the callback when isr happens during locking and unlocking the door
 */
void timer_normal_processing_mc1(){
	timer_flag_mc1++;//Increment the flag for the ticks
	//This condition means that 15 seconds passed
	if(timer_flag_mc1 == 465){
		stop_open_mc1 = 1 ;
	}
	//This condition means that 3 seconds passed
	else if(timer_flag_mc1 == 555){
		stop_waiting_mc1 = 1 ;
	}
	//This condition means that 15 seconds passed
	else if (timer_flag_mc1 == 1015){
		stop_close_mc1 = 1;
		timer_flag_mc1 = 0;//Reset the timer flag
		TIMER0_Deinit();//Reset the timer
	}
}

/*
 * Description :
 * This function is the callback when isr happens during the err message
 */
void timer_normal_processing_error_mc1(){
	timer_error_flag++ ;
	if(timer_error_flag == 1830){//One minute passed
		err_stop = 1 ;
		timer_error_flag = 0 ;//Reset the timer err flag
		TIMER0_Deinit();//Reset the timer
	}
}
