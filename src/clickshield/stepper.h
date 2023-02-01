#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  CS_STEP_FULL=1,
  CS_STEP_HALF=2,
  CS_STEP_QUARTER=4,
  CS_STEP_EIGHTH=8
} cs_step_mode_t;

void cs_step_init();
void cs_step_setmode(cs_step_mode_t mode);
void cs_step_handler(uint32_t time);
void cs_step_setPosition(float pos);
float cs_step_getPosition();
bool cs_step_getRunning();

#endif
