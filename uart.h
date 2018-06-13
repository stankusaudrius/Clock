	/*
	* uart.h
	*
	* Purpose: This module initializes USART0 of ATMEGA2560, and creates functions for communication.
	*
	* Input: variable continue_logging should be set to 1 in main(), if not, Atmega will not acknowledge incomming input until reset button is pressed.
	*
	* Output: Binary flag string_klar is passed to main when enter is pressed together with buffe[] to extract input values.
	*
	* Uses: avr/interrupt.h file to define port addresses and control register addresses, and io.h for standard definitions.
	*
	* Created: 2016-03-02
	* Author: Audrius Stankus s140506
	*/ 


#ifndef UART_H_
#define UART_H_

#endif /* UART_H_ */

volatile unsigned char byte;
volatile unsigned char ii;
volatile unsigned char string_klar;
volatile unsigned char continue_logging;
char buffe[20];
#define MY_UBRR 103  //defining UBRR which will be assigned to register UBRR0. 103 gives baud rate of 19200


void initUSART0(unsigned char ubbr); //Initializes USART0 with chosen UBBR value, full duplex, 8bit, 1 stop bit, no parity
void putchUSART0(char tx);		//sends a character to USART0 with polling method
char getchUSART0(void);		//receive characters from USART0 with polling method
void putstring_USART0(char *str); //send a string, uses putchUSART0(char tx);
void getstring_USART0(unsigned char *ptr);	//receive a string, uses getchUSART0();
void newline(void);		//sends 0x0D (carriage return) and 0x0A (line feed).



