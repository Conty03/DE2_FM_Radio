/*
 * SI4703.c
 *
 * Reference: https://github.com/eziya/AVR_SI4703/tree/master
 * 
 */ 

#ifndef F_CPU
#define F_CPU	8000000L
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "SI4703.h"
#include "128A_USART.h"

#define CHANNEL1	104.7
#define CHANNEL2	106.2

/*char msg[150];*/

int main(void)
{	
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
		
	SI4703_SeekUp();
 
    while (1) 
    {
	/* Replace with your application code */
	}
}