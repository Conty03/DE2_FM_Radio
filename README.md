# FM Radio

## Our team:

Adam Keřka  
Martin Čontoš  
Karel Kubín 


## Introduction
This project focuses on the implementation of an FM receiver system using the Si4703 FM module controlled by an ATmega328P microcontroller. A simple display is included to present basic information, and a low-voltage audio amplifier drives a small speaker, allowing us to both see and hear the signals received by the FM module. The goal of this project is to create a compact prototype radio system that can be further developed into even more sophisticated designs in the future.

<br>

<p align="center">
  <img src="images/DE2_FMradio_diagram.png" alt="Circuit diagram" style= "width:80%; margin-top:20px;">
</p>

## Hardware

| Component              | Model            | Description |
|------------------------|------------------|-------------|
| FM Module              | Si4703           | Low-power FM radio receiver (50–115 MHz) |
| Display                | DEP128064C1-W    | 128×64 monochrome OLED display, SSD1306 driver. |
| Microcontroller        | ATmega328P       | 8-bit AVR microcontroller commonly used in Arduino UNO but 8 MHz. |
| Audio Power Amplifier  | TPA741           | Low-voltage LF audio amplifier capable of driving small speakers. |
| Speaker                | CDS-25148-L100   | power = 1.5 W, resistance = 8 omhs, intensity = 94 dBA |
| Programmer             | CP2102           | Universal programmer with USB-UART converter | 
| 4pcs buttons           | ---              | Serves for controlling frequency and seek-up function  | 
| Led                    | ---              | Indicates power state ON/OFF  |           


<p align="center">
<img width="3174" height="2191" alt="_DSC4238_crop" src="https://github.com/user-attachments/assets/8f1b53e2-3849-4d26-9c42-edf5b539e512" />

## Software 

Upon powering the device, the ATmega328P initializes the FM module and display and immediately sets an initial frequency. This is achieved by calling the seek function, which scans the FM band and locks onto the strongest station available. Simultaneously initial frequency is displayed on the OLED. The radio then begins to play without unnecessary delay. 

After the first radio frequency being set, radio is waiting for an user input. Up/Down buttons request frequency changes by switching voltage on determined ATmega328P inputs. FM module is then updated accordingly and it results in tuning the desired radio station. Tuning radio is also possible by pressing the seek button triggering the seek-up function.

Volume is controlled by potentiometer connected in the feedback loop of TPA741 amplifier. In future we would like to utilize the internal DACs of Si4703 for adjusting the volume. Si4703 enables digital adjustment on 15 levels involving mute.

<br>

<p align="center">
  <img src="images/DE2_FMradio_flowchart_v2.drawio.png" alt="Software flowchart" style= "margin-top:40px;">
</p>

### Init 
Init function initializes the Si4703. Main goal of initialization is to establish TWI communication between the FM module and microcontroller and to set the module to standby mode (Power-up sequence). This function also sets default settings which are not intended to change during the program run: Mute Disable: ON, Mono: ON, RDS Enable: ON, De-emphasis: ON (50us), Band Select: 00 (87,5 - 105 MHz), Channel Spacing: 01 (100 kHz), Volume: 1111 (max), RSSI Seek Treshold: 0x19 (25 from <0;127>), Seek SNR Threshold: 0x04 (4 from <0;7>), Seek FM Impulse Detection Threshold: 0x08 (8 from <0;15>).

### Mono
Si4703 has 2 pins dedicated for an audio output (L + R) too enable both Mono and Stereo audio modes. Since stereo mode requires system of 2 speakers we force Si4703 to output only mono (both pins then output the same signal).

### Seek
Searches (seeks) radio station according to the strength of RSSI (Received Signal Strenght Indicator). In other words if Seek function finds a station which fulfils a lower limit of the RSSI, the station is tuned. Seek function is devided into SI4703_SeekUp() and SI4703_SeekDown() function. We can adjust seeking by setting appropriate register.

Bits 8 to 10 (SEEK, SEEKUP, SKMODE) are reserved for Seek function in Power configuration register. SEEK enables seeking by setting this bit to 1. SEEKUP determies direction of seeking within band 87.5 to 108 MHz. If we set SEEKUP bit to 1, it starts to "seek up" from actual frequency. Otherwise it starts to seek down. If SKMODE is set to 1, seek function stops seeking at the upper or lower band limit. In this project we prefer SKMODE to be set to 0, so the function continues seeking even after reaching the band limit.

In SEEKTH[7:0] you can set RSSI seek treshold.

### Volume
Despite using potentiometer as part of UI (User Interface) for controlling volume, it is neccessary to set an initial volume of Si4703.

Volume can be controlled by VOLUME[3:0] bits in System configuration 2 register. Bit combination of "0000" stands for mute, which is also the default value, and combination of "1111" for maximum volume.  Volume scale is logarithmic.


VOLEXT is 8th bit of the System congiguration 3 register and attenuates the output by 30dB if set to 1. Default value is 0, so in this program.

## Documentation

### gpio_cb
Callback function used by U8g2 for software SPI communication. It handles pin toggling (CS, RESET, SCK, MOSI) and delays required by the software SPI protocol.  
**Input:** `msg` (U8g2 request type), `arg_int` (pin state or delay), `arg_ptr` (unused)  
**Output:** Returns `1` on success, otherwise `0`.

### Debounce
Implements a software debouncing algorithm to filter out mechanical noise (signal "bouncing") from button presses using Timer 0.
**Input:** `pressedButton` (The current raw state of the button: `1` if pressed, `0` if released).  
**Output:** Returns the stabilized button state (`1` = valid press, `0` = noise/invalid).

### u8g2_Setup_ssd1306_128x64_noname_f
Initializes the U8g2 structure for the SSD1306 128×64 OLED in **full buffer mode**. This requires 1024 bytes of RAM but allows faster execution for complex graphics.  
**Input:** pointer to `u8g2`, rotation mode, byte-transfer function, GPIO callback  
**Output:** none.

### u8x8_byte_3wire_sw_spi
Software implementation of 3-wire SPI used by U8g2 to send data bit-by-bit (Bit-banging) over GPIO pins.  
**Input:** pointer to `u8x8`, message type, 8-bit argument, data pointer  
**Output:** returns `1` on successful data transfer.

### u8g2_InitDisplay
Initializes the OLED controller hardware and sends startup configuration commands.  
**Input:** pointer to `u8g2`  
**Output:** none.

### u8g2_SetPowerSave
Enables or disables OLED power-save (sleep) mode. Used to turn the display on after initialization.  
**Input:** pointer to `u8g2`, value `0` (active/on) or `1` (power-save/sleep)  
**Output:** none.

### u8g2_SetContrast
Sets the display contrast level (brightness).  
**Input:** pointer to `u8g2`, contrast value (0–255)  
**Output:** none.

### u8g2_ClearBuffer
Clears all pixels in the internal frame buffer (RAM). This is typically called at the start of the display loop in full buffer mode to remove previous content.  
**Input:** pointer to `u8g2`  
**Output:** none.

### u8g2_SendBuffer
Transfers the entire content of the internal frame buffer (RAM) to the OLED display controller. In full buffer mode, this is called once after all drawing commands are finished.  
**Input:** pointer to `u8g2`  
**Output:** none.

### u8g2_SetFont
Selects the font used for subsequent text drawing operations.  
**Input:** pointer to `u8g2`, pointer to font data (e.g., `u8g2_font_courB12_tf`)  
**Output:** none.

### u8g2_DrawStr
Draws a text string at the specified X, Y coordinates within the current page.  
**Input:** pointer to `u8g2`, x coordinate, y coordinate, C-string  
**Output:** returns width of the string.

### gpio_mode_output
Configures a specific GPIO pin as an output (modifies DDR register).  
**Input:** pointer to DDR register, bit index  
**Output:** none.

### gpio_write_high
Sets the selected output pin to logic HIGH (modifies PORT register).  
**Input:** pointer to PORT register, bit index  
**Output:** none.

### gpio_write_low
Sets the selected output pin to logic LOW (modifies PORT register).  
**Input:** pointer to PORT register, bit index  
**Output:** none.

### gpio_toggle
Inverts the state of the specified GPIO pin (toggles between HIGH and LOW).  
**Input:** pointer to PORT register, bit index  
**Output:** none.

### SI4703_Init
Initializes the Si4703 FM tuner module via I2C.  
**Input:** none  
**Output:** none.

### SI4703_SetVolume
Sets the volume level of the FM tuner.  
**Input:** volume level (0-15)  
**Output:** none.

### SI4703_SetFreq
Tunes the FM receiver to a specific frequency.  
**Input:** frequency value (float, e.g., 101.5)  
**Output:** none.

### SI4703_SeekUp
Starts an automatic seek operation to find the next available station with good signal quality.  
**Input:** none  
**Output:** none.

### SI4703_GetFreq
Reads the currently tuned frequency from the Si4703 registers.  
**Input:** none  
**Output:** returns frequency as a float.


### Functions from library U8g2
Functions from library U8g2 for SSD1306 display driver by [OLIKRAUS](https://github.com/olikraus/u8g2/tree/master/csrc):

u8g2_InitDisplay() - initializes the display

u8g2_SetPowerSave() - turns on/off power save mode (1 = on, 0 = off)

u8g2_SetContrast() - sets the level of contrast (from 0 to 255)

u8g2_ClearBuffer() - clears the screen

u8g2_SetFont() - 

u8g2_DrawStr()

u8g2_SendBuffer()
 


## References

Library U8g2 for SSD1306 display driver by [OLIKRAUS](https://github.com/olikraus/u8g2/tree/master/csrc)
Used together with [gpio.c](https://raw.githubusercontent.com/tomas-fryza/avr-labs/master/library/gpio/gpio.c) and [gpio.h](https://raw.githubusercontent.com/tomas-fryza/avr-labs/master/library/gpio/gpio.h) by [tomas-fryza](https://github.com/tomas-fryza)

Library [timer.h](https://raw.githubusercontent.com/tomas-fryza/avr-labs/refs/heads/master/library/timer/timer.h) by [tomas-fryza](https://github.com/tomas-fryza)

Libraries 128A_TWI.c, 128A_TWI.h, 128A_USART.c,128A_USART.h by kiki 

Library AVR_Si4703 for Si4703 FM module by [eziya](https://github.com/eziya/AVR_SI4703)

Datasheet Si4703 [SKYWORKS](https://www.skyworksinc.com/-/media/SkyWorks/SL/documents/public/data-sheets/Si4702-03-C19.pdf)

## Demonstration Video 
https://www.youtube.com/watch?v=jUPHBMsGwlg

## Poster
<p align = "center">
<img width="1414" height="2000" alt="FM Radio - project" src="https://github.com/user-attachments/assets/5f055c53-da6d-425d-a690-75665cc21f5a" />


