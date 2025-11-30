#ifndef UART_INTERFACE_H_INCLUDED
#define UART_INTERFACE_H_INCLUDED

/***********************************************************************/
/*****************    Includes   ***************************************/
/***********************************************************************/
#include <string.h>

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
/***********************************************************************/
/*****************    Private macro *  *********************************/
/***********************************************************************/

/* Defined speed of UART stage in Baud, note that 57600 is maximal */
#define USART_BAUDRATE      (uint16_t)(2400)
/* Calculating prescaler to set an appropriate baud speed */
#define BAUD_PRESCALE       (uint16_t)(((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

/* Macro to substitute set bit of the UART busy line */
#define UART_BUFF_BUSY      (!(UCSR0A & (1 << UDRE0)))
/***********************************************************************/
/*****************    Public macro *************************************/
/***********************************************************************/

#define RX_ISR_ENAB             (uint8_t)(1u)
#define RX_ISR_DISAB            (uint8_t)(0u)
#define UART_RX_BUFF_SIZE       (uint8_t)(16u)

#define UART_ISR_MODE           (uint8_t)(1u)
/***********************************************************************/
/*****************    Public functions    ******************************/
/***********************************************************************/

void UARTinitiliaze(uint8_t isr_enable_flag);
uint8_t UARTisLFreceived(void);
uint8_t UARTcopyBuffer(uint8_t * buffer, uint8_t lng);
uint8_t* UARTFetchReceivedLine(uint8_t* pLength);
#endif // UART_INTERFACE_H_INCLUDED