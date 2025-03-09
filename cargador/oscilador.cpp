/*
 * cargador.cpp
 *
 *  Created on: 17 jun. 2021
 *      Author: joaquin
 */


#include "ch.hpp"
#include "hal.h"
extern "C" {
    void testCargador(void);
    void initTimers(void);
}
using namespace chibios_rt;

#include "dispositivos.h"

#define STM32_TIM3_SMCR_SMS(n)               ((((n) & 7) << 16) |  (((n) & 0b111) << 0))
#define STM32_TIM3_SMCR_TS(n)                ((((n) & 0b11000) << 20) |  (((n) & 0b111) << 4))


void initADC(void);

//
//uint8_t osciladorOculto = 0;

//void initADC(void);

/*
 * Duty cycle = Amps / 0.6
 * 100% = 8000
 * Ciclos = A*600
 *
 * Hasta 51A
 */

/*
 * TIM2 CH1 (PA15) salida PWM Coche
 *      CH2 (sin salida) Trigger para regular ADC
 * TIM3 CH4 (sin salida) Trigger para Injected ADC
 */
static PWMConfig pwmcfgTIM2 = {
        8000000,                                 /* 10kHz PWM clock frequency.   */
        8000,                                    /* Initial PWM 10% width       */
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
        8000000,                                 /* 10kHz PWM clock frequency.   */
        8000,                                    /* Initial PWM 10% width       */
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

void testCargadorBasico(void)
{
    // PA15-TIM2CH1 (LINE_TIM2CH1) es el pin de entrada al ondulador
    // configuramos la salida
    palSetLineMode(LINE_TIM2CH1, PAL_MODE_ALTERNATE(0));
    palSetLineMode(LINE_TIM2CH1, PAL_MODE_OUTPUT_PUSHPULL);
    while (true) {
        palClearLine(LINE_TIM2CH1);
        chThdSleepMilliseconds(1000);
        palSetLine(LINE_TIM2CH1);
        chThdSleepMilliseconds(1000);
    }
}

void cargador::fijaAmperios(float amperios)
{
    uint16_t ancho;
    // si hay que bajar mucho, cuidado
    // (% de ciclo de trabajo) = corriente[A]/0,6
    // esta configurado de forma que ancho=8000 => 100%
    // 30A == 50% => ancho=4000
    float Imin = mAmin->valor()/1000.0f;
    if (amperios<Imin)
        amperios = Imin;
    ancho = (uint16_t) (amperios*133.0f);
    pwmEnableChannel(&PWMD2, 0, ancho); // Onda para wallbox
//    pwmEnableChannel(&PWMD1, 2, ancho-200); // detección tensión +12V
//    pwmEnableChannel(&PWMD1, 3, ancho+3000); // detección tensión -12 V
}

void cargador::ocultaOscilador(void)
{
    // desconecto oscilador
    palSetLine(LINE_TIM2CH1);
    palSetLineMode(LINE_TIM2CH1, PAL_MODE_ALTERNATE(0));
    palSetLineMode(LINE_TIM2CH1, PAL_MODE_OUTPUT_PUSHPULL);
//
//  palSetPad(GPIOA, 8);
//  palClearPad(GPIOA,7);
//  palSetPadMode(IOPORT1,8, PAL_MODE_OUTPUT_PUSHPULL);
//    palSetPadMode(IOPORT1,7, PAL_MODE_OUTPUT_PUSHPULL);
    cargadorPtr->osciladorOculto = 1;
}

void cargador::sacaOscilador(void)
{
    // PA8 => TIM1_CH1
    // PA7 => TIM1_CH1N (D32 en extension-pin10)
    //PWMD1.tim->CNT = 1L;
    PWMD2.tim->CNT = PWMD2.tim->ARR;
//  palClearPad(GPIOA, 8);
//  palSetPad(GPIOA,7);
    palSetLineMode(LINE_TIM2CH1, PAL_MODE_ALTERNATE(1));
//  palSetPadMode(IOPORT1,8, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
//    palSetPadMode(IOPORT1,7, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
    cargadorPtr->osciladorOculto = 0;
    cargadorPtr->ondaNegOk = 1; // se actualizará mas tarde
}

void cargador::initCargador(void)
{
    cargador::init();
    sacaOscilador();
}

int8_t cargador::init(void)
{
    // PB12 es el rele (GPIOB_RELE, LINE_RELE)
    // PA15-TIM2CH1 LINE_TIM2CH1 es el pin de entrada al ondulador
    // Usamos TIM2CH2 para arrancar regular AD1 y TIM3CH4 para injected
    initADC();
    palSetLineMode(LINE_RELE1, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(LINE_RELE1);
    /*
      * Starting PWM driver 1 and enabling the notifications.
      * LINE_TIM2CH1 is programmed as PWM output (channel 1 of TIM2).
      */
    palSetLineMode(LINE_TIM2CH1, PAL_MODE_ALTERNATE(1));
    pwmStart(&PWMD2, &pwmcfgTIM2);
    fijaAmperios(5.0f);
    ocultaOscilador();
    pwmEnableChannel(&PWMD2, 1,  300); // CH3 4% after start pulse (top side). Ojo, en parada se fija al 5%, necesita margen
    pwmEnableChannel(&PWMD3, 3, 6400); // CH4 80% after start pulse (low side)
    initADC();
    if (pContratadaValle==NULL)
    {
        nextion::enviaLog(NULL, "Falta bloque CARG-AJUSTES");
        return 1;
    }
    if (calendar::getPeriodoTarifa()==valle)
        pContratada = pContratadaValle->valor();
    else
        pContratada = pContratadaPunta->valor();
    kWhIniCoche = medkWhCoche->diValor();
    statusResisOld = RDESCONOCIDO;
    kWhIniCoche = -999.0f;
    numFases[0] = 0;
    enviaTxt(cargadorPtr->idPagecoches, "fases1", estimaFases());
    aplicaTipoControl();
    return 0;
}

void initTimers(void)
{
    // PB12 es el rele (GPIOB_RELE, LINE_RELE)
    // PE9TIM1CH1 es el pin de entrada al ondulador

//    palSetLineMode(LINE_TIM2CH1, PAL_MODE_ALTERNATE(1));
//    palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_ALTERNATE(2));

    // probamos sincronización de timers: TIM2 como maestro, y TIM3 como esclavo
    // TIM3_SMCR_TS = ITR1 (0b0001)  // Triger selection = TIM2
    //          _SMS = 0111: External Clock Mode 1 - Rising edges of the selected trigger (TRGI) clock the counter.
    //          _ MS = 1: The effect of an event on the trigger input (TRGI) is delayed to allow a perfect
    //                   synchronization between the current timer and its slaves (through TRGO). It is useful if we
    //                   want to synchronize several timers on a single external event.
    // TIM2_CR2_MMS = 100: Compare - OC1REFC signal is used as trigger output (TRGO)

    // TIM3_CH4 lo tenemos en PB1
    TIM3->SMCR = STM32_TIM3_SMCR_TS(0b0001) | STM32_TIM3_SMCR_SMS(0b0111) | STM32_TIM_SMCR_MSM;
    TIM3->CR2 = STM32_TIM_CR2_MMS(0b100);

    pwmStart(&PWMD2, &pwmcfgTIM2);
    pwmStart(&PWMD3, &pwmcfgTIM3);
    pwmEnableChannel(&PWMD2, 0, 1300); // Onda para wallbox
    pwmEnableChannel(&PWMD2, 1,  300); // CH3 4% after start pulse (top side). Ojo, en parada se fija al 5%, necesita margen
    pwmEnableChannel(&PWMD3, 3, 6400); // CH4 80% after start pulse (low side)
}

void testCargador(void)
{
    // PB12 es el rele (GPIOB_RELE, LINE_RELE)
    // PE9TIM1CH1 es el pin de entrada al ondulador
    // Pruebo rele
    palSetLineMode(LINE_RELE1, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(LINE_RELE1);
    chThdSleepMilliseconds(2000);
    palSetLine(LINE_RELE1);
    chThdSleepMilliseconds(2000);
    palClearLine(LINE_RELE1);


    palSetLineMode(LINE_TIM2CH1, PAL_MODE_ALTERNATE(1));
    palSetPadMode(GPIOB, GPIOB_PIN1, PAL_MODE_ALTERNATE(2));

    // probamos sincronización de timers: TIM2 como maestro, y TIM3 como esclavo
    // TIM3_SMCR_TS = ITR1 (0b0001)  // Triger selection = TIM2
    //          _SMS = 0111: External Clock Mode 1 - Rising edges of the selected trigger (TRGI) clock the counter.
    //          _ MS = 1: The effect of an event on the trigger input (TRGI) is delayed to allow a perfect
    //                   synchronization between the current timer and its slaves (through TRGO). It is useful if we
    //                   want to synchronize several timers on a single external event.
    // TIM2_CR2_MMS = 100: Compare - OC1REFC signal is used as trigger output (TRGO)

    // TIM3_CH4 lo tenemos en PB1
    TIM3->SMCR = STM32_TIM3_SMCR_TS(0b0001) | STM32_TIM3_SMCR_SMS(0b0111) | STM32_TIM_SMCR_MSM;
    TIM3->CR2 = STM32_TIM_CR2_MMS(0b100);

    pwmStart(&PWMD2, &pwmcfgTIM2);
    pwmStart(&PWMD3, &pwmcfgTIM3);
    pwmEnableChannel(&PWMD2, 0, 1300); // Onda para wallbox
    pwmEnableChannel(&PWMD2, 1,  300); // CH3 4% after start pulse (top side). Ojo, en parada se fija al 5%, necesita margen
    pwmEnableChannel(&PWMD3, 3, 6400); // CH4 80% after start pulse (low side)

    initADC();

    while (true)
     {
         chThdSleepMilliseconds(1000);
     };

}
