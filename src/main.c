// change those depending on environment
#define DEF_HAVE_ENCODER 1
#define DEF_HAVE_STEPPER 1
#define DEF_HAVE_CAN 0

// dont touch this
#if !DEF_HAVE_CAN
#define DEF_HAVE_ENCODER 1
#define DEF_HAVE_STEPPER 1
#endif

#define CANID_ROT_POS 20
#define CANID_STEP_POS 10
#define CANID_STEP_STATE 11

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

static volatile bool inmenu = false;
static volatile float steppos = 0;

/**
 * @brief control loop callback for os tick
 * @params looptime contains the tick count value
 */
static void controlloop(uint32_t looptime) {
#if DEF_HAVE_ENCODER
  if (inmenu) {
    cs_rot_setIndicator(cs_rot_getPos());
  } else {
    cs_rot_setIndicator(steppos);
  }
  cs_rot_handle(looptime);
#endif
#if DEF_HAVE_STEPPER
  static int prevstepstate = 0;
  if (cs_step_getRunning() != prevstepstate) {
    prevstepstate = cs_step_getRunning();
    can_send_pkg(CANID_STEP_STATE, (uint8_t*)&prevstepstate, 4, NULL);
  }
  cs_step_handler(looptime);
  static uint32_t prevtime = 0;
  static float prevpos = 0;
  if (prevtime+100 < looptime) {
    prevtime = looptime;
    prevpos = cs_step_getPosition();
    can_send_pkg(CANID_STEP_POS, (uint8_t*)&prevpos, 4, NULL);
  }
#endif
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
  if (longpress) {
    cs_rot_reset();
  } else {
    inmenu = !inmenu;
    cs_rot_setBlink(inmenu);
    if (!inmenu) {
      pos = cs_rot_getPos();
      uint8_t data[8];
      // send can position
      can_send_pkg(CANID_ROT_POS, (uint8_t*)&pos, 4, NULL);
    }
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
}

static void canrxcallback(can_pkg_t * pkg) {
  float scratchf = 0;
  int scratchi = 0;
  switch(pkg->id) {
    case CANID_ROT_POS:
      scratchf = *(float*)pkg->data;
      cs_step_setPosition(scratchf);
      break;
    case CANID_STEP_POS:
      scratchf = *(float*)pkg->data;
      steppos = scratchf;
      break;
    case CANID_STEP_STATE:
      scratchi = *(int*)pkg->data;
      if (scratchi == 0) {
        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);
      } else {
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_8);
      }
      break;
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
  can_register_id(CANID_STEP_POS, canrxcallback);
  can_register_id(CANID_STEP_STATE, canrxcallback);
  cs_rot_init();
  cs_rot_setBtnCallback(btnhandler);
#endif
#if DEF_HAVE_STEPPER
  can_register_id(CANID_ROT_POS, canrxcallback);
  cs_step_init();
  cs_step_setmode(CS_STEP_FULL);
#endif

  can_init();

  // enable event loop
  os_setcallback(controlloop);

	while (1) {
    os_timeout(250e6, NULL);
  }
	return 0;
}
