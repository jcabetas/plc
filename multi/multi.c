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

#include "ch.h"
#include "hal.h"
#include "ccportab.h"

#include "portab.h"


void initTimers(void);


/*===========================================================================*/
/* ADC driver related.                                                       */
/*===========================================================================*/

#define ADC_GRP1_BUF_DEPTH      2
#define ADC_GRP2_BUF_DEPTH      2

/* Buffers are allocated with size and address aligned to the cache
   line size.*/
#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
#endif
adcsample_t samples1[CACHE_SIZE_ALIGN(adcsample_t, ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH)];

#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
#endif
adcsample_t samples2multi[CACHE_SIZE_ALIGN(adcsample_t, ADC_GRP2_NUM_CHANNELS * ADC_GRP2_BUF_DEPTH)];

/*
 * ADC streaming callback.
 */
size_t multi_n= 0, multi_nx = 0, multi_ny = 0;
void adccallback(ADCDriver *adcp) {

  /* Updating counters.*/
  multi_n++;
  if (adcIsBufferComplete(adcp)) {
      multi_nx += 1;
  }
  else {
    multi_ny += 1;
  }

  if ((multi_n % 200) == 0U) {
#if defined(PORTAB_LINE_LED2)
    palToggleLine(PORTAB_LINE_LED2);
#endif
  }
}

/*
 * ADC errors callback, should never happen.
 */
void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {

  (void)adcp;
  (void)err;
  if (err == ADC_ERR_AWD1)
  {
      uint8_t awdHappened = 1;
      uint32_t dato = ADC1->DR;
      uint32_t min = ADC1->LTR1;
      uint32_t max = ADC1->HTR1;
      //ADC1->ISR |= ADC_ISR_AWD1; // limpiamos la interrupcion
  }
  //chSysHalt("it happened");
}

/*===========================================================================*/
/* Application code.                                                         */
/*===========================================================================*/



/*
 * Application entry point.
 */
int multi(void) {


  initTimers();

  /* Board-dependent GPIO setup code.*/
  portab_setup();

  palSetLineMode(LINE_VPILOT, PAL_MODE_INPUT_ANALOG);
  /*
   * Starting PORTAB_ADC1 driver and the temperature sensor.
   */
//  ADC1->CFGR |= ADC_CFGR_AWD1EN;
  adcStart(&PORTAB_ADC1, &portab_adccfg1);
  adcSTM32EnableVREF(&PORTAB_ADC1);
  adcSTM32EnableTS(&PORTAB_ADC1);
  //ADC1->IER |= ADC_IER_AWD1IE;

  ADC1->HTR1 = 10;
  ADC1->LTR1 = 8;
  /* Performing a one-shot conversion on two channels.*/
  adcConvert(&PORTAB_ADC1, &portab_adcgrpcfg1, samples1, ADC_GRP1_BUF_DEPTH);
  cacheBufferInvalidate(samples1, sizeof (samples1) / sizeof (adcsample_t));

  /*
   * Starting an ADC continuous conversion triggered with a period of
   * 1/10000 second.
   */
  adcStartConversion(&PORTAB_ADC1, &portab_adcgrpcfg2,
                     samples2multi, ADC_GRP2_BUF_DEPTH);

  /*
   * Normal main() thread activity, if the button is pressed then the
   * conversion is stopped.
   */
  while (true) {
    if (palReadLine(PORTAB_LINE_BUTTON) == PORTAB_BUTTON_PRESSED) {
      adcStopConversion(&PORTAB_ADC1);
    }
    cacheBufferInvalidate(samples2multi, sizeof (samples2multi) / sizeof (adcsample_t));
    chThdSleepMilliseconds(500);
  }
  return 0;
}
