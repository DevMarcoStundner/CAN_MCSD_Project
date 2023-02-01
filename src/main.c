// change those depending on environment
#define DEF_HAVE_ENCODER 1
#define DEF_HAVE_STEPPER 1
#define DEF_HAVE_CAN 0

// dont touch this
#if !DEF_HAVE_CAN
#define DEF_HAVE_ENCODER 1
#define DEF_HAVE_STEPPER 1
#endif

#include "stm32l432xx.h"

#include "os/os.h"
#include "clickshield/clickshield.h"
#include "clickshield/rotary.h"
#include "clickshield/stepper.h"
#include "utils/serial.h"
#include "can/can.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"

static void controlloop(uint32_t looptime);
static void btnhandler(bool longpress);

static uint8_t serhelp(char * outbuf, char * const cmdbuf __attribute__((unused)));

/**
 * @brief control loop callback for os tick
 * @params looptime contains the tick count value
 */
static void controlloop(uint32_t looptime) {
  cs_rot_handle(looptime);
  cs_rot_setIndicator(cs_rot_getPos());
  cs_step_handler(looptime);
  can_handle(looptime);
  CS_LoopHandler(looptime);
  ser_handle();
}

/**
 * @brief callback for btn event from clickshield
 * @params value (contains the current event)
 */
static void btnhandler(bool longpress) {
  static float pos = 0;
  static bool blink = false;
  if (longpress) {
    blink = !blink;
    cs_rot_setBlink(blink);
  } else {
    pos += 10.0F;
    cs_step_setPosition(pos);
  }
}

/*
 * @brief serial function which returns a helpful message
 */
static uint8_t serhelp(char * outbuf, char * const cmdbuf __attribute__((unused))) {
  sprintf(outbuf, "Command format: #<c>,arg,arg\\n  c->command");
  return 0;
}

static void cantxcallback(can_ll_txmbx_t mailbox) {
  LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_8);
}

static void canrxcallback(can_pkg_t * pkg) {
  ser_buf_TypeDef * buffer = ser_get_free_buf();
  if (buffer != NULL) {
    snprintf(buffer->buf, SER_CMDBUFLEN, "Got PKG: id-%lu, len-%u\n", pkg->id, pkg->len);
    ser_txdata(buffer);
  }
}

int main()
{
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);
  CS_Init(CS_INIT_BTN);
  CS_BTN_SetCallback(btnhandler);

  ser_init();
  ser_addcmd('h', serhelp);

#if DEF_HAVE_ENCODER
  cs_rot_init();
  cs_rot_setBtnCallback(btnhandler);
#endif
#if DEF_HAVE_STEPPER
  cs_step_init();
  cs_step_setmode(CS_STEP_FULL);
#endif

  can_init();
  can_register_id(22, canrxcallback);

  // enable event loop
  os_setcallback(controlloop);

  uint8_t data[2] = "HI";
  can_ll_txmbx_t mailbox = 0;
	while (1) {
    os_timeout(250e6, NULL);
    mailbox = can_send_pkg(22, data, 8, NULL);
    mailbox = can_send_pkg(22, data, 8, NULL);
    ser_buf_TypeDef * buffer = ser_get_free_buf();
    if (buffer != NULL) {
      snprintf(buffer->buf, SER_CMDBUFLEN, "Can ERRORS: %i\n", can_get_errors());
      ser_txdata(buffer);
    }
  }
	return 0;
}
