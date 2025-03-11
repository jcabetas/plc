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


void initTimersADC(void);
int initADC3(void);

#define ADC_GRP1_BUF_DEPTH      4
#define ADC_GRP1_NUM_CHANNELS   1

#if CACHE_LINE_SIZE > 0
CC_ALIGN_DATA(CACHE_LINE_SIZE)
#endif
adcsample_t samples[CACHE_SIZE_ALIGN(adcsample_t,
                                     ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH)];

#define ADC_GRP3_BUF_DEPTH      4
#define ADC_GRP3_NUM_CHANNELS   2
extern adcsample_t samples3[CACHE_SIZE_ALIGN(adcsample_t,
                                    ADC_GRP3_NUM_CHANNELS * ADC_GRP3_BUF_DEPTH)];

uint32_t lastAWDvalue = 0U;

/*
 * ADC irq callback
 */
void adcawd1callback(void) {
    lastAWDvalue = ADC1->DR;
    ADC1->HTR1 = (ADC1->DR + 100) << 4;
    ADC1->LTR1 = (ADC1->DR - 100) << 4;
    palToggleLine(LINE_RELE1);
}

const ADCConfig portab_adccfg1 = {.difsel = 0U, .calibration = 0U};

/*
 * ADC conversion group.
 * Mode:        Continuous, 1 channels, HW triggered by GPT4-TRGO.
 * Channels:    IN4 (Vpilot).
 */
const ADCConversionGroup portab_adcgrpcfg1 = {
      .circular = true,
      .num_channels = ADC_GRP1_NUM_CHANNELS,
      .end_cb = NULL, //adccallback,
      .error_cb = NULL, //adcerrorcallback,
      .cfgr = ADC_CFGR_RES_12BITS | ADC_CFGR_EXTSEL_SRC(0b11)
      | ADC_CFGR_EXTEN_FALLING | ADC_CFGR_AWD1_N(4) | ADC_CFGR_AWD1EN, //| ADC_CFGR_AWD1SGL, //| ADC_CFGR_CONT_ENABLED |
      // TIM2_OC2 en vez de 12, que es TIM4_TRGO
      .cfgr2 = 0U,
      .ccr = 0U, .pcsel = ADC_SELMASK_IN4 | ADC_SELMASK_IN11,
      .ltr1 = 15000U, .htr1 = 25000U, .ltr2 = 0x00000000U,
      .htr2 = 0x03FFFFFFU, .ltr3 = 0x00000000U, .htr3 = 0x03FFFFFFU,
      .smpr = {
              ADC_SMPR1_SMP_AN4(ADC_SMPR_SMP_384P5), 0U
      },
      .sqr = {
              ADC_SQR1_SQ1_N(ADC_CHANNEL_IN4), 0U, 0U, 0U
      }
};

int initADC1(void) {
    palSetLineMode(LINE_VPILOT, PAL_MODE_INPUT_ANALOG);
    adcStart(&ADCD1, &portab_adccfg1);
    adcSTM32EnableVREF(&ADCD1);
    adcSTM32EnableTS(&ADCD1);

    adcStartConversion(&ADCD1, &portab_adcgrpcfg1, samples, ADC_GRP1_BUF_DEPTH);
    // Arranco injected conversions
    ADC1->JSQR |= ADC_CHANNEL_IN4 << ADC_JSQR_JSQ1_Pos | 1 << ADC_JSQR_JL_Pos
            | 0b00100 << ADC_JSQR_JEXTSEL_Pos | 0b10 << ADC_JSQR_JEXTEN_Pos; // TIM3_oc4, rising, 1 canal
    ADC1->CR |= ADC_CR_JADSTART;
    return 0;
}

void initTimersADCs(void)
{
    initTimersADC();
    initADC1();
    initADC3();
}

void testADC(void)
{
    initTimersADCs();
    while (true) {
        cacheBufferInvalidate(samples, sizeof(samples) / sizeof(adcsample_t));
        cacheBufferInvalidate(samples3, sizeof (samples3) / sizeof (adcsample_t));
        chThdSleepMilliseconds(500);
    }
}
