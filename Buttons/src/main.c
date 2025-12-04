#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <gpio.h>
//#include <Si4703.h>
#include "timer.h"

// =============================================================================== SEKCE TLACITKA
volatile uint8_t oldD;
uint32_t actFrequency; // Aktuální hodnota frekvence >>>>>>jaká velikost?<<<<<<<<<
uint8_t buttonPD2isPressed = 0; //Je zmáčknuté tlačítko na pinu PD2
uint8_t buttonPD3isPressed = 0; //Je zmáčknuté tlačítko na pinu PD3
uint8_t buttonPD4isPressed = 0; //Je zmáčknuté tlačítko na pinu PD4
uint8_t buttonPressedLong = 0; // tlačítko už je zmáčknuté určitou dobu >> 1
uint8_t buttonPressedLong2 = 0; // tlačítko už je zmáčknuté určitou delší dobu >> 1



int main(void)
{
    // --- Nastavení vstupů s pull-up ---
    gpio_mode_input_pullup(&DDRD, PD2);
    gpio_mode_input_pullup(&DDRD, PD3);
    gpio_mode_input_pullup(&DDRD, PD4);

    // --- Výstup PB0 (LED) ---
    gpio_mode_output(&DDRB, PB5);

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
        if (buttonPD2isPressed == 1 || buttonPD3isPressed == 1) {
          tim1_ovf_33ms();
          tim1_ovf_enable();
        } else{
          tim1_ovf_disable();
          tim1_stop();
        };
          
    }
}

ISR(PCINT2_vect)
{
    
    uint8_t newD = PIND;   // čteme celý port D

    // PD2 (PCINT18)
    if ((newD & (1 << PD2)) == 0 && (oldD & (1 << PD2)) != 0) {
        
        gpio_toggle(&PORTB, PB5);
        /*
        actFrequency = SI4703_GetFreq();
        actFrequency = actFrequency + 100;
        SI4703_SetFreq(actFrequency);
        */
       if (buttonPD3isPressed != 1) {
       buttonPD2isPressed = 1;
       }
    }

    if ((newD & (1 << PD2)) != 0 && (oldD & (1 << PD2)) == 0) {
  
     buttonPD2isPressed = 0;
     buttonPressedLong = 0;
     buttonPressedLong2 = 0;

    }

    // PD3 (PCINT19)
    if ((newD & (1 << PD3)) == 0 && (oldD & (1 << PD3)) != 0) {
        // sem dej akci pro tlačítko PD3
        gpio_write_high(&PORTB, PB5);
        if (buttonPD2isPressed != 1) {
            buttonPD3isPressed = 1;
        }
    }

    if ((newD & (1 << PD3)) != 0 && (oldD & (1 << PD3)) == 0) {
        // sem dej akci pro tlačítko PD3
        buttonPD3isPressed = 0;
        buttonPressedLong = 0;
        buttonPressedLong2 = 0;
    }


    // PD4 (PCINT20)
    if ((newD & (1 << PD4)) == 0 && (oldD & (1 << PD4)) != 0) {
        // sem dej akci pro tlačítko PD4
        gpio_write_low(&PORTB, PB5); // ===================================== AKCE PRO PD4 TLACITKO (SEEK)
        // SI4703_SeekUp();   
    }
  oldD = newD;
}

volatile uint8_t initTime = 0; // pocitadlo nasobku doby preteceni timeru
volatile uint8_t fastTime = 0; // pocitadlo pro zvyseni prodlevy pred zrychlenim


ISR(TIMER1_OVF_vect)
{
    
    actFrequency = SI4703_GetFreq();


    if (buttonPressedLong == 0 && buttonPressedLong2 == 0) {
        if (initTime > 20) {
          buttonPressedLong = 1;
          gpio_toggle(&PORTB, PB5); // ===================================== AKCE 1 PRO PD2 A PD3 TLACITKA SPOLU (ZATIM - UP, DOWN)
        
           /*
             if (buttonPD2isPressed == 1) {
                actFrequency += 100;
                SI4703_SetFreq(actFrequency);
             } else {
																actFrequency -= 100;
               SI4703_SetFreq(actFrequency);
             } 
           */
          
          initTime = 0;
        } else {
            initTime++;
        }
    } else if (buttonPressedLong == 1 && buttonPressedLong2 == 0){
      
        if (initTime > 6) {
          gpio_toggle(&PORTB, PB5); // ===================================== AKCE 2 PRO PD2 A PD3 TLACITKA SPOLU (ZATIM - UP, DOWN)
          initTime = 0;
          if (fastTime > 6) {
              buttonPressedLong2 = 1;
              fastTime = 0;
          } else {
              fastTime++;
          }
        } else {
            initTime++;
        }
    } else if (buttonPressedLong == 1 && buttonPressedLong2 == 1){
      
        if (initTime > 2) {
          gpio_toggle(&PORTB, PB5); // ===================================== AKCE 3 PRO PD2 A PD3 TLACITKA SPOLU (ZATIM - UP, DOWN)
          initTime = 0;
        } else {
            initTime++;
        }
    } else {
        buttonPressedLong = 0;
        buttonPressedLong2 = 0;
        initTime = 0;
        fastTime = 0;
    }
}

// =============================================================================== KONEC SEKCE TLACITKA










