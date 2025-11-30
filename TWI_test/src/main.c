#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

int main(void)
{
    UARTinitiliaze(0);   // inicializace UART, bez RX interruptu

    // Po inicializaci lze používat printf()
    printf("Hello world!\r\n");

    while (1)
    {
        printf("Running...\r\n");
        _delay_ms(1000);
    }
}
