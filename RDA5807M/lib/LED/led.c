#include "led.h"

void gpio_initialize(void){

    DDRB|=(1 << ONBOARD_LED);
    PORTB&=~(1 << ONBOARD_LED);
}

void gpio_LED_toggle(void){

    if(PORTB & (1 << ONBOARD_LED)){
        PORTB&=~(1 << ONBOARD_LED);
    }
    else {
        PORTB|= (1 << ONBOARD_LED);
    }
}

void gpio_LED_set(bool state){

    if(state == true){
        PORTB|= (1 << PIN5);
    }
    if(state == false) {
        PORTB&=~(1 << PIN5);
    }
}