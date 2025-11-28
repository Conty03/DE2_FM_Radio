#ifndef TIM_DEVICE_H_INCLUDED
#define TIM_DEVICE_H_INCLUDED

/* Standard libraries */
#include <stdint.h>
#include <stdbool.h>

/* AVR libraries */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "tim_device.h"

/* Function related to System Tick */
void tim_tick_initialize(void);
uint32_t tim_tick_get(void);
void tim_block_ms(uint32_t Delay);

/* Function related to System Tick -
 * intended to be implemented by developer */
void tim_blocked_dothis(void);

/* Function related to Buzzer Beep */
void tim_beep_D6D4_initialize(void);
void tim_beep_D6D4_sound(bool state);
void tim_beep_D6D4_freq(uint16_t freq);
#endif // TIM_DEVICE_H_INCLUDED