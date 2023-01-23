#include "stepper.h"

#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"
#include "libll/stm32l4xx_ll_tim.h"

#define STEPSD_PORT PORTB
#define STEPSTEP_PIN LL_GPIO_PIN_1
#define STEPDIR_PIN LL_GPIO_PIN_0
#define STEPMS1_PORT PORTA
#define STEPMS2_PORT PORTC
#define STEPMS1_PIN LL_GPIO_PIN_0
#define STEPMS2_PIN LL_GPIO_PIN_14
#define STEPEN_PORT PORTA
#define STEPEN_PIN LL_GPIO_PIN_3

#define 

void cs_step_init() {
  // init gpio
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

  LL_GPIO_InitTypeDef s_gpioinit;
  LL_GPIO_StructInit(&s_gpioinit);
  s_gpioinit.Pin = STEPSTEP_PIN;
  s_gpioinit.Mode = LL_GPIO_MODE_ALTERNATE;
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

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  LL_TIM_InitTypeDef s_timinit;
  LL_TIM_StructInit(&s_timinit);
  s_timinit.Prescaler = 800; // with ARRL=1000 frequency of 800Hz
  s_timinit.Autoreload = 1000;
  s_timinit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  //s_timinit.RepetitionCounter = 0; //is set afterwards
  LL_TIM_Init(TIM1, &s_timinit);

  TIM_OC_InitStruct s_timocinit;
  LL_TIM_OC_StructInit(&s_timocinit);
  s_timocinit.CompareValue = s_timinit.Autoreload/2;
  s_timocinit.OCMode = LL_TIM_OCMODE_ACTIVE;
  //s_timocinit.OCMode = LL_TIM_OCMODE_PWM1;
  s_timocinit.OCNState = LL_TIM_OCSTATE_ENABLE;
  s_timocinit.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  s_timocinit.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3, &s_timocinit);
}
