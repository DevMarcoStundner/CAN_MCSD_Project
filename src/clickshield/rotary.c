#include "rotary.h"
#include "stm32l432xx.h"
//#include "libll/stm32l4xx_ll_rcc.h"
#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"
#include "libll/stm32l4xx_ll_spi.h"

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

static int32_t position = 0;
static int32_t overflow = 0;
static int32_t lastpos = 0;
//static uint16_t leds;

// spi1 interrupt handler

// timer 2 interrupt handler

void cs_rot_init() {
  // init gpio
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

  LL_GPIO_InitTypeDef s_gpioinit;
  LL_GPIO_StructInit(&s_gpioinit);
  s_gpioinit.Pin = ROTA_PIN | ROTB_PIN;
  s_gpioinit.Mode = LL_GPIO_MODE_INPUT;
  s_gpioinit.Speed = LL_GPIO_SPEED_HIGH;
  LL_GPIO_Init(ROTA_PORT, &s_gpioinit);

  s_gpioinit.Pin = ROTBTN_PIN;
  s_gpioinit.Pull = LL_GPIO_PULL_UP;
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

  // set up variables

}

void cs_rot_reset() {
  // reset timer
  // reset variables
}

int32_t cs_rot_getPos() {
  // return current position
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
