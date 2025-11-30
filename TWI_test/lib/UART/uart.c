/* uart.c - fixed version
 * Based on your original file, corrections:
 *  - use direct register assignments (avoid |= where it can leave garbage)
 *  - enable TX permanently in init
 *  - correct wait for UDRE0 in putchar
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

/* Set the desired baudrate here */
#define USART_BAUDRATE      (uint16_t)(9600)
/* Calculating prescaler to set an appropriate baud speed */
#define BAUD_PRESCALE       (uint16_t)(((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

/* Macro to substitute set bit of the UART busy line */
#define UART_BUFF_BUSY      (!(UCSR0A & (1 << UDRE0)))

#define DEBUG_PRINT        0

static int uart_putchar(char c, FILE *stream);
/* Output file declaration for UART output redirection */
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

static volatile int uart_lf_flag = 0;
static volatile uint8_t inner_buffer[UART_RX_BUFF_SIZE];
static volatile uint8_t inner_buff_tail = 0;
static volatile uint8_t inner_buff_head = 0;

/* putchar for stdout redirection */
static int uart_putchar(char c, FILE *stream) {
    /* wait for data register empty */
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = (uint8_t)c;
    return 0;
}

static void uart_enter_critical(void) {
    UCSR0B &= ~(1 << RXCIE0);
}
static void uart_exit_critical(void) {
    UCSR0B |= (1 << RXCIE0);
}

/* RX ISR -> push to ring buffer */
ISR(USART_RX_vect) {
    uint8_t ch = UDR0;

    if (ch == '\n') uart_lf_flag = 1;

    inner_buffer[inner_buff_head] = ch;
    inner_buff_head = (inner_buff_head + 1) % UART_RX_BUFF_SIZE;
    inner_buffer[inner_buff_head] = 0;
}

/* Public functions */

void UARTinitiliaze(uint8_t isr_enable_flag) {
    /* 8N1 frame: UCSZ01:0 = 3 */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    /* Baud rate - write directly (do not |=) */
    UBRR0H = (uint8_t)(BAUD_PRESCALE >> 8);
    UBRR0L = (uint8_t)(BAUD_PRESCALE & 0xFF);

    /* Clear UCSR0A defaults (no double speed) */
    UCSR0A = 0;

    /* Enable TX (and RX if you want to receive). RX interrupt optional */
    UCSR0B = (1 << TXEN0); /* TX enabled permanently */
    if (isr_enable_flag) {
        UCSR0B |= (1 << RXEN0) | (1 << RXCIE0); /* enable RX and RX interrupt */
    } else {
        /* if you want RX without ISR, enable RXEN0: UCSR0B |= (1<<RXEN0); */
    }

    /* Redirect stdout to uart */
    stdout = &mystdout;

    /* init buffer indices */
    inner_buff_head = inner_buff_tail = 0;
    memset((uint8_t*)inner_buffer, 0, sizeof(inner_buffer));
}

uint8_t UARTisLFreceived(void) {
    if (uart_lf_flag) {
        uart_lf_flag = 0;
        return 1;
    }
    return 0;
}

uint8_t UARTcopyBuffer(uint8_t * buffer, uint8_t lng) {
    uint8_t length = 0xFF;

    if (buffer == NULL || lng <= UART_RX_BUFF_SIZE) {
        return length;
    }

    if (inner_buff_head >= inner_buff_tail) {
        /* contiguous */
        length = inner_buff_head - inner_buff_tail;
        memcpy(buffer, (uint8_t*)inner_buffer + inner_buff_tail, length);
    } else {
        /* wrapped */
        uint8_t first = UART_RX_BUFF_SIZE - inner_buff_tail;
        memcpy(buffer, (uint8_t*)inner_buffer + inner_buff_tail, first);
        memcpy(buffer + first, (uint8_t*)inner_buffer, inner_buff_head);
        length = first + inner_buff_head;
    }

    inner_buff_tail = inner_buff_head;
    return length;
}

uint8_t* UARTFetchReceivedLine(uint8_t* pLength) {
    uint8_t * retP = NULL;
    static uint8_t outer_buffer[UART_RX_BUFF_SIZE + 1];

    if (UARTisLFreceived()) {
        uart_enter_critical();
        *pLength = UARTcopyBuffer(outer_buffer, UART_RX_BUFF_SIZE + 1);
        outer_buffer[*pLength] = '\0';
        retP = outer_buffer;
        uart_exit_critical();
    }
    return retP;
}