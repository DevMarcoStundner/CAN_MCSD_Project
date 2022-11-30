#include "adc.h"
#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"
#include "libll/stm32l4xx_ll_adc.h"
#include "libll/stm32l4xx_ll_dac.h"

typedef struct {
  unsigned long pkgpin;
  unsigned long channel;
} myadc_sources_TypeDef;

const myadc_sources_TypeDef myadc_sources[MYADC_MAX_PINS] = {
  {LL_GPIO_PIN_0, LL_ADC_CHANNEL_5}, // ADC_PIN_A0
  {LL_GPIO_PIN_1, LL_ADC_CHANNEL_6}, // ADC_PIN_A1
  {LL_GPIO_PIN_3, LL_ADC_CHANNEL_8}, // ADC_PIN_A2
  {LL_GPIO_PIN_4, LL_ADC_CHANNEL_9}, // ADC_PIN_A3, DAC_OUT1
  {LL_GPIO_PIN_5, LL_ADC_CHANNEL_10}, // ADC_PIN_A4, DAC_OUT2
  {LL_GPIO_PIN_6, LL_ADC_CHANNEL_11}, // ADC_PIN_A5
  {LL_GPIO_PIN_7, LL_ADC_CHANNEL_12}, // ADC_PIN_A6
  {LL_GPIO_PIN_2, LL_ADC_CHANNEL_7}, // ADC_PIN_A7
  {LL_DAC_CHANNEL_1, LL_ADC_CHANNEL_DAC1CH1}, // ADC_PIN_DAC1
  {LL_DAC_CHANNEL_2, LL_ADC_CHANNEL_DAC1CH2}, // ADC_PIN_DAC2
  {0, LL_ADC_CHANNEL_VREFINT}, // ADC_PIN_VREF
};

/*
 * @brief initialize adc and optionally dac/gpio
 * @param srcpin one from MYADC_PIN_x, selects the input channel to configure
 * @return 0 on success, 1 otherwise
 */
uint8_t myadc_configure(myadc_sources_EnumDef srcpin) {
  ErrorStatus err = ERROR;
  if (srcpin < MYADC_PIN_DAC1) {
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    // set pin to analog
    LL_GPIO_InitTypeDef gpio_init;
    LL_GPIO_StructInit(&gpio_init);
    gpio_init.Pin = myadc_sources[srcpin].pkgpin;
    gpio_init.Mode = LL_GPIO_MODE_ANALOG;
    err = LL_GPIO_Init(GPIOA, &gpio_init);
    if (err != SUCCESS) return 1;
  } else if (srcpin == MYADC_PIN_DAC1 || srcpin == MYADC_PIN_DAC2) {
    // enable dac clocks
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);
    LL_DAC_InitTypeDef dac_init;
    LL_DAC_StructInit(&dac_init);
    dac_init.OutputConnection = LL_DAC_OUTPUT_CONNECT_INTERNAL;
    err = LL_DAC_Init(DAC1, myadc_sources[srcpin].pkgpin, &dac_init);
    if (err != SUCCESS) return 1;
  }
  // config adc
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
  LL_ADC_DisableDeepPowerDown(ADC1);
  LL_ADC_EnableInternalRegulator(ADC1);
  for (int i=0; i<100000; i++) {
    __NOP();
  }
  if (LL_ADC_IsEnabled(ADC1)) {
    LL_ADC_Disable(ADC1);
  }
  LL_ADC_CommonInitTypeDef adccommon_init;
  LL_ADC_CommonStructInit(&adccommon_init);
  adccommon_init.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV1;
  err = LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(__ADC1__), &adccommon_init);
  if (err != SUCCESS) return 1;

  LL_ADC_InitTypeDef adc_init;
  LL_ADC_StructInit(&adc_init);
  adc_init.Resolution = LL_ADC_RESOLUTION_12B;
  err = LL_ADC_Init(ADC1, &adc_init);
  if (err != SUCCESS) return 1;

  // do calibration ?
  LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
  uint32_t adcal = LL_ADC_IsCalibrationOnGoing(ADC1);
  while (LL_ADC_IsCalibrationOnGoing(ADC1));
  for (int i=LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES; i>0; i--) {
    __NOP();
  }

  LL_ADC_REG_InitTypeDef adcreg_init;
  LL_ADC_REG_StructInit(&adcreg_init);
  err = LL_ADC_REG_Init(ADC1, &adcreg_init);
  if (err != SUCCESS) return 1;

  LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);
  LL_ADC_SetChannelSamplingTime(ADC1, myadc_sources[srcpin].channel, LL_ADC_SAMPLINGTIME_640CYCLES_5);
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, myadc_sources[srcpin].channel);
  return 0;
}

/*
 * @brief read value from configured adc channel
 * @param pvalue is a pointer to the value destination
 */
uint8_t myadc_getval(uint16_t * pvalue) {
  // enable adc
  LL_ADC_ClearFlag_ADRDY(ADC1);
  LL_ADC_Enable(ADC1);
  while (!LL_ADC_IsActiveFlag_ADRDY(ADC1)); 
  LL_ADC_ClearFlag_ADRDY(ADC1);
  // clear interrupts
  LL_ADC_ClearFlag_EOC(ADC1);
  // start conversion
  LL_ADC_REG_StartConversion(ADC1);
  // wait for finish
  while (!LL_ADC_IsActiveFlag_EOC(ADC1));
  // read value
  *pvalue = LL_ADC_REG_ReadConversionData12(ADC1);
  // disable adc
  LL_ADC_Disable(ADC1);
  while (LL_ADC_IsEnabled(ADC1));
  return 0;
}

/*
 * @brief set analog value to dac channel
 * @note Currently, only the internal adc connections can be used
 * @param value is the right aligned 12 bit dac value
 * @param pin either MYADC_PIN_DAC1 or MYADC_PIN_DAC2 depending on the desired channel
 */
uint8_t mydac_setval(uint16_t value, myadc_sources_EnumDef pin) {
  if (pin != MYADC_PIN_DAC1 && pin != MYADC_PIN_DAC2) {
    return 1;
  }
  uint32_t dacchan = myadc_sources[pin].pkgpin;
  LL_DAC_Enable(DAC1, dacchan);
  LL_DAC_ConvertData12RightAligned(DAC1, dacchan, value);
  while (value != LL_DAC_RetrieveOutputData(DAC1, dacchan));
  return 0;
}
