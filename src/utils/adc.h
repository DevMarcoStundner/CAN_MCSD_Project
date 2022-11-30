#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define MYADC_MAX_PINS (11)

typedef enum {
	MYADC_PIN_A0=0,
	MYADC_PIN_A1=1,
	MYADC_PIN_A2=2,
	MYADC_PIN_A3=3,
	MYADC_PIN_A4=4,
	MYADC_PIN_A5=5,
	MYADC_PIN_A6=6,
	MYADC_PIN_A7=7,
	MYADC_PIN_DAC1=8,
	MYADC_PIN_DAC2=9,
	MYADC_PIN_VREF=10,
} myadc_sources_EnumDef;

uint8_t myadc_configure(myadc_sources_EnumDef srcpin);
uint8_t myadc_getval(uint16_t * pvalue);
uint8_t mydac_setval(uint16_t value, myadc_sources_EnumDef pin);

#endif
