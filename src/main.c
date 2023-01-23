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

static CS_RGB_TypeDef rgbcolor = {255,0,0};
static CS_RGB_TypeDef patterns[] = {
  {0,0,0},
  {255,0,0},
  {0,255,0},
  {0,0,255},
  {255,255,255},
};
static const uint8_t patterncnt = sizeof(patterns)/sizeof(CS_RGB_TypeDef);
static bool tim_elapsed = false;
static unsigned int btncnt = 0;

static void controlloop(uint32_t looptime);
static void btnhandler(CS_BTN_Action_TypeDef value);

static uint8_t serhelp(char * outbuf, char * const cmdbuf __attribute__((unused)));
static uint8_t seradd(char * outbuf, char * const cmdbuf);
static uint8_t ser_measadc(char * outbuf, char * const cmdbuf);
static uint8_t ser_getbtnpresses(char * outbuf, char * const cmdbuf __attribute__((unused)));

/**
 * @brief control loop callback for os tick
 * @params looptime contains the tick count value
 */
static void controlloop(uint32_t looptime) {
  //LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
  cs_rot_handle();
  uint16_t leds = cs_rot_calcIndicator(cs_rot_getPos()%16,16);
  cs_rot_setIndicator(leds);
  CS_LoopHandler(looptime);
  ser_handle();
  //LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
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

/*
 * @brief serial function which adds 2 numbers
 * @note both serial arguments must be non-zero
 */
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

/*
 * @brief serial function which sets a dac value(arg 1) and prints the measured adc value
 */
static uint8_t ser_measadc(char * outbuf, char * const cmdbuf) {
  if (strlen(cmdbuf) < 1) {
    snprintf(outbuf, SER_MAX_RESPLEN, "Missing Argument error");
    return 1;
  }
  uint16_t ret = 0;
  uint16_t dacval = atoi(cmdbuf);
  ret += mydac_setval(dacval, MYADC_PIN_DAC1);
  uint16_t adcval = 100;
  ret += myadc_getval(&adcval);
  snprintf(outbuf, SER_MAX_RESPLEN, "ADC: %d", adcval);
  return ret;
}

/*
 * @brief serial function which prints the button fast presses since last long press
 */
static uint8_t ser_getbtnpresses(char * outbuf, char * const cmdbuf __attribute__((unused))) {
  snprintf(outbuf, SER_MAX_RESPLEN, "Button pressed %u times.", btncnt);
  return 0;
}

/**
 * @brief callback to demonstrate the os_timeout functionality
 */
void timeouthandler() {
  tim_elapsed = true;
}

int main()
{
  //LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  //LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
  //LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);

  CS_Init(CS_INIT_BTN);
  CS_BTN_SetCallback(btnhandler);

  //myadc_configure(MYADC_PIN_DAC1);

  ser_init();
  ser_addcmd('h', serhelp);
  ser_addcmd('a', seradd);
  ser_addcmd('m', ser_measadc);
  ser_addcmd('b', ser_getbtnpresses);

  cs_rot_init();
  cs_step_init();

  // enable event loop
  os_setcallback(controlloop);

  // wait for 3 seconds after board reset
  //os_timeout(3e9, timeouthandler);
  //while (!tim_elapsed);

  uint8_t pat = 0;
	while (1) {
    cs_step_move(-10000);
    os_timeout(250e6, NULL);
    ser_buf_TypeDef * buffer = ser_get_free_buf();
    if (buffer != NULL) {
      //snprintf(buffer->buf, SER_CMDBUFLEN, "Encoder: EMPTY\n");
      //snprintf(buffer->buf, SER_CMDBUFLEN, "Encoder: %li\n", cs_rot_getPos());
      snprintf(buffer->buf, SER_CMDBUFLEN, "Encoder: %li,%lu,%lu\n", cs_rot_getPos(), LL_TIM_GetCounter(TIM2), LL_TIM_GetCounter(TIM2));
      ser_txdata(buffer);
    }

  }
	return 0;
}
