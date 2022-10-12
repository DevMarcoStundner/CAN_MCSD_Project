#ifndef CLICKSHIELD_H
#define CLICKSHIELD_H

#include <stdint.h>

#define CS_INIT_BUTTON (0x01)
#define CS_INIT_RGB (0x02)
#define CS_INIT_POTI (0x04)

typedef struct {
  uint16_t red;
  uint16_t green;
  uint16_t blue;
} CS_RGB_TypeDef;

void CS_Init(uint16_t);
void CS_RGB_Set(CS_RGB_TypeDef);

void CS_LoopHandler();

#endif
