/*
 * main_MC2.c
 *
 *  Created on: Nov 1, 2021
 *      Author: Mohamed Emad
 */
#define F_CPU 8000000

#include"uart.h"
#include<avr/io.h>
#include<util/delay.h>
#include"external_eeprom.h"
#include"twi.h"
#include"std_types.h"
#include"dc_motor.h"
#include"timer0.h"
#include"buzzer.h"


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void receive_pass(uint8 saveFlag);
void save_pass();
void read_pass();
void check_pass(uint8 *pass1 , uint8 *pass2 , uint8 state_flag);
void door_control();
void timer_normal_processing();
void breakdown_state_buzzer();
void timer_normal_processing_error_mc2();

/*******************************************************************************
 *                          Global variables                                    *
 *******************************************************************************/
uint8 pass[5] ;//This array holds the user's input (password)
uint8 stored_pass[5];//This array holds the stored password in eeprom
uint16 timer_flag = 0 ;//This variable counts the ticks of the timer
uint16 timer_error_flag = 0 ;//This variable holds the ticks of the timer in err message(when a thief tries to break in)
uint8 err_stop = 0 ;//Tells us when to stop sending the err message
uint8 stop_open = 0;//Tells us when to stop opening the door
uint8 stop_waiting = 0 ;//Tells us when to stop waiting and start closing the door
uint8 stop_close = 0 ;//Tells us when to stop closing the door
//This variable holds the value of the timer from mc1 to confirm that the 2 mcs have same value of timer while unlocking and locking
uint8 recieved_timer_val = 0;
//This structure holds the properties of timer0 init.
timer0_config config = {0 , NORMAL , 0 ,ONE_ZERO_TWO_FOUR};
/*This variable holds the number of wrong entered pass */
unsigned int num_of_repeat = 0 ;



int main(){
	//This structure holds the properties of uart init.
	uart_ConfigType conifg_uart = {DISABLED , ONE_BIT , EIGHT_BIT , 9600};
	UART_init(&conifg_uart);//Start init the uart
	//This structure holds the properties of i2c init.
	i2c_ConfigType config_i2c = {500000 , 0x01};
	TWI_init(&config_i2c);//Init the i2c
	DcMotor_Init();//Dc motor init
	SREG |= (1 << 7);//Enable global interrupts
	while(1){
		receive_pass(1);
	}
}

/*
 * Description :
 * This function is to recieve the password from mc1
 * params :
 * saveFlag : This flag decides wheather the pass should be saved in eeprom or not
 * saveFlag ==0 : Don't save in eeprom
 * saveFlag ==1 : Save the pass in eeprom
 */
void receive_pass(uint8 saveFlag){
	/*This variable holds the 6th bit from the mc1 to determine the next state
	 * this variable goes as parameter to the next function
	 * state_flag == 0 : we want to control the door
	 * state_flag == 1 : we want to change password
	 * */
	uint8 state_flag ;
	/*This loop receives the password*/
	for(int i = 0 ; i< 5 ; i++){
		pass[i] = UART_recieveByte();
	}
	state_flag = UART_recieveByte();
	if (saveFlag == 1){//To save the password in eeprom
		save_pass();
	}
	//if we don't want to save the password we just check
	else if(saveFlag == 0){
		//Check the pass with the stored one
		check_pass(pass, stored_pass , state_flag);
	}
}
/*
 * Description :
 * This function saves in EEprom
 */
void save_pass(){
	/*This loop saves the password in EEprom*/
	for(int i = 0 ; i< 5 ; i++){
		EEPROM_writeByte(0x0311+i , pass[i]);//Save the password
		_delay_ms(10);
	}
	read_pass();
	receive_pass(0);
}
/*
 * Description :
 * This function reads the password saved in eeprom
 */
void read_pass(){
	for(int i = 0 ; i< 5 ; i++){
		EEPROM_readByte(0x0311+i , stored_pass + i);
		_delay_ms(10);
	}
}

/*
 * Description :
 * This function checks 2 passwords with each other
 * params :
 * pass1[5] : Array holds the first password that we want to check
 * pass2[5] :Array holds the second password that we want to check
 * state_flag :this variable deciedes the next state
 *state_flag == 0 : we want to control the door
 *state_flag == 1 : we want to change password
 *
 */
void check_pass(uint8 *pass1 , uint8 *pass2 , uint8 state_flag){
	uint8 is_matched = 0 ;
	for(int i = 0 ; i<5 ; i++){
		if(pass1[i] != pass2[i]){
			is_matched = 1 ;
		}
	}
	//If the passwords didn't match
	if (is_matched){
		num_of_repeat++ ;
		if(num_of_repeat == 3){
			//reset num_of_repeat
			num_of_repeat = 0;
			//Tells mc1 that there is a thief
			UART_sendByte(3);
			//turn on the buzzer for 1 minute
			breakdown_state_buzzer();
		}
		else {
			UART_sendByte(is_matched);//Tells mc1 the result of checking
			receive_pass(0);//Go back to receive_pass again

		}

	}
	else{
		num_of_repeat = 0;
		UART_sendByte(is_matched);//Tells mc1 the result of checking
		if(state_flag == 0){//Start door unlocking
			door_control();
		}
		else if(state_flag == 1) {
			//Go back to receive_pass (if the user wants to change pass)
			receive_pass(1);
		}
	}
}
/*
 * Description:
 * This function simulates the door unlocking step
 */
void door_control(){
	TIMER0_init(&config);//Start the timer
	//Set the call back when an interrupts occur
	TIMER0_setCallBack(timer_normal_processing);
	/*First parameter is the direction of rotating*/
	/* Second parameter is the speed of motor in precent*/
	DcMotor_Rotate(CLK_WISE , 30);//Move clk wise for 15 seconds
	recieved_timer_val = UART_recieveByte();//recieve the timer0 state of mc1
	while(!stop_open || recieved_timer_val != 1){};
	DcMotor_Rotate(STOP , 0);//Stop the motor for 3 seconds
	recieved_timer_val = UART_recieveByte();
	while(!stop_waiting || recieved_timer_val != 2){};
	DcMotor_Rotate(ANTI_CLK_WISE , 30);//Move anti clk wise for 15 seconds
	recieved_timer_val = UART_recieveByte();
	while(!stop_close || recieved_timer_val != 3){};
	DcMotor_Rotate(STOP , 0);
	//Reset all flags to use again
	stop_open = 0 ;
	stop_waiting = 0 ;
	stop_close = 0 ;
	//Go back to state of recieving pass
	receive_pass(0);

}

void breakdown_state_buzzer(){
	TIMER0_init(&config);//Start the timer
	//Set the call back when an interrupts occur
	TIMER0_setCallBack(timer_normal_processing_error_mc2);
	BUZZER_init();//Init the buzzer
	BUZZER_on();//Start the buzzer
	while(!err_stop){}//wait until 1 minute pass
	BUZZER_off();//close the buzzer
	err_stop = 0 ;//reset the flag
	receive_pass(0);	//Go back to state of recieving pass

}
/*
 * Description :
 * This function is the callback when isr happens during locking and unlocking the door
 */
void timer_normal_processing(){
	timer_flag++;//Increment the flag for the ticks
	//This condition means that 15 seconds passed
	if(timer_flag == 465){
		stop_open = 1 ;
	}
	//This condition means that 3 seconds passed
	else if(timer_flag == 555){
		stop_waiting = 1 ;
	}
	//This condition means that 15 seconds passed
	else if (timer_flag == 1015){
		stop_close = 1;
		timer_flag = 0;//Reset the timer flag
		TIMER0_Deinit();//Reset the timer
	}
}
/*
 * Description :
 * This function is the callback when isr happens during the err message
 */
void timer_normal_processing_error_mc2(){
	timer_error_flag++ ;
	//One minute passed
	if(timer_error_flag == 1830){
		err_stop = 1 ;
		timer_error_flag = 0 ;//Reset the timer err flag
		TIMER0_Deinit();//Reset the timer
	}
}




