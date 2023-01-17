#ifndef ROTARY_H
#define ROTARY_H

#include <stdint.h>
#include <stdbool.h>

void cs_rot_init();
void cs_rot_reset();
int32_t cs_rot_getPos();
int32_t cs_rot_checkOverflow();
int32_t cs_rot_getDiff();
void cs_rot_setIndicator(uint16_t leds);

#endif
