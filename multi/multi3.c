/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * Modificaciones:
 * - En hal_adc_lld.c, funcion adc_lld_serve_interrupt, comentar emask para caso de actuacion AWD
 * - En halconf.h incluir #define  STM32_ADC_ADC12_IRQ_HOOK  if (isr & ADC_ISR_AWD1) adcawd1callback();
 */

#include "ch.h"
#include "hal.h"
#include "ccportab.h"

void initTimers(void);


/*===========================================================================*/
/* ADC driver related.                                                       */
/*===========================================================================*/
extern const ADCConfig portab_adccfg3;
extern const ADCConversionGroup portab_adcgrpcfg3;

#define ADC_GRP3_BUF_DEPTH      4
#define ADC_GRP3_NUM_CHANNELS   2

#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
#endif
adcsample_t samples3[CACHE_SIZE_ALIGN(adcsample_t, ADC_GRP3_NUM_CHANNELS * ADC_GRP3_BUF_DEPTH)];

/*
 * ADC streaming callback.
 */
size_t multi_n3= 0, multi_nx3 = 0, multi_ny3 = 0;
void adc3callback(ADCDriver *adcp) {

  /* Updating counters.*/
  multi_n3++;
  if (adcIsBufferComplete(adcp)) {
      multi_nx3 += 1;
  }
  else {
    multi_ny3 += 1;
  }
  if ((multi_n3 % 200) == 0U) {
#if defined(LINE_LED)
    palToggleLine(LINE_LED);
#endif
  }
}

/*
 * ADC3 errors callback, should never happen.
 */
void adc3errorcallback(ADCDriver *adcp, adcerror_t err) {

  (void)adcp;
  (void)err;
  chSysHalt("it happened");
}



const ADCConfig portab_adccfg3 = {
  .difsel       = 0U,
  .calibration  = 0U
};

/*
 * ADC conversion group.
 * Mode:        Continuous, 2 channels, HW triggered by GPT4-TRGO.
 * Channels:    IN4.
 * 4-20mA 1 => PC1   ADC123_INP11
 * 4-20mA 2 => PC3_C ADC3_INP1
 */
const ADCConversionGroup portab_adcgrpcfg3 = {
  .circular     = true,
  .num_channels = ADC_GRP3_NUM_CHANNELS,
  .end_cb       = adc3callback,
  .error_cb     = adc3errorcallback,
  .cfgr         = ADC_CFGR_CONT_ENABLED | ADC_CFGR_RES_12BITS,// | ADC_CFGR_EXTSEL_SRC(0b01100) ,//| ADC_CFGR_AWD1SGL, //| ADC_CFGR_CONT_ENABLED |
                      // el GPT4 usa TIM4_TRGO
  .cfgr2        = 0U,
  .ccr          = 0U,
  .pcsel        = ADC_SELMASK_IN1 | ADC_SELMASK_IN11,
  .ltr1         = 0x00000000U,
  .htr1         = 0x03FFFFFFU,
  .ltr2         = 0x00000000U,
  .htr2         = 0x03FFFFFFU,
  .ltr3         = 0x00000000U,
  .htr3         = 0x03FFFFFFU,
  .smpr         = {
    ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_384P5) | ADC_SMPR2_SMP_AN11(ADC_SMPR_SMP_384P5),
    0U
  },
  .sqr          = {
    ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1) | ADC_SQR1_SQ2_N(ADC_CHANNEL_IN11),
    0U,
    0U,
    0U
  }
};

/*
 * GPT configuration.
 */
const GPTConfig portab_gptcfg3 = {
  .frequency    =  20000U,
  .callback     =  NULL,
  .cr2          =  TIM_CR2_MMS_1,   /* MMS = 010 = TRGO on Update Event.    */
  .dier         =  0U
};

int multi3(void) {

    /*
     * Starting PORTAB_GPT1 driver, it is used for triggering the ADC.
     */
    gptStart(&GPTD4, &portab_gptcfg3);
    palSetLineMode(LINE_4_20mA_1, PAL_MODE_INPUT_ANALOG);
    palSetLineMode(LINE_4_20mA_2, PAL_MODE_INPUT_ANALOG);


  adcStart(&ADCD3, &portab_adccfg3);
  adcStartConversion(&ADCD3, &portab_adcgrpcfg3, samples3, ADC_GRP3_BUF_DEPTH);

  while (true) {
    cacheBufferInvalidate(samples3, sizeof (samples3) / sizeof (adcsample_t));
    uint32_t val = ADC3->DR;
    chThdSleepMilliseconds(500);
  }
  return 0;
}
