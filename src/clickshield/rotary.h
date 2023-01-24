#ifndef ROTARY_H
#define ROTARY_H

#include <stdint.h>
#include <stdbool.h>

void cs_rot_init();
void cs_rot_handle(uint32_t time);
float cs_rot_getPos();
void cs_rot_setIndicator(float pos);
void cs_rot_setBlink(bool blink);
void cs_rot_setBtnCallback(void (*callback)(bool longpress));

#endif
