#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
//#include <stddef.h>
#include <stdio.h>
#include <twi.h>
#include <rda5807m.h>
#include <led.h>
#include <util/delay.h>
#include <uart.h>
#include <timer.h>

int main(void)
{
    /* variables for CmdDispatch service */
    /*uint8_t rxStrlng;
    uint8_t* rxStrBuff = NULL;*/

    /* Initialize peripherals*/
    UARTinitiliaze(UART_ISR_MODE);
    tim_tick_initialize();
    _delay_ms(1000);

    gpio_initialize();
    twi_init();
    RDA5807mInit();

    RDA5807mSetFreq(9950);
    RDA5807mSetVolm(8);

    /* Enable interrupts */
    sei();

    /* printf redirected to UART in uart_interface.c*/
    printf("SYS_READY\n");

    /* I2C scanner */
     {
        uint8_t found[32];
        size_t n = twi_i2cScanner(found, sizeof(found));
        if (n == 0) {
            printf("I2C devices: 0 - none found\r\n");
            for (int i = 0; i < 3; ++i) {
                gpio_LED_toggle();
                _delay_ms(200);
                gpio_LED_toggle();
                _delay_ms(200);
            }
        } else {
            printf("I2C devices: %u\r\n", (unsigned)n);
            for (size_t i = 0; i < n && i < sizeof(found); ++i) {
                printf("0x%02X ", (unsigned)found[i]);
            }
            printf("\r\n");
        }
    }

    while(1) {
        _delay_ms(1000);
        
        gpio_LED_toggle();
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