#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

void UT_byteset(uint8_t *dest, uint8_t ch, uint32_t count);
bool ut_fequal(float a, float b);

#endif
