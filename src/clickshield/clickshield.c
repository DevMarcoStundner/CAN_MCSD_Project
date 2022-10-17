#include "clickshield.h"
#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"
#include "utils/utils.h"

typedef struct {
  bool rgb;
  bool btn;
  bool poti;
  bool dim;
} peripheralsTypeDef;

static peripheralsTypeDef activePeripherals = {0,0,0,0};
static const uint32_t rgbpinmask = LL_GPIO_PIN_4 | LL_GPIO_PIN_6 | LL_GPIO_PIN_8;
static const uint32_t rgbperiod = 5;
static const uint32_t rgbpins[3] = {6, 8, 4}; // red, green, blue
volatile static uint8_t rgbvalues[3] = {0,0,0};
volatile static uint8_t rgbstates[3] = {0,0,0};
static const uint8_t btndelay = 10;
static const uint8_t btnlongpress = 500;
static bool btnvalue = false;
static void (*btncallback)(CS_BTN_Action_TypeDef) = 0;
static const bool btnactstate = false;

static void btnEventHandler(uint32_t time) {
  static uint32_t btnepoch = 0;
  if (btnvalue == btnactstate) {
    btnepoch = time;
  } else if (time-btnepoch < btnlongpress) {
    if (btncallback != 0) btncallback(CS_BTN_Action_Click);
  } else if (time-btnepoch > btnlongpress) {
    if (btncallback != 0) btncallback(CS_BTN_Action_Longpress);
  }
}

void CS_LoopHandler(uint32_t current_time) {
  static uint32_t rgbepoch = 0;
  static uint32_t btnlastfix = 0;
  if (activePeripherals.dim) {
    if (current_time-rgbepoch > rgbperiod) {
      rgbepoch = current_time;
    }
    for(uint8_t i=0; i<3; i++) {
      rgbstates[i] = ((current_time-rgbepoch) < rgbvalues[i]);
    }
    CS_RGB_SetPins(rgbstates);
  }
  if (activePeripherals.btn) {
    bool btntmp = CS_BTN_ReadRaw();
    if ((current_time-btnlastfix > btndelay) && (btntmp != btnvalue)) {
      btnvalue = btntmp;
      btnEventHandler(current_time);
    } else if (btntmp != btnvalue) {
      btnlastfix = current_time;
    }
  }
}

void CS_Init(uint16_t initDevices) {
  if (initDevices & CS_INIT_RGB || initDevices & CS_INIT_DIM) {
    activePeripherals.rgb = true;
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_InitTypeDef s_gpioinit;
    LL_GPIO_StructInit(&s_gpioinit);
    s_gpioinit.Pin = rgbpinmask;
    s_gpioinit.Mode = LL_GPIO_MODE_OUTPUT;
    s_gpioinit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    s_gpioinit.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    LL_GPIO_Init(GPIOA, &s_gpioinit);
    CS_RGB_SetPins(rgbstates);
  }
  if (initDevices & CS_INIT_DIM) {
    activePeripherals.dim = true;
  }
  if (initDevices & CS_INIT_BTN) {
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_InitTypeDef s_gpioinit;
    LL_GPIO_StructInit(&s_gpioinit);
    s_gpioinit.Pin = LL_GPIO_PIN_3;
    s_gpioinit.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(GPIOA, &s_gpioinit);
  }
  if (initDevices & CS_INIT_POT) {
  }
}

void CS_BTN_SetCallback(void (*function)(CS_BTN_Action_TypeDef)) {
  btncallback = function;
}

bool CS_BTN_ReadDeb() {
  return btnvalue;
}

bool CS_BTN_ReadRaw() {
  return LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_3);
}

void CS_RGB_SetDim(CS_RGB_TypeDef color) {
  for (uint8_t i=0; i<3; i++) {
    rgbvalues[i] = rgbperiod * color[i] / 255;
  }
}

void CS_RGB_SetPins(CS_RGB_TypeDef color) {
  uint32_t pinstates = 0;
  for (uint8_t i=0; i<3; i++) {
    if (color[i]) {
      pinstates |= (1<<rgbpins[i]);
    }
  }
  // inverted as led is active-low
  LL_GPIO_ResetOutputPin(GPIOA, pinstates);
  LL_GPIO_SetOutputPin(GPIOA, rgbpinmask & (~pinstates));
}
