#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_utils.h"

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"

#include "os/os.h"
#include "clickshield/clickshield.h"

void HardFault_Handler() {
  while(1);
}

int main()
{
  /*
  */
  CS_Init(CS_INIT_BUTTON | CS_INIT_RGB);
  CS_RGB_TypeDef rgbcolor = {1,1,1};
  CS_RGB_Set(rgbcolor);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);

	while (1) {
    uint32_t looptime = os_time();
    CS_LoopHandler(looptime);
    LL_mDelay(250);
  }
	return 0;
}
