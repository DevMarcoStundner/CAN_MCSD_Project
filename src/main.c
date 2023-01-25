#include "stm32l432xx.h"

#include "os/os.h"
#include "clickshield/clickshield.h"
#include "clickshield/rotary.h"
#include "clickshield/stepper.h"
#include "utils/serial.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static bool btnflag = false;
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
  CS_LoopHandler(looptime);
  ser_handle();
}

/**
 * @brief callback for btn event from clickshield
 * @params value (contains the current event)
 */
static void btnhandler(bool longpress) {
  static bool blink = false;
  if (longpress) {
    blink = !blink;
    cs_rot_setBlink(blink);
  } else {
    btnflag = true;
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
  cs_rot_setBtnCallback(btnhandler);
  cs_step_init();
  cs_step_setmode(CS_STEP_FULL);

  // enable event loop
  os_setcallback(controlloop);

  float pos = 0;
	while (1) {
    os_timeout(250e6, NULL);
    if (btnflag) {
      pos += 1.0F;
      cs_step_setPosition(pos);
      btnflag = false;
    }
    ser_buf_TypeDef * buffer = ser_get_free_buf();
    if (buffer != NULL) {
      snprintf(buffer->buf, SER_CMDBUFLEN, "Encoder: %li\n", (int32_t)(cs_rot_getPos()*100.0));
      ser_txdata(buffer);
    }
  }
	return 0;
}
