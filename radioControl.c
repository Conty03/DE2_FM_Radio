
// -- Includes ---------------------------------------------
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <twi.h>            // I2C/TWI library for AVR-GCC
#include <uart.h>           // Peter Fleury's UART library
#include <stdio.h>          // C library. Needed for `sprintf`
#include "timer.h"
#include <rda5807m.h>


initReceiver();
	_delay_ms(10);
	
	// Load and restore last system configuration.
	loadConfig(&_currentConfig);
	updateChannel(52);
	_delay_ms(20);
	updateVolumeControl(40);
	_delay_ms(10);
