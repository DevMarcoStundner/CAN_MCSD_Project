#include "rotary.h"
#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"
#include "libll/stm32l4xx_ll_spi.h"
#include "libll/stm32l4xx_ll_tim.h"

#define ROTA_PIN  LL_GPIO_PIN_1
#define ROTA_PORT GPIOA
#define ROTB_PIN  LL_GPIO_PIN_5
#define ROTB_PORT GPIOA

#define ROTBTN_PIN LL_GPIO_PIN_4
#define ROTBTN_PORT GPIOA
#define ROTSPI_PORT GPIOB
#define ROTMOSI_PIN LL_GPIO_PIN_5
#define ROTMISO_PIN LL_GPIO_PIN_4
#define ROTSCK_PIN LL_GPIO_PIN_3

#define ROTCS_PIN LL_GPIO_PIN_6
#define ROTCS_PORT GPIOA
#define ROTRESET_PIN LL_GPIO_PIN_15
#define ROTRESET_PORT GPIOC

const static uint32_t maxcnt = 5;
static volatile int32_t position = 0;
static volatile int32_t overflow = 0;
static volatile uint32_t lastcnt = 0;
static uint32_t tmpcntval = 0;

// timer 2 interrupt handler
void TIM2_IRQHandler() {
  if (LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
    tmpcntval = LL_TIM_GetCounter(TIM2);
    LL_TIM_ClearFlag_UPDATE(TIM2);
    if (LL_TIM_GetCounter(TIM2) > maxcnt/2) {
      overflow--;
      lastcnt++; // to avoid off by one error
    } else {
      overflow++;
      lastcnt--; // to avoid off by one error
    }
  }
}

void cs_rot_init() {
  // init gpio
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

  LL_GPIO_InitTypeDef s_gpioinit;
  LL_GPIO_StructInit(&s_gpioinit);
  s_gpioinit.Pin = ROTA_PIN | ROTB_PIN;
  s_gpioinit.Mode = LL_GPIO_MODE_ALTERNATE;
  s_gpioinit.Speed = LL_GPIO_SPEED_HIGH;
  s_gpioinit.Alternate = LL_GPIO_AF_1; // TIM2
  LL_GPIO_Init(ROTA_PORT, &s_gpioinit);

  s_gpioinit.Pin = ROTBTN_PIN;
  s_gpioinit.Pull = LL_GPIO_PULL_UP;
  s_gpioinit.Mode = LL_GPIO_MODE_INPUT;
  LL_GPIO_Init(ROTBTN_PORT, &s_gpioinit);

  s_gpioinit.Pin = ROTCS_PIN;
  s_gpioinit.Pull = LL_GPIO_PULL_NO;
  s_gpioinit.Mode = LL_GPIO_MODE_OUTPUT;
  s_gpioinit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(ROTCS_PORT, &s_gpioinit);

  s_gpioinit.Pin = ROTRESET_PIN;
  LL_GPIO_Init(ROTRESET_PORT, &s_gpioinit);

  s_gpioinit.Pin = ROTSCK_PIN | ROTMOSI_PIN;
  s_gpioinit.Mode = LL_GPIO_MODE_ALTERNATE;
  s_gpioinit.Alternate = LL_GPIO_AF_5; // SPI1
  LL_GPIO_Init(ROTSPI_PORT, &s_gpioinit);

  // init spi
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

  LL_SPI_InitTypeDef s_spiinit;
  LL_SPI_StructInit(&s_spiinit);
  s_spiinit.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
  s_spiinit.Mode = LL_SPI_MODE_MASTER;
  s_spiinit.DataWidth = LL_SPI_DATAWIDTH_16BIT;
  s_spiinit.ClockPolarity = LL_SPI_POLARITY_LOW;
  s_spiinit.ClockPhase = LL_SPI_PHASE_1EDGE;
  s_spiinit.NSS = LL_SPI_NSS_SOFT;
  s_spiinit.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
  s_spiinit.BitOrder = LL_SPI_MSB_FIRST;
  LL_SPI_Init(SPI1, &s_spiinit);

  LL_SPI_Enable(SPI1);

  // init timer
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  LL_TIM_InitTypeDef s_timinit;
  LL_TIM_StructInit(&s_timinit);
  s_timinit.Prescaler = 0;
  s_timinit.Autoreload = maxcnt;
  s_timinit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  //s_timinit.RepetitionCounter = 0;
  LL_TIM_Init(TIM2, &s_timinit);

  LL_TIM_ENCODER_InitTypeDef s_timencinit;
  s_timencinit.EncoderMode = LL_TIM_ENCODERMODE_X2_TI2;
  s_timencinit.IC1Polarity = LL_TIM_IC_POLARITY_BOTHEDGE;
  s_timencinit.IC1ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
  s_timencinit.IC1Prescaler = LL_TIM_ICPSC_DIV1;
  s_timencinit.IC1Filter = LL_TIM_IC_FILTER_FDIV32_N8;
  s_timencinit.IC2Polarity = LL_TIM_IC_POLARITY_RISING;
  s_timencinit.IC2ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
  s_timencinit.IC2Prescaler = LL_TIM_ICPSC_DIV1;
  s_timencinit.IC2Filter = LL_TIM_IC_FILTER_FDIV32_N8;
  LL_TIM_ENCODER_Init(TIM2, &s_timencinit);
  LL_TIM_EnableIT_UPDATE(TIM2);
  LL_TIM_ClearFlag_UPDATE(TIM2);
  NVIC_EnableIRQ(TIM2_IRQn);
  NVIC_SetPriority(TIM2_IRQn, 0x01);

  LL_TIM_EnableCounter(TIM2);

  // set up variables

}

void cs_rot_reset() {
  // reset timer
  // reset variables
}

void cs_rot_handle() {
  NVIC_DisableIRQ(TIM2_IRQn);
  uint32_t timcnt = LL_TIM_GetCounter(TIM2);
  position += timcnt - lastcnt + overflow*maxcnt;
  lastcnt = timcnt;
  overflow = 0;
  NVIC_EnableIRQ(TIM2_IRQn);
}

int32_t cs_rot_getPos() {
  // return current position
  return position;
}

uint16_t cs_rot_calcIndicator(int32_t position, uint32_t fullscale) {
  int32_t poscnt = (position*16)/fullscale;
  if (position < 0) {
    poscnt = 16+poscnt;
  }
  return 1<<(poscnt);
}

void cs_rot_setIndicator(uint16_t leds) {
  // cs low
  LL_GPIO_ResetOutputPin(ROTCS_PORT, ROTCS_PIN);
  LL_SPI_TransmitData16(SPI1, leds);
  while(LL_SPI_IsActiveFlag_BSY(SPI1));
  LL_GPIO_SetOutputPin(ROTCS_PORT, ROTCS_PIN);
  // cs high
}

int32_t cs_rot_getOverflow() {
  // return overflow count
}

int32_t cs_rot_getDiff(){
  // return difference to last position
}
