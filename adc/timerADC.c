/*
 * timerADC.c
 *
 *  Created on: 11 mar 2025
 *      Author: joaquin
 */
#include "ch.h"
#include "hal.h"


#define STM32_TIM3_SMCR_SMS(n)               ((((n) & 7) << 16) |  (((n) & 0b111) << 0))
#define STM32_TIM3_SMCR_TS(n)                ((((n) & 0b11000) << 20) |  (((n) & 0b111) << 4))

/*
 * TIM2 CH1 (PA15) salida PWM Coche
 *      CH2 (sin salida) Trigger para regular ADC1
 * TIM3 CH4 (sin salida) Trigger para Injected ADC1
 * TIM4 CH4 (sin salida) Trigger para ADC3
 */
static PWMConfig pwmcfgTIM2 = {
        8000000,                                 /* 8 MHz TIM2 clock frequency.   */
        8000,                                    /* 1 kHz (8Mhz/8000) PWM frequency */
        NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0,
  0
};

static PWMConfig pwmcfgTIM3 = {
        8000000,                                 /* 8 MHz TIM2 clock frequency.   */
        8000,                                    /* 1 kHz (8Mhz/8000) PWM frequency */
        NULL,
  {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0,
  0
};

static PWMConfig pwmcfgTIM4 = {                  /* Me vale con 10 Hz muestreo */
        10000,                                   /* 10kHz PWM clock frequency. */
        1000,                                    /* 10 Hz PWM                  */
        NULL,
  {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0,
  0
};


// Usamos sincronización de timers: TIM2 como maestro, y TIM3 como esclavo
// TIM2_CR2_MMS = 100: Compare - OC1REFC signal is used as trigger output (TRGO)
// TIM3_SMCR_TS = ITR1 (0b0001)  // Triger selection = TIM2
//          _SMS = 0111: External Clock Mode 1 - Rising edges of the selected trigger (TRGI) clock the counter.

void initTimersADC(void)
{
    palSetLineMode(LINE_TIM2CH1, PAL_MODE_ALTERNATE(1));

    // Si queremos ver salidas de timers auxiliares
    // - TIM2_CH2 esta en PB3
    // - TIM3_CH4 lo tenemos en PB1
    // palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_ALTERNATE(2));
    // palSetPadMode(GPIOB, GPIOB_SPIx_CLK, PAL_MODE_ALTERNATE(1));

    TIM3->SMCR = STM32_TIM3_SMCR_TS(0b0001) | STM32_TIM3_SMCR_SMS(0b0111);
    TIM3->CR2 = STM32_TIM_CR2_MMS(0b100);

    pwmStart(&PWMD2, &pwmcfgTIM2);
    pwmStart(&PWMD3, &pwmcfgTIM3);
    pwmStart(&PWMD4, &pwmcfgTIM4);
    pwmEnableChannel(&PWMD2, 0, 1300); // Onda para wallbox
    pwmEnableChannel(&PWMD2, 1,  300); // CH3 4% after start pulse (top side). Ojo, en parada se fija al 5%, necesita margen
    pwmEnableChannel(&PWMD3, 3, 6400); // CH4 80% after start pulse (low side)
    pwmEnableChannel(&PWMD4, 3,  100); // Onda para ADC3, cualquier factor vale, usamos 10% ancho
}
