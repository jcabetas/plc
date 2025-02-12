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
}
using namespace chibios_rt;

#include "dispositivos.h"

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
 * TIM1 CH1 (PE9) salida PWM Coche
 *      CH3 (sin salida) Trigger para regular ADC
 *      CH4 (sin salida) Trigger para Injected ADC
 */
static PWMConfig pwmcfg = {
        8000000,                                 /* 10kHz PWM clock frequency.   */
        8000,                                    /* Initial PWM 10% width       */
        NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL}
  },
  0,
  0,
  0
};



void testCargadorBasico(void)
{
    // PE9TIM1CH1 es el pin de entrada al ondulador
    // configuramos la salida
    // #define LINE_OSCILADOR              PAL_LINE(GPIOE, 9U)
    palSetLineMode(LINE_OSCILADOR, PAL_MODE_ALTERNATE(0));
    palSetLineMode(LINE_OSCILADOR, PAL_MODE_OUTPUT_PUSHPULL);
    while (true) {
        palClearLine(LINE_OSCILADOR);
        chThdSleepMilliseconds(1000);
        palSetLine(LINE_OSCILADOR);
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
    palSetLine(LINE_OSCILADOR);
    palSetLineMode(LINE_OSCILADOR, PAL_MODE_ALTERNATE(0));
    palSetLineMode(LINE_OSCILADOR, PAL_MODE_OUTPUT_PUSHPULL);
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
    palSetLineMode(LINE_OSCILADOR, PAL_MODE_ALTERNATE(1));
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
    // PE9TIM1CH1 es el pin de entrada al ondulador
    palSetLineMode(LINE_RELE1, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(LINE_RELE1);
    /*
      * Starting PWM driver 1 and enabling the notifications.
      * GPIOE9 is programmed as PWM output (channel 1 of TIM1).
      */
    palSetLineMode(LINE_OSCILADOR, PAL_MODE_ALTERNATE(1));
    pwmStart(&PWMD2, &pwmcfg);
    fijaAmperios(5.0f);
    ocultaOscilador();
    pwmEnableChannel(&PWMD2, 2,  300); // CH3 4% after start pulse (top side). Ojo, en parada se fija al 5%, necesita margen
    pwmEnableChannel(&PWMD2, 3, 6400); // CH4 80% after start pulse (low side)
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

//void testCargador(void)
//{
//    // PB12 es el rele (GPIOB_RELE, LINE_RELE)
//    // PE9TIM1CH1 es el pin de entrada al ondulador
//    palSetLineMode(LINE_RELE, PAL_MODE_OUTPUT_PUSHPULL);
//    palClearLine(LINE_RELE);
//    /*
//      * Starting PWM driver 1 and enabling the notifications.
//      * GPIOE9 is programmed as PWM output (channel 1 of TIM1).
//      */
//    palSetLineMode(LINE_OSCILADOR, PAL_MODE_ALTERNATE(1));
//    pwmStart(&PWMD1, &pwmcfg);
//    fijaAmperios(10.0f);
//    pwmEnableChannel(&PWMD1, 2,  400); // CH3 5% after start pulse (top side)
//    pwmEnableChannel(&PWMD1, 3, 6400); // CH4 80% after start pulse (low side)
//    initADC();
//    while (true)
//     {
////         PWMD1.tim->CNT = PWMD1.tim->ARR;
//   //  para ocultar oscilador
//   //  palSetLineMode(LINE_TIM1_CH1, PAL_MODE_ALTERNATE(0));
//   //  palSetLineMode(LINE_TIM1_CH1, PAL_MODE_OUTPUT_PUSHPULL);
//         chThdSleepMilliseconds(5000);
//     };
//
//}
