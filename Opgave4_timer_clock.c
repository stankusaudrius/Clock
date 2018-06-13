/*
 * Opgave4_time_clock.c
 *
 * Purpose: This module creates communication between PC and USART0 of ATMega2560. Program is set up to request input of current time, 
 * and continue counting the time as a regular clock. If input error is encountered, request for new input is generated. When clock is 
 * working it can only be interrupted by external interrupt INT3, which resets the program.
 * 
 * Input: MY_UBBR is defined in header file uart.h, it is chosen based on desired baud rate. UBBR = F_CPU/8/BAUD - 1. Variable ii and flag 
 * string_klar are received from module uart.c as well as array "buffe[]".
 *
 * Output: Time in format hh:mm:ss transmitted to USART0 every second, or messages requesting for input, reporting error. Flag continue_logging 
 * is passed back to module uart.c.
 * 
 * Uses: IO definitions file io.h, interrupt library avr/interrupt.h, uart.c module header file uart.h; stdio.h 
 * header file for sprintf and sscanf functions.
 *
 * Created: 2016-04-07
 * Author : Audrius
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include "uart.h"
#include <stdio.h>
#include <avr/interrupt.h>

#define button_port_ddr		DDRD
#define button_port			PORTD
#define button_port_input	PIND
#define button_pin			3
#define LED_port_ddr		DDRB
#define LED_port			PORTB
#define LED_pin				7

int hours, minutes, seconds;
char result[20];
unsigned char update_time;		//Used as a flag to enable counting time or not
volatile unsigned char continue_logging;	//Flag to enable or disable input from terminal
volatile unsigned char reset_pressed = 1;
unsigned short interrupt_taeller;
volatile unsigned char timer_flag;
enum state {Initialization, Idle, Clock};
volatile char state = Initialization;


void init_ports(void);		//Initialize ports
void initInterrupts(void);	//Initialize Interrupts
void time_counter(void);	//Checks which variable/s to increase/reset
void init_timer(void);		//Initializes timer/counter1 to CTC mode; prescaler 64, OCR1A 250, sets up interrupt TIMER1 COMPA to trigger every 1/1000 second. 

int main(void)
{

	initUSART0(MY_UBRR);		//Initialize USART0
	initInterrupts();
	init_ports();
	init_timer();

	
    while (1) 
	{
		switch (state)
		{
		case Initialization:
			newline();		//sends 0x0D and 0x0A to go to the next line
			newline();
			putstring_USART0("Please input current time in format hh:mm:ss, with collon");
			newline();
			continue_logging = 1;
			state = Idle;
		break;

		case Idle:
		
			if (string_klar == 1 )			//if enter was pressed or carriage return command sent
			{
				if (ii != 8)		//if received string is not 8 characters long
				{
					newline();
					putstring_USART0("Error, try again");		//report error
					newline();
					newline();
				}
				else		//if string is correct length
				{
					sscanf(buffe, "%02d:%02d:%02d", &hours, &minutes, &seconds ); //function to scan input string and extract decimal values from there
					ii = 0;
						
					if ((!(hours>=0 && hours<=23) || !(minutes>=0 && minutes<=59) || !(seconds>=0 && seconds<=59)))		//if input values are not 0-23, 0-59 and 0-59, reports error
					{
						newline();
						putstring_USART0("Error, try again");
						newline();
						newline();
						continue_logging = 1;
					}
				}
				ii=0;		//put first character of next string again on first position in array buffe.
				string_klar = 0 ;		//reset so that if statement is only executed once when new string is received

			}
		break;
	
		case Clock:
			if (timer_flag == 1 )		//if all is ok and time needs to be updated
			{
				sprintf(result, "%02d:%02d:%02d\b\b\b\b\b\b\b\b", hours, minutes, seconds);		//convert decimal values of time back to string, and take cursor 8 positions back
				putstring_USART0(&result[0]);		//put the time on terminal
				time_counter();			//simulating the clock - which time variable/s to increase / reset.
				continue_logging = 0;	//disables any additional input from keyboard until reset button is pressed.
				timer_flag = 0;			
			}
		break;
		}
		
	}
}

void initInterrupts(void)
{
	DDRD &= ~(1<<PD3);				//External interrupt INT3 pin input
	EICRA |= (1<<ISC31);			//Interrupt activates when INT3 on falling edge
	EIMSK |= (1<<INT3);				//External Interrupt Mask Register. Enable INT3 interrupt
	
	TIMSK1 = (1<<OCIE1A);			//timer counter 1 compare match A interrupt enable
	
	sei();							//Enable global interrupts
}

void init_ports(void)
{
	button_port_ddr &= ~(1<<button_pin);	//Button pin as input
	button_port |= (1<<button_pin);  //Button pin as active low
}
void init_timer(void)
{
	TCCR1B = (1<<CS11) | (1<<CS10) | (1<<WGM12); //timer/counter 1 prescaler 64; CTC mode
	OCR1A = 250;					//16000000Hz/prescaler*1000Hz = 250, compare matches with TCNT1 every 1ms.
}

void time_counter(void)
{
	if (seconds==59)		//is seconds reaches 59
	{
		if (minutes==59)	//if minutes reaches 59
		{
			if(hours==23)	//if hours reaches 23
			{
				hours=0;	//reset to 0
				minutes=0;	//reset to 0
				seconds=0;	//reset to 0
			}
			else
			{
				hours++;	//if seconds and minutes reaches 59, but hours is not more than 22, increase hours by one
				minutes=0;	//reset minutes
				seconds=0;	//reset seconds
			}
		}
		else
		{
			minutes++;		//if seconds reached 59 and minutes is less than 59, increase minutes by one
			seconds=0;		//reset seconds
		}
	}
	else
	{
		seconds++;			//if seconds hasn't reached 59. increase by one
	}
}

ISR(INT3_vect) //Interrupt fires if button is pressed
{
		state = Initialization;
}

ISR(TIMER1_COMPA_vect) //Interrupt fires every 1/1000 second
{
	interrupt_taeller++;

	if (interrupt_taeller == 999) 
	{
		timer_flag = 1;
		interrupt_taeller = 0;
	}
}