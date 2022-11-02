# ClickShield Peripherals
## RGB Led
Active Low, pinout:
- Red: PA6
- Green: PA8
- Blue: PA4

## Button
External Pullup, connected on Pin PA3

## Potentiometer
Connected on Pin PA7, which has AF for ADC\_IN7

# How To
## Makefile
The makefile contains the following commands:
- build -> creates the .elf and .bin file from source
- clean -> clears all temporary files
- flash -> flashes the bin file via st-util
