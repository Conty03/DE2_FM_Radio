#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <gpio.h>
#include <Si4703.h>
#include "timer.h"

// =============================================================================== SEKCE TLACITKA
volatile uint8_t oldD;
uint32_t actFrequency; // Aktuální hodnota frekvence >>>>>>jaká velikost?<<<<<<<<<
uint8_t buttonPD2isPressed = 0; //Je zmáčknuté tlačítko na pinu PD2
uint8_t buttonPD3isPressed = 0; //Je zmáčknuté tlačítko na pinu PD3
uint8_t buttonPD4isPressed = 0; //Je zmáčknuté tlačítko na pinu PD4
uint8_t buttonPressedLong = 0; // tlačítko už je zmáčknuté určitou dobu >> 1
uint8_t buttonPressedLong2 = 0; // tlačítko už je zmáčknuté určitou delší dobu >> 1
uint8_t zkouska12 = 1; // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx potom smazat



int main(void)
{
    // --- Nastavení vstupů s pull-up ---
    gpio_mode_input_pullup(&DDRD, PD2);
    gpio_mode_input_pullup(&DDRD, PD3);
    gpio_mode_input_pullup(&DDRD, PD4);

    if (zkouska12 == 1){ // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx vvvvvvv
    // --- Výstup PB0 (LED) ---
    gpio_mode_output(&DDRB, PB5);
    } // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ^^^^^^^

    oldD = PIND;   // uložit počáteční stav portu D

    // --- Povolit PCINT2 skupinu ---
    PCICR |= (1 << PCIE2);

    // Povolit PCINT18, PCINT19, PCINT20 (PD2, PD3, PD4)
    PCMSK2  |= (1 << PCINT18)     // PD2
            | (1 << PCINT19)     // PD3
            | (1 << PCINT20);    // PD4

    sei(); // globální povolení přerušení

    while (1) {
        
        if (buttonPD2isPressed == 1 || buttonPD3isPressed == 1) { 
          tim1_ovf_33ms();
          tim1_ovf_enable();
        } else{
          tim1_ovf_disable();
          tim1_stop();
        }; // když je zmáčknuté tlačítko 1 nebo 2, tak se spouští timer (rozpoznání krátkého/dlouhého stisku)
          
    }
}

ISR(PCINT2_vect)
{
    
    uint8_t newD = PIND;   // čteme celý port D

    // PD2 (PCINT18) stisknutí - zvýší frekvenci o 100 (krátký i dlouhý stisk)
    if ((newD & (1 << PD2)) == 0 && (oldD & (1 << PD2)) != 0) {
        
      /*
      if (zkouska12 = 1) { // xxxxxxxxxxxxxxxxxxxxxx vvvvvvvvvvvvv
        gpio_toggle(&PORTB, PB5);
      } else { // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ^^^^^^^^^^^^^
        actFrequency = SI4703_GetFreq();
        actFrequency = actFrequency + 100;
        SI4703_SetFreq(actFrequency);
      } // xxxxxxxxxxxxxxxxxxxxxxxxx
      */

      if (buttonPD3isPressed != 1) {
      buttonPD2isPressed = 1;
      }
    }

    // PD2 (PCINT18) uvolnění
    if ((newD & (1 << PD2)) != 0 && (oldD & (1 << PD2)) == 0) {
  
     buttonPD2isPressed = 0;
     buttonPressedLong = 0;
     buttonPressedLong2 = 0;

    }

    // PD3 (PCINT19) stisknutí - sníží frekvenci o 100 (krátký i dlouhý stisk)
    if ((newD & (1 << PD3)) == 0 && (oldD & (1 << PD3)) != 0) {
        
      /*
        if (zkouska12 = 1) { // xxxxxxxxxxxxxxxxxxxxxx vvvvvvvvvvvvv
          gpio_toggle(&PORTB, PB5);
        } else { // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ^^^^^^^^^^^^^
          actFrequency = SI4703_GetFreq();
          actFrequency = actFrequency - 100;
          SI4703_SetFreq(actFrequency);
        } // xxxxxxxxxxxxxxxxxxxxxxxxx
      */

      if (buttonPD2isPressed != 1) {
        buttonPD3isPressed = 1;
        }
    }

    // PD3 (PCINT18) uvolnění
    if ((newD & (1 << PD3)) != 0 && (oldD & (1 << PD3)) == 0) {
        
        buttonPD3isPressed = 0;
        buttonPressedLong = 0;
        buttonPressedLong2 = 0;
    }


    // PD4 (PCINT20) - funkce seek - najde nejbližší stanici na vyšší frekvenci
    if ((newD & (1 << PD4)) == 0 && (oldD & (1 << PD4)) != 0) {
        
        if (zkouska12 = 1) { // xxxxxxxxxxxxxxxxxxxxxx vvvvvvvvvvvvv
          gpio_toggle(&PORTB, PB5);
        } else { // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ^^^^^^^^^^^^^
          SI4703_SeekUp();
        } // xxxxxxxxxxxxxxxxxxxxxxxxx
           
    }
  oldD = newD;
}

volatile uint8_t initTime = 0; // pocitadlo nasobku doby preteceni timeru
volatile uint8_t fastTime = 0; // pocitadlo pro zvyseni prodlevy pred zrychlenim


ISR(TIMER1_OVF_vect)
{
    if (zkouska12 != 1) { // xxxxxxxxxxxxxxxxxxxx
      actFrequency = SI4703_GetFreq();
    } // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

    if (buttonPressedLong == 0 && buttonPressedLong2 == 0) { // krátký stisk - jdenorázová změna frekvence o 100
        if (initTime > 20) {
          buttonPressedLong = 1;

          if (zkouska12 == 1) { // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx vvvvvvvvvvvv

            gpio_toggle(&PORTB, PB5); // ===================================== AKCE 1 PRO PD2 A PD3 TLACITKA SPOLU (ZATIM - UP, DOWN)
          
          } else {

            if (buttonPD2isPressed == 1) {
              actFrequency += 100;
            } else if (buttonPD3isPressed == 1) {
              actFrequency -= 100;
            } 

            SI4703_SetFreq(actFrequency);
          } // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ^^^^^^^^^^
          
          initTime = 0;
        } else {
            initTime++;
        }

    } else if (buttonPressedLong == 1 && buttonPressedLong2 == 0){ // dlouhý stisk - frekvence skáče v pravidelných intervalech nahoru/dolů
      
        if (initTime > 6) {
          
          if (zkouska12 == 1) { // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx vvvvvvvvvvvv

            gpio_toggle(&PORTB, PB5);
          
          } else {

            if (buttonPD2isPressed == 1) {
              actFrequency += 100;
            } else if (buttonPD3isPressed == 1) {
              actFrequency -= 100;
            } 

            SI4703_SetFreq(actFrequency);
          } // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ^^^^^^^^^^

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

    } else if (buttonPressedLong == 1 && buttonPressedLong2 == 1){ // nejdelší stisk - změna frekvence zrychlí
      
        if (initTime > 2) {

          if (zkouska12 == 1) { // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx vvvvvvvvvvvv

            gpio_toggle(&PORTB, PB5);
          
          } else {

            if (buttonPD2isPressed == 1) {
              actFrequency += 100;
            } else if (buttonPD3isPressed == 1) {
              actFrequency -= 100;
            } 

            SI4703_SetFreq(actFrequency);
          } // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ^^^^^^^^^^

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
