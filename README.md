# FM Radio

## Our team:

Adam Keřka  
Martin Čontoš  
Karel Kubín 


## Introduction
This project focuses on the implementation of an FM receiver system using the RDA5807M FM module controlled by an ATmega328P microcontroller. A simple display is included to present basic information, and a low-voltage audio amplifier drives a small speaker, allowing us to both see and hear the signals received by the FM module. The goal of this project is to create a compact prototype radio system that can be further developed into even smaller designs in the future.


## Hardware Used

| Component              | Model            | Description |
|------------------------|------------------|-------------|
| FM Module              | RDA5807M         | Low-power FM radio receiver (50–115 MHz) |
| Display                | DEP128064C1-W    | 128×64 monochrome OLED display, SSD1306 driver. |
| Microcontroller        | ATmega328P       | 8-bit AVR microcontroller commonly used in Arduino Uno. |
| Audio Power Amplifier  | TPA741           | Low-voltage NF audio amplifier capable of driving small speakers. |
| Programmer             | CP2102           | Universal programmer with USB-UART converter | 
| 4pcs buttons           | ---              | --- | 
| 2pcs Leds              | ---              | --- |           


## Software 
Upon powering the device, the FM radio software initializes the FM module and immediately sets an initial frequency. This is achieved by calling the seek function, which scans the FM band and locks onto the strongest station available. The radio then begins to play without unnecessary delay.

After the start a first radio frequency being set, it is now waiting for an user input. Up/Down buttons that request frequency changes trigger the appropriate adjustment, the FM module is then updated accordingly and it results in tuning the desired radio station. 



## References

Library U8g2 for SSD1306 display driver by [OLIKRAUS] https://github.com/olikraus/u8g2/tree/master/csrc  
Used together with [gpio.c](https://raw.githubusercontent.com/tomas-fryza/avr-labs/master/library/gpio/gpio.c) and [gpio.h](https://raw.githubusercontent.com/tomas-fryza/avr-labs/master/library/gpio/gpio.h)

