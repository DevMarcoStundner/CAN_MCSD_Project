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

# Clock config
## Bus clocks
- All buses have a fmax=80MHz.
- All AHB/APB prescalers divide by 1.

## PLL
- fin is 4Mhz to 16Mhz
- fvco is 64MHz to 344MHz
- M is 1 to 8
- R is 2,4,6,8
- N is 8 to 86
- SYSCLK source is PLLCLK

For SYSCLK=80MHz:
- M=4
- N=80
- R=4
- CLKSRC=HSI

# How To
## Makefile
The makefile contains the following commands:
- build -> creates the .elf and .bin file from source
- clean -> clears all temporary files
- flash -> flashes the bin file via st-util
