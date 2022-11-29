#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_utils.h"

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"

#include "os/os.h"
#include "utils/serial.h"
#include "clickshield/clickshield.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
static void controlloop(uint32_t looptime) {
  //LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
  CS_LoopHandler(looptime);
  //LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
  ser_handle();
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

static uint8_t serhelp(char * outbuf, char * const cmdbuf __attribute__((unused))) {
  sprintf(outbuf, "Command format: #<c>,arg,arg\\n  c->command");
  return 0;
}

static uint8_t seradd(char * outbuf, char * const cmdbuf) {
  int a=0,b=0;
  a = atoi(cmdbuf);
  char * const blocation = strchr(cmdbuf,',');
  if (blocation != NULL) {
    b = atoi(blocation+1);
  } else {
    snprintf(outbuf, SER_MAX_RESPLEN, "Missing Argument error");
    return 1;
  }
  if (a!=0 && b!=0) {
    snprintf(outbuf, SER_MAX_RESPLEN, "result: %d", a+b);
  } else {
    snprintf(outbuf, SER_MAX_RESPLEN, "Argument error");
    return 1;
  }
  return 0;
}

/**
 * @brief callback to demonstrate the os_timeout functionality
 */
void timeouthandler() {
  LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
}

int main()
{
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);

  CS_Init(CS_INIT_BTN | CS_INIT_RGB | CS_INIT_DIM);
  CS_BTN_SetCallback(btnhandler);
  CS_RGB_SetDim(rgbcolor);

  ser_init();
  ser_addcmd('a', seradd);
  ser_addcmd('h', serhelp);

  os_setcallback(controlloop);

  // light the board diode for 3 seconds
  os_timeout(3e9, timeouthandler);

	while (1) {
    // toggle the board diode with a period of 500ms
    os_timeout(250e6, NULL);
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
  }
	return 0;
}
