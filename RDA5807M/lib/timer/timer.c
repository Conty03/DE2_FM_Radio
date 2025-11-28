#include "tim_device.h"
#include "stdio.h"

/* Clock frequency of timer F_CPU/PRESCALER = 16MHz/64
   in case of any change, recalculate this constant !!! */
#define BEEP_CLOCK          (uint32_t)(250000)

/* Incremented variable by TIMER0_COMPA_vect - system tick*/
static volatile uint32_t sys_tick = 0;

/*!
 *
 * @brief TIMER0_COMPA_vect
 *
 * TIM0 interrupt routine - increment the sys_tick
 *
 */
ISR(TIMER0_COMPA_vect) {
    sys_tick++;
}

/*!
 *
 * @brief Initialize 1 millisecond tick timer TIM0
 *
 * Function initializes AVR timer in CTC mode with
 * set parameters to invoke ISR each 1 ms.
 * ISR TIMER0_COMPA_vect contains incremented 32b
 * value, which represents tick.
 *
 * @sa tim_tick_get
 * @sa tim_block_ms
 * @sa tim_tick_initialize
 *
 */
void tim_tick_initialize(void) {

    /* TIM0 - ATmega328p 8-bit timer *************/

    /* WGM01: CTC mode - clear timer of compare match.
    This means that timer is incremented to value OCR1A,
    when the value is reached, interrupt is called and
    timer is cleared
    */
    TCCR0A |= (1 << WGM01);
    /* TCCR0B: CS01, CS00 sets 64x prescaler */
    TCCR0B |= (1 << CS01)|(1 << CS00);
    /* Enable timer interrupt */
    TIMSK0|= (1 << OCIE0A);
    /* Set the compare value based on calculation:
       freq = CLOCK/(PRESCALER * OCR1A)
       in case of 16MHz Clock and 64x prescaler
       the frequency of counting is 250kHZ what
       means that if timer counts from 0 to 249,
       value 249 is reached with frequency 1kHZ*/
    OCR0A = 249;
}
/*!
 *
 * @brief Un-Initialize 1 millisecond tick timer TIM0
 *
 * @sa tim_tick_initialize
 *
 */
void tim_tick_unitialize(void) {

    TCCR0A &=~(1 << WGM01);
    TIMSK0 &=~(1 << OCIE0A);

}
/*!
 *
 * @brief Get current MCU time in milliseconds
 *
 * Function returns value of incremented 32-bit
 * variable sys_tick (TIMER0_COMPA_vect) and
 * does it when timer interrupt is disabled.
 *
 * @return sys_tick - 1 millisecond 32-bit variable
 * @sa tim_block_ms
 *
 */
uint32_t tim_tick_get(void) {
    uint32_t tick = 0;
    /* Disable interrupt for correct
    reading of the sys_tick 32-bit value*/
    TIMSK0&=~(1 << OCIE0A);
    tick = sys_tick;
    /* Enable-back timer interrupt */
    TIMSK0|= (1 << OCIE0A);
    return tick;
}


/*!
 *
 * @brief Block (Delay) MCU for time in milliseconds
 *
 * @param [in] Delay - Delay in milliseconds
 *
 * MCU gets stuck here for passed time Delay in milliseconds.
 * For specified time function calls only tim_blocked_dothis.
 * This function shall be implemented by user/developer
 *
 *
 */
void tim_block_ms(uint32_t Delay) {

  uint32_t tickstart = 0;
  tickstart = tim_tick_get();

  while ((tickstart + Delay > tim_tick_get())) {
#if 0
        tim_blocked_dothis();
#endif
  }
}

/*!
 *
 * @brief Function called when tim_block_ms is in progress
 *
 * To be done
 *
 * @sa tim_block_ms
 *
 */
void tim_blocked_dothis(void) {

}
/*!
 *
 * @brief Initialize beep timer TIM1 f = CLOCK/64/OCR1A
 *
 * Function initialize timer for further generating
 * frequency of the highest frequency 250kHz. Also,
 * GPIO pins D9 (Buzzer anode controlled by timer)
 * and D7 (permanently grounded buzzer cathode) are
 * initialized. Timer works in the AVR CTC mode and
 * is configured as toggling PORTB.PIN1 when matches
 *
 * @sa tim_beep_D6D4_sound
 * @sa tim_beep_D6D4_freq
 *
 */
void tim_beep_D6D4_initialize(void) {

    /* TIM0 - ATmega328p 8-bit timer *************/

    /* Enable beep anode - timer controlled output 'D9'*/
    DDRB|= (1 << PIN1);
    /* Enable beep cathode - permanently grounded 'D7'*/
    DDRD|= (1 << PIN5);
    /* Set cathode as permanently grounded 'D7'*/
    PORTD|= (1 << PIN5);

    /* WGM12: CTC mode - clear timer of compare match.
    This means that timer is incremented to value OCR1A,
    when the value is reached, interrupt is called and
    timer is cleared
    CS11, CS10: Prescaler of the clock source 64x */
    TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);
    /* COM1A0: enable toggling PORTB.PIN1 when CTC
    reaches value OCR1A */
    TCCR1A = (1 << COM1A0);

}

/*!
 *
 * @brief Enable/Disable beeping timer TIM1
 *
 * @param [in] state - intended state
 *
 * Function disables/ enable timer and its feature
 * to toggle PORTB.PIN1
 *
 * @sa tim_blocked_dothis
 *
 */
extern void tim_beep_D6D4_sound(bool state) {
    if(state) {
        /* Enable toggling PORTB.PIN1 and
        enable timer device */
        TCCR1B |= (1 << WGM12);
        TCCR1A |= (1 << COM1A0);

    }
    else {
        /* Disable toggling PORTB.PIN1 and
        Disable timer device */
        TCCR1B &= ~(1 << WGM12);
        TCCR1A &= ~(1 << COM1A0);
    }
}

/*!
 *
 * @brief Set frequency of beep timer TIM1 in Hz
 *
 * @param [in] freq - Frequency in Hz
 *
 * Function calculates necessary OCR1A value related to
 * the clock frequency to adjust TIM1 CTC frequency (tone)
 * In case of any changes of the TIM1 prescaler, also do
 * correction of the BEEP_CLOCK constant !!!
 *
 */

extern void tim_beep_D6D4_freq(uint16_t freq) {

    uint16_t aux_OCR1A = 0u;
    /* Calculate OCR1A compare register value for
    TCNT1A counter (TCNT1A increments up to OCR1A) */
    aux_OCR1A = (uint16_t)((BEEP_CLOCK)/(uint32_t)freq);
    /* Reset counting register for fluent beep */
    TCNT1 = 0;
    /* Write register for frequency change */
    OCR1A = aux_OCR1A;

}