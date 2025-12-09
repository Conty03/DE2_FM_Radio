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

/* Display pinout */
#define PIN_SCK   PB5
#define PIN_MOSI  PB3
#define PIN_CS    PB2
#define PIN_RST   PB0

u8g2_t u8g2;

float actFreq;
uint8_t volume = 5;

/* 3 stages of changing frequency */
volatile uint8_t stepFreq = 1;
volatile uint8_t slowFreq = 0;
volatile uint8_t fastFreq = 0;
volatile uint8_t seekFreq = 1;

volatile uint8_t buttonPD2isPressed = 0; 
volatile uint8_t buttonPD3isPressed = 0;
volatile uint8_t buttonPD4isPressed = 0;
volatile uint8_t buttonPressedLong = 0;
volatile uint8_t buttonPressedLong2 = 0;
volatile uint8_t buttonReleased = 0;

volatile uint8_t initTime = 0;   // number of Timer1 overflows
volatile uint8_t fastTime = 0;   // number of Timer1 overflows within long press mode
volatile uint8_t oldD;

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

/* Update display */
void displayUpdateFreq(float freq)
{
  char buf[16];
  dtostrf(freq, 5, 1, buf);

  /* clear old number */
  u8g2_SetDrawColor(&u8g2, 0);  // set color: black 
  u8g2_DrawBox(&u8g2, 10, 28, 60, 16);
  u8g2_SetDrawColor(&u8g2, 1);  // set color: white

  /* draw new number */
  u8g2_SetFont(&u8g2, u8g2_font_courB12_tf);
  u8g2_DrawStr(&u8g2, 10, 40, buf);

  u8g2_SendBuffer(&u8g2);
}

int main(void)
{	
  gpio_mode_input_pullup(&DDRD, 2);  // Up
  gpio_mode_input_pullup(&DDRD, 3);  // Down
  gpio_mode_input_pullup(&DDRD, 4);  // Seek
  gpio_mode_output(&DDRD, PD6);      // LED
  oldD = PIND;    // save initial state of port D

  SI4703_Init();
  SI4703_SetVolume(volume);  
  SI4703_SeekUp();
  actFreq = SI4703_GetFreq();

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

  /* Display UI */
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetFont(&u8g2, u8g2_font_courB12_tf);
  u8g2_DrawStr(&u8g2, 10, 20, "Frequency:");
  u8g2_SendBuffer(&u8g2);
  displayUpdateFreq(actFreq);

  while (1) 
  {
    /* Up/Down buttons increase or decrease frequency by 100 kHz, if pressed for longer time frequency changes automatically */
    if ((buttonPD2isPressed == 1) || (buttonPD3isPressed == 1)) {

      if ((stepFreq == 1) || (slowFreq == 1) || (fastFreq == 1)) {
        gpio_toggle(&PORTD, PD6);

        if (buttonPD2isPressed == 1)
          actFreq += 0.1;
        else
          actFreq -= 0.1;

        if (actFreq > 108.0)
          actFreq = 87.5;
        else if (actFreq < 87.5)
          actFreq = 108.0;
        
        SI4703_SetFreq(actFreq);
        displayUpdateFreq(actFreq);

        stepFreq = 0;
        slowFreq = 0;
        fastFreq = 0;
      }

      tim1_ovf_33ms();      // set (start) Timer1 overflow interrupt
      tim1_ovf_enable();    // enable Timer1 overflow interrupt      

    } else if (buttonPD4isPressed == 1) {
      
      if (seekFreq == 1) {

        gpio_toggle(&PORTD, PD6);
        seekFreq = 0;
        SI4703_SeekUp();

        actFreq = SI4703_GetFreq();
        displayUpdateFreq(actFreq);
      }

    } else if (buttonReleased == 1) {
      buttonReleased = 0;
      tim1_ovf_disable();
      tim1_stop();
      TCNT1 = 0;    // reset timer NOT SURE

      initTime = 0;
      fastTime = 0;
    }
  }
}
 
/* Interrupt service routine PORTD */
ISR(PCINT2_vect)
{    
  uint8_t newD = PIND;   // update current state of port D

  // PD2 (PCINT18) pressed - increase frequency by 100 kHz (short and long press)
  if ((newD & (1 << PD2)) == 0 &&
      (oldD & (1 << PD2)) != 0) {   // falling edge detecction (pull-up)  1 \___ 0

    if (buttonPD3isPressed != 1) {    // prevent conflict when both buttons are pressed
    buttonPD2isPressed = 1;
    }
  }
 
  // PD3 (PCINT19) pressed - decrease frequency by 100 kHz (short and long press)
  if ((newD & (1 << PD3)) == 0 &&
      (oldD & (1 << PD3)) != 0) {

    if (buttonPD2isPressed != 1) {
      buttonPD3isPressed = 1;
    }
  }

  // PD4 (PCINT20) pressed - seek up function
  if ((newD & (1 << PD4)) == 0 &&
      (oldD & (1 << PD4)) != 0) {

    buttonPD4isPressed = 1;
  }

  // PDx release
  if (((newD & (1 << PD2)) != 0 && (oldD & (1 << PD2)) == 0) ||   // rising edge detection 0 ___/ 1
      ((newD & (1 << PD3)) != 0 && (oldD & (1 << PD3)) == 0) ||
      ((newD & (1 << PD4)) != 0 && (oldD & (1 << PD4)) == 0)) {

    buttonPD2isPressed = 0;
    buttonPD3isPressed = 0;
    buttonPD4isPressed = 0;
    buttonReleased = 1;

    buttonPressedLong = 0;
    buttonPressedLong2 = 0;

    initTime = 0;
    fastTime = 0;

    stepFreq = 1;
    slowFreq = 0;
    fastFreq = 0;
    seekFreq = 1;
  }

  oldD = newD;
}

/* Interrupt service routine TIMER1 overflow */
ISR(TIMER1_OVF_vect)
{
  if (buttonPressedLong == 0 && buttonPressedLong2 == 0) {    // button pressed shortly - no frequency change yet

    if (initTime > 20) {    // after 21 overflows (approx. 660 ms)
      buttonPressedLong = 1;
      initTime = 0;

    } else {
      initTime++;
    }
 
  } else if (buttonPressedLong == 1 && buttonPressedLong2 == 0){    // button still pressed - slow frequency change
    
    if (initTime > 6) {   // after every 7th overflow (approx. 200 ms)
      slowFreq = 1;
      initTime = 0;
 
      if (fastTime > 6) {   // after maxInitTime * maxFastTime overflows of timer1
        buttonPressedLong2 = 1;
        fastTime = 0;

      } else {
        fastTime++;
      }
 
    } else {
      initTime++;
    }
 
  } else if (buttonPressedLong == 1 && buttonPressedLong2 == 1) {   // long press mode - fast frequency change
     
    if (initTime > 2) {
      fastFreq = 1;
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