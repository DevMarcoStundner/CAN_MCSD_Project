#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_utils.h"

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"

// debugging only
#include "libll/stm32l4xx_ll_tim.h"

#include "os/os.h"
#include "utils/serial.h"
#include "utils/adc.h"
#include "clickshield/clickshield.h"
#include "clickshield/rotary.h"
#include "clickshield/stepper.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static bool tim_elapsed = false;
static unsigned int btncnt = 0;

static void controlloop(uint32_t looptime);
static void btnhandler(CS_BTN_Action_TypeDef value);

static uint8_t serhelp(char * outbuf, char * const cmdbuf __attribute__((unused)));

/**
 * @brief control loop callback for os tick
 * @params looptime contains the tick count value
 */
static void controlloop(uint32_t looptime) {
  cs_rot_handle();
  uint16_t leds = cs_rot_calcIndicator(cs_rot_getPos()%30,30);
  cs_rot_setIndicator(leds);
  CS_LoopHandler(looptime);
  ser_handle();
}

/**
 * @brief callback for btn event from clickshield
 * @params value (contains the current event)
 */
static void btnhandler(CS_BTN_Action_TypeDef value) {
  switch(value) {
    case CS_BTN_Action_Click:
      btncnt++;
      break;
    case CS_BTN_Action_Longpress:
      btncnt=0;
      break;
  }
}

/*
 * @brief serial function which returns a helpful message
 */
static uint8_t serhelp(char * outbuf, char * const cmdbuf __attribute__((unused))) {
  sprintf(outbuf, "Command format: #<c>,arg,arg\\n  c->command");
  return 0;
}

int main()
{
  CS_Init(CS_INIT_BTN);
  CS_BTN_SetCallback(btnhandler);

  ser_init();
  ser_addcmd('h', serhelp);

  cs_rot_init();
  cs_step_init();
  cs_step_setmode(CS_STEP_FULL);

  // enable event loop
  os_setcallback(controlloop);

	while (1) {
    cs_step_move(10);
    os_timeout(250e6, NULL);
    /*
    ser_buf_TypeDef * buffer = ser_get_free_buf();
    if (buffer != NULL) {
      //snprintf(buffer->buf, SER_CMDBUFLEN, "Encoder: EMPTY\n");
      //snprintf(buffer->buf, SER_CMDBUFLEN, "Encoder: %li\n", cs_rot_getPos());
      snprintf(buffer->buf, SER_CMDBUFLEN, "Encoder: %li,%lu,%lu\n", cs_rot_getPos(), LL_TIM_GetCounter(TIM2), LL_TIM_GetCounter(TIM2));
      ser_txdata(buffer);
    }
    */
  }
	return 0;
}
