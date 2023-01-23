#ifndef ROTARY_H
#define ROTARY_H

#include <stdint.h>
#include <stdbool.h>

void cs_rot_init();
void cs_rot_reset();
void cs_rot_handle();
int32_t cs_rot_getPos();
int32_t cs_rot_checkOverflow();
int32_t cs_rot_getDiff();
uint16_t cs_rot_calcIndicator(int32_t position, uint32_t fullscale);
void cs_rot_setIndicator(uint16_t leds);

#endif
