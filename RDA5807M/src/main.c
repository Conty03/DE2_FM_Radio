#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
//#include <stddef.h>
//#include <stdio.h>
#include <twi.h>
#include <rda5807m.h>

int main(void)
{
    /* variables for CmdDispatch service */
    //uint8_t rxStrlng;
    //uint8_t* rxStrBuff = NULL;

    /* Initialize peripherals*/
    //UARTinitiliaze(UART_ISR_MODE);
    //tim_tick_initialize();
    //gpio_initialize();
    twi_init();

  RDA5807mInit();
  RDA5807mSetFreq(9950);
  RDA5807mSetVolm(8);

    /* Enable interrupts */
    sei();

    /* printf redirected to UART in uart_interface.c*/
    //printf("SYS_READY\n");

    while(1) {
        /* Check whether any new string has arrived (UART) */
        /*
        rxStrBuff = UARTFetchReceivedLine(&rxStrlng);
        if (NULL != rxStrBuff) {
            // If so and is terminated by <LF>, process it as command
            CmdDispatch(rxStrBuff, rxStrlng);
        }
        */
    }

    return 0;
}