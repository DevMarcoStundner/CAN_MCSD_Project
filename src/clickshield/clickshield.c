#include "clickshield.h"
#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"

void CS_LoopHandler(uint32_t current_time) {
}

void CS_Init(uint16_t initDevices) {
  if (initDevices & CS_INIT_RGB) {
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_InitTypeDef s_gpioinit;
    LL_GPIO_StructInit(&s_gpioinit);
    s_gpioinit.Pin = LL_GPIO_PIN_4 | LL_GPIO_PIN_6 | LL_GPIO_PIN_8;
    s_gpioinit.Mode = LL_GPIO_MODE_OUTPUT;
    s_gpioinit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    s_gpioinit.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(GPIOA, &s_gpioinit);
  }
  if (initDevices & CS_INIT_BUTTON) {
  }
  if (initDevices & CS_INIT_POTI) {
  }
}

void CS_RGB_Set(CS_RGB_TypeDef colour) {
  uint32_t pinstates = 0;
  if (colour.red > 0) {
    pinstates |= 
}
