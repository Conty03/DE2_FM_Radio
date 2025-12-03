#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <gpio.h>
//#include <Si4703.h>
#include "timer.h"


volatile uint8_t oldD;
uint32_t actFrequency; // Aktuální hodnota frekvence >>>>>>jaká velikost?<<<<<<<<<
uint8_t buttonPD2isPressed = 0; //Je zmáčknuté tlačítko na pinu PD2


int main(void)
{
    // --- Nastavení vstupů s pull-up ---
    gpio_mode_input_pullup(&DDRD, PD2);
    gpio_mode_input_pullup(&DDRD, PD3);
    gpio_mode_input_pullup(&DDRD, PD4);

    // --- Výstup PB0 (LED) ---
    gpio_mode_output(&DDRB, PB0);

    oldD = PIND;   // uložit počáteční stav portu D


    // --- Povolit PCINT2 skupinu ---
    PCICR |= (1 << PCIE2);





    // Povolit PCINT18, PCINT19, PCINT20 (PD2, PD3, PD4)
    PCMSK2  |= (1 << PCINT18)     // PD2
            | (1 << PCINT19)     // PD3
            | (1 << PCINT20);    // PD4

    sei(); // globální povolení přerušení

    while (1) {
        // hlavní smyčka prázdná
        if ((buttonPD2isPressed == 1) && ) {
          tim1_ovf_1sec();
          tim1_ovf_enable();
        } else {
          //tim1_ovf_disable();
          tim1_stop();
        }
    }
}

ISR(PCINT2_vect)
{
    uint8_t newD = PIND;   // čteme celý port D

    // PD2 (PCINT18)
    if ((newD & (1 << PD2)) != 0 && (oldD & (1 << PD2)) == 0) {

        gpio_write_high(&PORTB, PB0);
        /*
        actFrequency = SI4703_GetFreq();
        actFrequency = actFrequency + 100;
        SI4703_SetFreq(actFrequency);
        */
       buttonPD2isPressed = 1;
    }

    if ((newD & (1 << PD2)) == 0 && (oldD & (1 << PD2)) != 0) {

     buttonPD2isPressed = 0;

    }

    // PD3 (PCINT19)
    if ((newD & (1 << PD3)) == 0 && (oldD & (1 << PD3)) != 0) {
        // sem dej akci pro tlačítko PD3
        gpio_write_high(&PORTB, PB0);
    }

    // PD4 (PCINT20)
    if ((newD & (1 << PD4)) == 0 && (oldD & (1 << PD4)) != 0) {
        // sem dej akci pro tlačítko PD4
        gpio_write_low(&PORTB, PB0);
    }
  oldD = newD;
}

ISR(TIMER1_OVF_vect)
{
    gpio_toggle(&PORTB, PB0);
}
 