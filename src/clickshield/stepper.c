#include "stepper.h"

#include "stm32l432xx.h"
#include "libcmsis/system_stm32l4xx.h"
#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"
#include "libll/stm32l4xx_ll_tim.h"

#define STEPSD_PORT GPIOB
#define STEPSTEP_PIN LL_GPIO_PIN_1
#define STEPDIR_PIN LL_GPIO_PIN_0
#define STEPMS1_PORT GPIOA
#define STEPMS2_PORT GPIOC
#define STEPMS1_PIN LL_GPIO_PIN_0
#define STEPMS2_PIN LL_GPIO_PIN_14
#define STEPEN_PORT GPIOA
#define STEPEN_PIN LL_GPIO_PIN_3

static volatile enum {
  RUN_IDLE=0,
  RUN_LATCH,
  RUN_END
} control_status;

static int32_t lastpos = 0;
static int32_t newpos = 0;
static volatile int32_t relpos = 0;

void TIM1_UP_TIM16_IRQHandler() {
  if (LL_TIM_IsActiveFlag_UPDATE(TIM1)) {
    LL_TIM_ClearFlag_UPDATE(TIM1);
    if (control_status == RUN_LATCH) {
      // make next move
    } else {
      control_status = RUN_IDLE;
    }
  }
}

void TIM1_CC_IRQHandler() {
  if (LL_TIM_IsActiveFlag_CC3(TIM1)) {
    LL_TIM_ClearFlag_CC3(TIM3);
    if (relpos > ((uint32_t)1<<16)-1) {
      relpos -= 0xFFFF;
      control_status = RUN_LATCH;
      LL_TIM_SetRepetitionCounter(TIM1, (uint16_t)0xFFFF-1);
    } else {
      LL_TIM_SetRepetitionCounter(TIM1, (uint16_t)relpos);
      LL_TIM_SetOnePulseMode(TIM1, LL_TIM_ONEPULSEMODE_SINGLE);
      control_status = RUN_END;
    }
  }
}

void cs_step_init() {
  // init gpio
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  LL_GPIO_InitTypeDef s_gpioinit;
  LL_GPIO_StructInit(&s_gpioinit);
  s_gpioinit.Pin = STEPSTEP_PIN;
  s_gpioinit.Mode = LL_GPIO_MODE_ALTERNATE;
  //s_gpioinit.Mode = LL_GPIO_MODE_OUTPUT;
  s_gpioinit.Speed = LL_GPIO_SPEED_HIGH;
  s_gpioinit.Alternate = LL_GPIO_AF_1; // TIM1
  LL_GPIO_Init(STEPSD_PORT, &s_gpioinit);

  s_gpioinit.Pin = STEPDIR_PIN;
  s_gpioinit.Mode = LL_GPIO_MODE_OUTPUT;
  s_gpioinit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(STEPSD_PORT, &s_gpioinit);

  s_gpioinit.Pin = STEPDIR_PIN;
  LL_GPIO_Init(STEPSD_PORT, &s_gpioinit);

  s_gpioinit.Pin = STEPMS1_PIN;
  LL_GPIO_Init(STEPMS1_PORT, &s_gpioinit);

  s_gpioinit.Pin = STEPMS2_PIN;
  LL_GPIO_Init(STEPMS2_PORT, &s_gpioinit);

  s_gpioinit.Pin = STEPEN_PIN;
  LL_GPIO_Init(STEPEN_PORT, &s_gpioinit);

  LL_TIM_InitTypeDef s_timinit;
  LL_TIM_StructInit(&s_timinit);
  s_timinit.Prescaler = 8000; // 100Hz pwm frequency
  s_timinit.Autoreload = 100;
  s_timinit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  s_timinit.RepetitionCounter = 0; // is set afterwards
  LL_TIM_Init(TIM1, &s_timinit);

  LL_TIM_OC_InitTypeDef s_timocinit;
  LL_TIM_OC_StructInit(&s_timocinit);
  s_timocinit.CompareValue = s_timinit.Autoreload/2;
  s_timocinit.OCMode = LL_TIM_OCMODE_PWM2;
  s_timocinit.OCNState = LL_TIM_OCSTATE_ENABLE;
  s_timocinit.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  s_timocinit.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3, &s_timocinit);

  LL_TIM_EnableIT_UPDATE(TIM1);
  NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
  NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0x01);

  LL_TIM_EnableAllOutputs(TIM1);
  LL_GPIO_SetOutputPin(STEPEN_PORT, STEPEN_PIN);
}

void cs_step_setmode(cs_step_mode_t mode) {
  if (mode == CS_STEP_FULL || mode == CS_STEP_QUARTER) {
    LL_GPIO_ResetOutputPin(STEPMS1_PORT, STEPMS1_PIN);
  } else {
    LL_GPIO_SetOutputPin(STEPMS1_PORT, STEPMS1_PIN);
  }
  if (mode == CS_STEP_FULL || mode == CS_STEP_HALF) {
    LL_GPIO_ResetOutputPin(STEPMS2_PORT, STEPMS2_PIN);
  } else {
    LL_GPIO_SetOutputPin(STEPMS2_PORT, STEPMS2_PIN);
  }
}

int cs_step_move(int32_t pos) {
  if (LL_TIM_IsEnabledCounter(TIM1)) {
    return 1; // Error as move is currently active
  }
  // calc position increment
  relpos = pos - newpos;
  newpos = pos + relpos;
  if (relpos < 0) {
    LL_GPIO_ResetOutputPin(STEPSD_PORT, STEPDIR_PIN);
  } else {
    LL_GPIO_SetOutputPin(STEPSD_PORT, STEPDIR_PIN);
  }
  relpos = (uint32_t)relpos; // make value positive
  if (relpos > ((uint32_t)1<<16)-1) {
    relpos -= 0xFFFF;
    control_status = RUN_LATCH;
    LL_TIM_SetRepetitionCounter(TIM1, (uint16_t)0xFFFF-1);
  } else {
    LL_TIM_SetRepetitionCounter(TIM1, (uint16_t)relpos);
    LL_TIM_SetOnePulseMode(TIM1, LL_TIM_ONEPULSEMODE_SINGLE);
    control_status = RUN_END;
  }
  LL_TIM_EnableCounter(TIM1);

  return 0;
}
