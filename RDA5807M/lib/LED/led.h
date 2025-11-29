#ifndef GPIO_INTERFACE_H_INCLUDED
#define GPIO_INTERFACE_H_INCLUDED

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>


#define ONBOARD_LED     (uint8_t)(PIN5)

#endif // GPIO_INTERFACE_H_INCLUDED
void gpio_initialize(void);
void gpio_LED_toggle(void);
void gpio_LED_set(bool state);