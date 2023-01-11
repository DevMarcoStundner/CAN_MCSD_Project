#include "rotary.h"
#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_gpio.h"

#define ROTA_PIN  LL_GPIO_PIN1
#define ROTA_PORT GPIOA
#define ROTB_PIN  LL_GPIO_PIN5
#define ROTB_PORT GPIOA

#define ROTBTN_PIN LL_GPIO_PIN4
#define ROTBTN_PORT GPIOA
#define ROTSPI_PORT GPIOB
#define ROTMOSI_PIN LL_GPIO_PIN5
#define ROTMISO_PIN LL_GPIO_PIN4
#define ROTSCK_PIN LL_GPIO_PIN3

#define ROTCS_PIN LL_GPIO_PIN6
#define ROTCS_PORT GPIOA

static int32_t position = 0;
static int32_t overflow = 0;
static int32_t lastpos = position;

void cs_rot_init() {
  // init gpio
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

int32_t cs_rot_getOverflow() {
  // return overflow count
}

int32_t cs_rot_getDiff(){
  // return difference to last position
}
