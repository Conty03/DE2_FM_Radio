/*
 * SI4703.c
 *
 * Created: 2018-05-29
 * Author: kiki
 * Reference: https://github.com/eziya/AVR_SI4703/tree/master
 * 
 */ 

#ifndef F_CPU
#define F_CPU	8000000L
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
/*#include <string.h>*/
#include <stdint.h>

#include "SI4703.h"
#include "128A_USART.h"
#include <gpio.h>

#define CHANNEL1	104.7
#define CHANNEL2	106.2
#define MONO		true

/*Input pins used for buttons*/
#define DDR		DDRD
#define PORT	PORTD
#define PIN		PIND
#define D2		PD2		// Up
#define D3		PD3 	// Down
#define D4		PD4 	// Seek

uint8_t volume = 8;


/*char msg[150];*/

int main(void)
{	
    gpio_mode_input_pullup(&DDRD, 2);
	gpio_mode_input_pullup(&DDRD, 3);
	gpio_mode_input_pullup(&DDRD, 4);

	/*USART*/
	/*USART0_Init();	
	
	if(!SI4703_Init())
	{
		memset(msg, 0, sizeof(msg));
		sprintf(msg, "SI4703_Init failed.\r\n");
		USART0_TxBuffer((uint8_t *)msg, strlen(msg));		
		
		while(1)
		{
			
		}
	}
	else
	{
		memset(msg, 0, sizeof(msg));
		sprintf(msg, "SI4703_Init succeeded.\r\n");
		USART0_TxBuffer((uint8_t *)msg, strlen(msg));	
	}*/
	
	SI4703_Init();
	SI4703_SeekUp();

	//SI4703_GetFrequency();
 
    while (1) 
    {
		/*Karel*/
	}
}