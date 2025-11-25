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



## Software 

The oled display is controlled by 3 wire SPI, using SCK, MOSI and CS + RS. Display then should provide a basic information about radio frequency.

#define PIN_SCK   PB5
#define PIN_MOSI  PB3
#define PIN_CS    PB2
#define PIN_RST   PB0


