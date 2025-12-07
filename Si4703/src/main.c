/*
 * main.c
 *
 * Reference
 * GPIO library:
 * Timer library:
 * Si4703 library: https://github.com/eziya/AVR_SI4703/tree/master
 * 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>

#include "SI4703.h"
#include <gpio.h>
#include "timer.h"
#include <u8g2.h>

#define CHANNEL1	99.5  // Evropa2
#define CHANNEL2	105.5 // Evropa2 (Brno)

/*Buttons*/
#define DDR		DDRD
#define PORT	PORTD
#define PIN		PIND
#define D2		PD2		// Up
#define D3		PD3 	// Down
#define D4		PD4 	// Seek

/*Display*/
#define PIN_SCK   PB5
#define PIN_MOSI  PB3
#define PIN_CS    PB2
#define PIN_RST   PB0

u8g2_t u8g2;

float actFrequency;
uint8_t buttonPD2isPressed = 0; 
uint8_t buttonPD3isPressed = 0;
uint8_t buttonPD4isPressed = 0; 
uint8_t buttonPressedLong = 0;
uint8_t buttonPressedLong2 = 0;

volatile uint8_t initTime = 0;   // timer read counter for long press detection
volatile uint8_t fastTime = 0;   // timer read counter for fast frequency change
volatile uint8_t oldD;

uint8_t volume = 8;

/* GPIO + delay callback for u8g2 */
static uint8_t gpio_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  /* Suppress warnings about unused parameters */
  (void)u8x8;
  (void)arg_ptr;

  switch(msg)
  {
    /* Init all required pins */
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      gpio_mode_output(&DDRB, PIN_SCK);
      gpio_mode_output(&DDRB, PIN_MOSI);
      gpio_mode_output(&DDRB, PIN_CS);
      gpio_mode_output(&DDRB, PIN_RST);

      gpio_write_high(&PORTB, PIN_CS);   // CS high (inactive)
      gpio_write_high(&PORTB, PIN_RST);  // RST high
      return 1;

    /* Chip Select pin */
    case U8X8_MSG_GPIO_CS:
      if(arg_int)
        gpio_write_high(&PORTB, PIN_CS);   // CS high
      else
        gpio_write_low(&PORTB, PIN_CS);   // CS low
      return 1;

    /* Reset pin */
    case U8X8_MSG_GPIO_RESET:
      if(arg_int)
        gpio_write_high(&PORTB, PIN_RST);
      else
        gpio_write_low(&PORTB, PIN_RST);
      return 1;

    /* Clock pin */
    case U8X8_MSG_GPIO_SPI_CLOCK:
      if(arg_int)
        gpio_write_high(&PORTB, PIN_SCK);
      else
        gpio_write_low(&PORTB, PIN_SCK);
      return 1;

    /* MOSI pin */
    case U8X8_MSG_GPIO_SPI_DATA:
      if(arg_int)
        gpio_write_high(&PORTB, PIN_MOSI);
      else
        gpio_write_low(&PORTB, PIN_MOSI);
      return 1;

    /* Delay */
    case U8X8_MSG_DELAY_MILLI:
      while(arg_int--)
        _delay_ms(1);
      return 1;
  }

  return 0;
}

int main(void)
{	
  gpio_mode_input_pullup(&DDRD, 2);  // Up
  gpio_mode_input_pullup(&DDRD, 3);  // Down
  gpio_mode_input_pullup(&DDRD, 4);  // Seek
  gpio_mode_output(&DDRB, PB5);      // LED
  
  SI4703_Init();
  SI4703_SetFreq(CHANNEL1);
  SI4703_SetVolume(volume);
  //actFrequency = SI4703_GetFreq();
  actFrequency = CHANNEL1;

  oldD = PIND;    // save initial state of port D

  /* Enable PCINT2 */
  PCICR |= (1 << PCIE2);

  /* Enable interrupts on PD2, PD3, PD4 */
  PCMSK2  |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20);   

  /* Enable global interrupts */
  sei(); 

  /* 3-wire SPI constructor */
  u8g2_Setup_ssd1306_128x64_noname_f( &u8g2, U8G2_R0, u8x8_byte_3wire_sw_spi, gpio_cb);

  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);    // switch off power save mode
  u8g2_SetContrast(&u8g2, 150);   // <0; 255>

  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_courB12_tf);
  u8g2_DrawStr(&u8g2, 10, 10, "Frequency:");
  u8g2_DrawStr(&u8g2, 10, 40, "99,5");
  u8g2_SendBuffer(&u8g2);

  while (1) 
  {
    /* If Up or Down button is pressed, timer is started (including long press detection) */
    if (buttonPD2isPressed == 1 || buttonPD3isPressed == 1) { 
      tim1_ovf_33ms();
      tim1_ovf_enable();
    } else{
      tim1_ovf_disable();
      tim1_stop();
    }
  }
}
 
ISR(PCINT2_vect)
{    
  uint8_t newD = PIND;   // čteme celý port D

  // PD2 (PCINT18) stisknutí - zvýší frekvenci o 100 (krátký i dlouhý stisk)
  if ((newD & (1 << PD2)) == 0 && (oldD & (1 << PD2)) != 0) {

    gpio_toggle(&PORTB, PB5);

    actFrequency += 0.1;
    SI4703_SetFreq(actFrequency);

    if (buttonPD3isPressed != 1) {
    buttonPD2isPressed = 1;
    }
   }
 
   // PD3 (PCINT19) stisknutí - sníží frekvenci o 100 (krátký i dlouhý stisk)
   if ((newD & (1 << PD3)) == 0 && (oldD & (1 << PD3)) != 0) {
    
    gpio_toggle(&PORTB, PB5);

    actFrequency += 0.1;
    SI4703_SetFreq(actFrequency);

    if (buttonPD2isPressed != 1) {
      buttonPD3isPressed = 1;
    }
  }

  // PD2 a PD3 (PCINT18) uvolnění
   if (((newD & (1 << PD3)) != 0 && (oldD & (1 << PD3)) == 0) || ((newD & (1 << PD2)) != 0 && (oldD & (1 << PD2)) == 0)) {
       
       buttonPD2isPressed = 0;
       buttonPD3isPressed = 0;
       buttonPressedLong = 0;
       buttonPressedLong2 = 0;
       initTime = 0;
       fastTime = 0;

   }


  // PD4 (PCINT20) - funkce seek - najde nejbližší stanici na vyšší frekvenci
  if ((newD & (1 << PD4)) == 0 && (oldD & (1 << PD4)) != 0) {
      
    gpio_toggle(&PORTB, PB5);

    SI4703_SeekUp();
    actFrequency = SI4703_GetFreq();

    char buf[16];
    dtostrf(actFrequency, 5, 1, buf);

    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_courB12_tf);
    u8g2_DrawStr(&u8g2, 10, 10, "Frequency:");
    u8g2_DrawStr(&u8g2, 10, 40, buf);
    u8g2_SendBuffer(&u8g2);

    /*Other option:*/
    /*
    u8g2_SetDrawColor(&u8g2, 0);           // black color
    u8g2_DrawBox(&u8g2, 10, 28, 60, 16);   // clear area
    u8g2_SetDrawColor(&u8g2, 1);           // white color

    u8g2_DrawStr(&u8g2, 10, 40, buf);
    u8g2_SendBuffer(&u8g2); 
    */    
  }
  oldD = newD;
}

ISR(TIMER1_OVF_vect)
{

  if (buttonPressedLong == 0 && buttonPressedLong2 == 0) { // krátký stisk - jdenorázová změna frekvence o 100

     /*
      if (zkouska12 == 1) { // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx vvvvvvvvvvvv
  
        gpio_toggle(&PORTB, PB5);
      
      } else {

      if (buttonPD2isPressed == 1) {
        actFrequency += 0.1;
      } else if (buttonPD3isPressed == 1) {
        actFrequency -= 0.1;
      } 

        SI4703_SetFreq(actFrequency);
      } // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ^^^^^^^^^^
     */

    if (initTime > 20) {
       buttonPressedLong = 1;
       initTime = 0;
     } else {
         initTime++;
     }
 
  } else if (buttonPressedLong == 1 && buttonPressedLong2 == 0){ // dlouhý stisk - frekvence skáče v pravidelných intervalech nahoru/dolů
     
    if (initTime > 6) {

      gpio_toggle(&PORTB, PB5);

      if (buttonPD2isPressed == 1) {
        actFrequency += 0.1;
      } else if (buttonPD3isPressed == 1) {
        actFrequency -= 0.1;
      } 

      SI4703_SetFreq(actFrequency);

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
 
      gpio_toggle(&PORTB, PB5);
 
      if (buttonPD2isPressed == 1) {
          actFrequency += 0.1;
        } else if (buttonPD3isPressed == 1) {
          actFrequency -= 0.1;
        } 
 
      SI4703_SetFreq(actFrequency);
 
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