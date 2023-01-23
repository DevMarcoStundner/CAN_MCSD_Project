#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  CS_STEP_FULL,
  CS_STEP_HALF,
  CS_STEP_QUARTER,
  CS_STEP_EIGHTH
} cs_step_mode_t;

void cs_step_init();
void cs_step_setmode(cs_step_mode_t mode);
int cs_step_move(int32_t pos);

#endif
