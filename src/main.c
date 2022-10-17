#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_utils.h"

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"

#include "os/os.h"
#include "clickshield/clickshield.h"

volatile static CS_RGB_TypeDef rgbcolor = {0,0,128};

void controlloop(uint32_t looptime) {
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
  CS_LoopHandler(looptime);
  LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
}

void btnhandler(CS_BTN_Action_TypeDef value) {
  switch(value) {
    case CS_BTN_Action_Click:
      rgbcolor[0] = 255; break;
    case CS_BTN_Action_Longpress:
      rgbcolor[0] = 0; break;
  }
  CS_RGB_SetDim(rgbcolor);
}

int main()
{
  CS_Init(CS_INIT_BTN | CS_INIT_RGB | CS_INIT_DIM);
  CS_BTN_SetCallback(btnhandler);
  CS_RGB_SetDim(rgbcolor);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
  os_setcallback(controlloop);

	while (1) {
  }
	return 0;
}
