#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"
#include "libll/stm32l4xx_ll_utils.h"

#include "clickshield/clickshield.h"

int main()
{
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
  CS_Init(CS_INIT_BUTTON | CS_INIT_RGB);
  CS_RGB_TypeDef rgbcolor = {1,1,1};

	while (1) {
    CS_RGB_Set(rgbcolor);
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
    LL_mDelay(250);
  }
	return 0;
}
