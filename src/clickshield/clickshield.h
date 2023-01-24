#ifndef CLICKSHIELD_H
#define CLICKSHIELD_H

#include <stdint.h>
#include <stdbool.h>

#define CS_INIT_BTN (0x01)
#define CS_INIT_RGB (0x02)
#define CS_INIT_POT (0x04)
#define CS_INIT_DIM (0x08)

#define CS_RGB_POS_RED (0x00)
#define CS_RGB_POS_GREEN (0x01)
#define CS_RGB_POS_BLUE (0x02)

typedef uint8_t CS_RGB_TypeDef[3];

void CS_Init(uint16_t);
void CS_BTN_SetCallback(void (*)(bool longpress));
void CS_RGB_SetDim(CS_RGB_TypeDef);
void CS_RGB_SetPins(volatile CS_RGB_TypeDef);
bool CS_BTN_ReadRaw();
bool CS_BTN_ReadDeb();

void CS_LoopHandler(uint32_t);

#endif
