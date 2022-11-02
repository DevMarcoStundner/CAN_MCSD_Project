#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_utils.h"

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"

#include "os/os.h"
#include "clickshield/clickshield.h"

static volatile CS_RGB_TypeDef rgbcolor = {0,0,10};
static const CS_RGB_TypeDef patterns[] = {
  {0,0,0},
  {255,0,0},
  {0,255,0},
  {0,0,255},
};
static const uint8_t patterncnt = sizeof(patterns)/sizeof(CS_RGB_TypeDef);

/**
 * @brief control loop callback for os tick
 * @params looptime contains the tick count value
 */
void controlloop(uint32_t looptime) {
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
  CS_LoopHandler(looptime);
  LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
}

/**
 * @brief callback for btn event from clickshield
 * @params value (contains the current event)
 */
void btnhandler(CS_BTN_Action_TypeDef value) {
  static uint8_t patindex = 0;
  switch(value) {
    case CS_BTN_Action_Click:
      patindex++;
      if(patindex < patterncnt) break;
      __attribute__ ((fallthrough));
    case CS_BTN_Action_Longpress:
      patindex=0;
      break;
  }
  CS_RGB_SetDim(patterns[patindex]);
}

int main()
{
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);

  CS_Init(CS_INIT_BTN | CS_INIT_RGB | CS_INIT_DIM);
  CS_BTN_SetCallback(btnhandler);
  CS_RGB_SetDim(rgbcolor);

  os_setcallback(controlloop);

	while (1) {
  }
	return 0;
}
