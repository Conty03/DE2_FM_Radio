/*
 * Created by Vladimir Sustek, 23.6.2019
 *
 * Feel free to use/modify/whatever
 *
**/
/***********************************************************************/
/*****************    Includes   ***************************************/
/***********************************************************************/
#include "uart.h"

/***********************************************************************/
/*****************    Private macro *  *********************************/
/***********************************************************************/

/* Defined speed of UART stage in Baud, note that 57600 is maximal */
#define USART_BAUDRATE      (uint16_t)(9600)
/* Calculating prescaler to set an appropriate baud speed */
#define BAUD_PRESCALE       (uint16_t)(((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

/* Macro to substitute set bit of the UART busy line */
#define UART_BUFF_BUSY      (!(UCSR0A & (1 << UDRE0)))


#define DEBUG_PRINT        0

/***********************************************************************/
/*****************    Private data     *********************************/
/***********************************************************************/

static int uart_putchar(char c, FILE *stream);
/* Output file declaration for UART output redirection */
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);

static volatile int uart_lf_flag = 0;
static volatile uint8_t inner_buffer[UART_RX_BUFF_SIZE] = {0};
static volatile uint8_t inner_buff_tail     = 0;
static volatile uint8_t inner_buff_head    = 0;


/***********************************************************************/
/*****************    Private functions    *****************************/
/***********************************************************************/

/* STDIO redirection of printf to appropriate UART.
 * Note that TX line is always enabled and disabled
   after each transmission (function call)*/
static int uart_putchar(char c, FILE *stream) {

    /* Enable TX line */
    UCSR0B|= (1 << TXEN0);

    /* Start UART transmission - fill buffer*/
    UDR0 = c;

    while (UART_BUFF_BUSY){
        /* Do nothing - stuck here*/
    }

    /* Disable TX line */
    UCSR0B&=~(1 << TXEN0);

    return 0;
}
/* @brief Disable UART interrupt (so its ISR)
 *
 *
 */
static void uart_enter_critical(void) {
    UCSR0B &=~(1 << RXCIE0);
}
/* @brief Enable UART interrupt (so its ISR)
 *
 *
 */
static void uart_exit_critical(void) {
    UCSR0B |=(1 << RXCIE0);
}

/* @brief USART RX ISR handler
 *
 * Functions stores received characters into the
 * static buffer (owned by this module) to the array
 * inner_buffer. In case of received <LF> character
 * or if more than UART_RX_BUFF_SIZE is received
 * without any reading the uart_lf_flag is set and
 * user may handle this event as the further receive
 * is disabled (RX interrupt disabled).
 *
 * @sa uart_received_LF
 * @sa uart_copy_buffer
 *
 */
ISR(USART_RX_vect){

    uint8_t iUDR0 = UDR0;

    if (iUDR0 == '\n')
        uart_lf_flag = 1;

    inner_buffer[inner_buff_head] = iUDR0;
    inner_buff_head = (inner_buff_head + 1) % UART_RX_BUFF_SIZE;
    inner_buffer[inner_buff_head] = 0;

}

/***********************************************************************/
/*****************    Public functions    ******************************/
/***********************************************************************/
/* @brief Initialize UART peripheral
 *
 * @param [in] ISR_Mode - optional interrupt on receive enable
 * @return [None]
 *
 * Function initializes AVR USART peripheral as the 8-bit,
 * further is by default set no parity, 1 stop bit.
 * The Baud Rate is set to maximal 57600 baud and constant
 * for clock prescaler is calculated in the MACROS section
 * It is intended to access the transmit stage by stdio
 * printf function, so stdout is customized as well.
 *
 */

void UARTinitiliaze(uint8_t isr_enable_flag) {

    /* Use 8 - bit size of transaction symbol */
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
    /* Upper part of bitrate 57600*/
    UBRR0H |= (BAUD_PRESCALE >> 8);
    /* Lower part of bitrate 57600 */
    UBRR0L |= BAUD_PRESCALE;
    /* Enable Interrupt on receive */
    if(isr_enable_flag) {
            UCSR0B |= (1 << RXCIE0);
    }
    /* Force off the TX and RX for further enabling */
    //UCSR0B &= ~(1 << RXEN0) | ~(1 << TXEN0);

    /* Redirect output direction of stdio*/
    stdout = &mystdout;
}

/* @brief Has been <LF> character received ?
 *
 * @return uart_lf_flag - 0 or 1 according to the flag
 *
 * Function checks whether uart_lf_flag triggered by
 * uart_copy_buffer has been set (<LF> received). In
 * Valid case, function returns 1 and resets this flag.
 *
 */
uint8_t UARTisLFreceived(void) {
    if(uart_lf_flag) {
        uart_lf_flag = 0;
        return 1;
    }
    return 0;
}
/* @brief Copy content of received UART to buffer
 *
 * @return 0 in case of success, -1 otherwise
 *
 * Function copies content of buffer handled by
 * receiving ISR(USART_RX_vect) interrupt. After
 * the data are copied, buffer is cleared. Note
 * that function may return -1 if passed buffer
 * is NULL pointer. Maximal length of copied data
 * is determined by UART_RX_BUFF_SIZE - prevent
 * potential overflow passing sufficient buffer.
 *
 */
uint8_t UARTcopyBuffer(uint8_t * buffer, uint8_t lng){

    /* Error value */
    uint8_t length = 0xFF;

    if(buffer == NULL || lng <= UART_RX_BUFF_SIZE) {
        /* Return Error value when null pointer
           or buffer length is too small*/
        return length;
    }

    if (inner_buff_head > inner_buff_tail) {

#if DEBUG_PRINT
        /* printf redirected to UART in uart_interface.c*/
        printf("Scenario: tail < head\n");
#endif
        memcpy(buffer, (uint8_t*)inner_buffer + inner_buff_tail, inner_buff_head - inner_buff_tail);

        length = inner_buff_head - inner_buff_tail;
#if DEBUG_PRINT
        /* printf redirected to UART in uart_interface.c*/
        printf("Tail: %d, Head %d, Length: %d\n", inner_buff_tail, inner_buff_head, length);
#endif
    }
    else {
#if DEBUG_PRINT
        /* printf redirected to UART in uart_interface.c*/
        printf("Scenario: tail >= head\n");
#endif
        memcpy(buffer, (uint8_t*)inner_buffer + inner_buff_tail, UART_RX_BUFF_SIZE - inner_buff_tail);
        memcpy(buffer + (UART_RX_BUFF_SIZE - inner_buff_tail), (uint8_t*)inner_buffer, inner_buff_head);

        length = (UART_RX_BUFF_SIZE - inner_buff_tail) + inner_buff_head;
#if DEBUG_PRINT
        /* printf redirected to UART in uart_interface.c*/
        printf("Tail: %d, Head %d, Length: %d\n", inner_buff_tail, inner_buff_head, length);
#endif
    }

    inner_buff_tail = inner_buff_head;

    return length;
}

/* @brief
 *
 * @return
 *
 */
uint8_t* UARTFetchReceivedLine(uint8_t* pLength) {

    uint8_t * retP = NULL;

    /* +1 bigger for 0x0 appending - end of string for printf */
    static uint8_t outer_buffer[UART_RX_BUFF_SIZE + 1];

    if(UARTisLFreceived()) {
        uart_enter_critical();
        *pLength = UARTcopyBuffer(outer_buffer, UART_RX_BUFF_SIZE + 1);
        /* For formal printf string termination */
        outer_buffer[*pLength] = '\0';
#if DEBUG_PRINT
        /* printf redirected to UART in uart_interface.c*/
        printf("Outer buffer: %s\n", outer_buffer);
#endif
        retP = outer_buffer;
        uart_exit_critical();
    }

    return retP;
}