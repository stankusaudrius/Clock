/*
 * uart.c
 *
 * Purpose: This module initializes USART0 of ATMEGA2560, and creates functions for communication.
 *
 * Input: ASCII characters from terminal. Flag continue_logging should be set to 1 in main(), if not, Atmega will not acknowledge incomming input until reset button is pressed.
 *
 * Output: Binary flag string_klar is passed to main when enter is pressed and array buffe[20] is passed to main to extract input values.
 * 
 * Uses: header file uart.h, avr/interrupt.h file to define port addresses and control register addresses, and io.h for standard definitions.
 * 
 * Created: 2016-03-02
 * Author: Audrius Stankus s140506
 */ 

#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char byte;
volatile unsigned char ii;
volatile unsigned char string_klar;
volatile unsigned char continue_logging;
char buffe[20];		//stores a string of characters received from USART0

void initUSART0(unsigned char ubbr)
{
	UCSR0A = (1<<U2X0);		 //full duplex mode
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);//USART0 enable receive and transmit, enable receive complete interrupt
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);  //8bit no parity
	UBRR0L = ubbr;			//if UBRR is less than 256 it will fit into this register.
	UBRR0H = (ubbr>>8);		//if UBRR is higher than 256, its most significant bits in binary will be put on this register
}

void putchUSART0(char tx)	 //transmit
{
	while ( !(UCSR0A & (1<<UDRE0)) );	//wait for empty transmit buffer
	UDR0 = tx;				//put data into buffer
}

char getchUSART0(void)		//receive
{
	while (!(UCSR0A & (1<<RXC0)));		//wait until a character is received
	return UDR0;			//return the data
};

void putstring_USART0(char *str)
{
	while (*str)
	{
		putchUSART0(*str);
		str++;
	}
}

void getstring_USART0(unsigned char *ptr)
{
	char cx;
	while((cx=byte) != 0x0D)  	//while received character is not carriage return (enter) (0x0D)
	{
				*ptr = cx;				//assign first character to first place in array
				putchUSART0(cx);		//transmit the character if needed to show original input
				ptr++;				//go to next place in array
	}
	*ptr = 0;		//when enter is pressed, clear the array.
}

void newline(void)
{
	putchUSART0(0x0D); //to the start of the row
	putchUSART0(0x0A);// down one line

}

ISR(USART0_RX_vect)
{
	byte = UDR0;	//read received value into variable byte
	if (continue_logging == 1)	//allows disabling any input from terminal.
	{
		putchUSART0(byte);	//echo character back to terminal. May be omitted
		if (byte != 0x0D)	//if character pressed on keyboard is not Enter
		{
			buffe[ii] = byte;	//put character in array buffe
			ii++;				//and increment to next position in array for next character
		}
		else		//if enter is pressed
		{
			string_klar = 1;	//raise flag for ready string
			newline();
		}
		
	}

}


