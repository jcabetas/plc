/*
 * emulateADC.c
 *
 *  Created on: 1/5/2016
 *      Author: joaquin
 */
//
//#include "hal.h"
//#include "chprintf.h"
//#include "calendar.h"
//#include "oscilador.h"
//#include "hal_adc_lld.h"
// ver ChibiStudio/chibios_stable-21.11.x/testhal/STM32/multi/ADC/cfg/stm32h743zi_nucleo144/portab.c

#include "ch.hpp"
#include "hal.h"
#include "gets.h"
#include "string.h"
#include "nextion.h"
extern "C" {
    void testCargador(void);
    void adcAWDCallback(void);
}
using namespace chibios_rt;

#include "dispositivos.h"

///* Algunas definiciones que faltan en Chibios */
#define ADC_JSQR_JSQ4_N(n)       ((n) << 15)  /**< @brief 1st channel in seq. */
#define ADC_JSQR_NUM_CH(n)      (((n) - 1) << 20)

#define STM32_ADC_NUMBER         18
#define STM32_ADC_IRQ_PRIORITY    6

// original
//uint16_t minSinCocheADC=219, maxSinCocheADC=600, minConectadoADC=676, maxConectadoADC=1016;
//uint16_t minPidecargaADC=1099, maxPidecargaADC=1513, minPideVentilacionADC=1534, maxPideVentilacionADC=1900;

// con bateria
//uint16_t minSinCocheADC=324, maxSinCocheADC=654, minConectadoADC=655, maxConectadoADC=1027;
//uint16_t minPidecargaADC=1028, maxPidecargaADC=1448, minPideVentilacionADC=1449, maxPideVentilacionADC=1875;

// con fuente roja
//uint16_t minSinCocheADC=242, maxSinCocheADC=592, minConectadoADC=593, maxConectadoADC=986;
//uint16_t minPidecargaADC=987, maxPidecargaADC=1427, minPideVentilacionADC=1428, maxPideVentilacionADC=1874;

// fuente roja y nuevo OPAMP
uint16_t minSinCocheADC=250, maxSinCocheADC=689, minConectadoADC=690, maxConectadoADC=1092;
uint16_t minPidecargaADC=1093, maxPidecargaADC=1542, minPideVentilacionADC=1543, maxPideVentilacionADC=1999;


event_source_t haCambiadoADC_source;

//void ocultaOscilador(void);
//void sacaOscilador(void);


/*
 * Con OPAMP a +3.3V (salida High, Pilot=+11.21V) => entrada ADC=0.254 V Lectura ADC teorica = 315,  real 303..314
 *              0.0V (salida Low , Pilot=-10.54V) => entrada ADC=2.946 V Lectura ADC teorica = 3655, real: 3640
 * Ahora:
 *   Vad = -12*12/80-Vcp*12/100 ; ADC = Vad*3655/2.956
 *   => Si Vcp = -12V => Vad = 3,24, ADC=4006
 *   => Si Vcp =  12V => Vad = 0,36, ADC= 445
 */

/*
 * Nueva estrategia: leer ADC en vacío (con salida positiva y negativa), para determinar VPos y Vneg
 * - Ecuación OPAMP:
 *     Vpilot/100+Vout/12+Vneg/82 = 0
 *     Vout = -12*(Vneg/82+Vpilot/100)
 *     ADC = Vout*4096/3.3
 * - En fase pos:                               [Vpos = 11.8, Vneg = -11.5V]
 *     Vpilot = VPos                            [11.8V]
 *     Vouta = -12*(Vneg/82+Vpos/100)           [0.266927, ADC=331]
 *     -3.3/4096*ADCa = 12/82*Vneg+12/100*Vpos  [-0.266927 = 0.14634*Vneg + 0.12*Vpos]
 * - En fase neg:
 *     VPilotb = VNeg                           [-11.5]
 *     Voutb = -12*(Vneg/82+Vneg/100)           [-3.06293, ADC=3802]
 *     -3.3/4096*ADCb = 12/82*Vneg+12/100*Vneg  [-3.06293 = 0,266341*Vneg] => Vneg=-11.5 => Vpos=11.8
 *
 *  Conociendo el valor de Vpos y Vneg, deducimos el valor de los ADC de cada estado
 *  - En vacio salida pos:
 *     Vpilot = Vpos [11.8]
 *     Vout = -12*(Vneg/82+Vpilot/100) = -12*(Vneg/82+Vpos/100) [0.266927]
 *     ADC = 331
 *  - En vacio salida neg:
 *     VPilot = Vneg [-11.5]
 *     Vout = -12*(Vneg/82+Vneg/100) = -12*Vneg*(1/82+1/100) [3.06293]
 *     ADC = 3802
 *  - Coche conectado (2k74)
 *     VPilot = Vpos*2,74/3,74+0,6          [9.24492]
 *     Vout = -12*(Vneg/82+VPilot/100)      [0.573536]
 *     ADC = 711
 *  - Coche pidiendo (0k882)
 *     VPilot = Vpos*0.882/1.882+0.6        [6.13596]
 *     Vout = -12*(Vneg/82+Vpilot/100)      [0.946612]
 *     ADC = 1175
 *  - Coche pidiendo con ventilacion (0k246)
 *     VPilot = Vpos*0.246/1.246+0.6        [2.9297]
 *     Vout = -12*(Vneg/82+Vpilot/100)      [1.3314]
 *     ADC = 1652
 */

//typedef enum { RDESCONOCIDO=0, RDESCONECTADO, RCONECTADO, RPIDECARGA, RPIDEVENTILACION} estadoRes_t;
//uint16_t minSinCocheADC=250, maxSinCocheADC=689, minConectadoADC=690, maxConectadoADC=1092;
//uint16_t minPidecargaADC=1093, maxPidecargaADC=1542, minPideVentilacionADC=1543, maxPideVentilacionADC=1999;
static const char *estadoResStr[5] = {"??","Desconectado","Conectado","Pide","Pide y Vent"};

void cargador::iniciaADC(void)
{
    // reseteo datos ADC
    cargadorPtr->numDatHist = 0;
    nextion::enviaLog(NULL,"Eliminados datos ADC");
}

void cargador::enviaHistADC(void)
{
    // envio datos historicos de cambios a Nextion
    char buffer[80];
    nextion::enviaLog(NULL,"Cambios en ADC:");
    for (uint8_t i=0;i<cargadorPtr->numDatHist;i++)
    {
        // -12/82+Vpilot/100+Vadc/12 = 0
        // Vpilot = 100*(12/82-Vadc/12) = 100*(12/82-3.3/4096*ADC/12) = 14.6341f-0.006713867f*ADC
        float Vpilot = 14.6341f-0.006713867f*cargadorPtr->datADCHist[i];
        chsnprintf(buffer,sizeof(buffer),"%s (ADC:%d, Vpilot:%.2f)",estadoResStr[cargadorPtr->estadosResHist[i]],cargadorPtr->datADCHist[i],Vpilot);
        nextion::enviaLog(NULL,buffer);
    }
    chsnprintf(buffer,sizeof(buffer),"pide[%d-%d], pideVent[%d,%d]", minPidecargaADC, maxPidecargaADC, minPideVentilacionADC, maxPideVentilacionADC);
    nextion::enviaLog(NULL,buffer);
    chsnprintf(buffer,sizeof(buffer),"sinCoche[%d-%d], Conect.[%d-%d]", minSinCocheADC, maxSinCocheADC, minConectadoADC, maxConectadoADC);
    nextion::enviaLog(NULL,buffer);
}

void cargador::estimaValoresADC(void)
{
    uint32_t valorActual, valorActualJ;
    uint32_t sumaPos, sumaNeg;
    uint16_t maxPos, maxNeg, minPos, minNeg;
    char buffer[80];
    if (cargadorPtr==NULL)
    {
        nextion::enviaLog(NULL,"No hay definido cargador!");
        return;
    }
    nextion::enviaLog(NULL,"Desconecta cargador del coche en 10s");
    chThdSleepMilliseconds(10000);
    // desactivo interrupciones ventana ADC
    ADC1->HTR1 = 0xFFF;
    ADC1->LTR1 = 0;
    sacaOscilador();
    cargadorPtr->fijaAmperios(32.0f);

    // tomo 16 medidas
    sumaPos = 0;
    maxPos = 0;
    minPos = 0xFFFF;
    sumaNeg = 0;
    maxNeg = 0;
    minNeg = 0xFFFF;
    for (uint n=0;n<16;n++)
    {
        // leo ADC
        valorActual = ADC1->DR;
        sumaPos += valorActual;
        if (valorActual>maxPos)
            maxPos = valorActual;
        if (valorActual<minPos)
            minPos = valorActual;
        valorActualJ = ADC1->JDR1;
        sumaNeg += valorActualJ;
        if (valorActualJ>maxNeg)
            maxNeg = valorActualJ;
        if (valorActualJ<minNeg)
            minNeg = valorActualJ;
        chThdSleepMilliseconds(10);
    }
    uint32_t mediaPos = (sumaPos>>4);
    uint32_t mediaNeg = (sumaNeg>>4);
    chsnprintf(buffer,sizeof(buffer),"PosADC = [%d..%d..%d]",minPos,mediaPos,maxPos);
    nextion::enviaLog(NULL,buffer);
    chsnprintf(buffer,sizeof(buffer),"NegADC = [%d..%d..%d]",minNeg,mediaNeg,maxNeg);
    nextion::enviaLog(NULL,buffer);
    /*  - En vacio salida neg:
    *     VPilot = Vneg [-11.5]
    *     Vout = -12*(Vneg/82+Vneg/100) = -12*Vneg*(1/82+1/100) [3.06293]
    *     ADC = 3802
    */
    float Vneg = ((float) mediaNeg)*(-0.00302492917f); // 1/4096*3.3/12/(1/82+1/100);
    /*
     * En vacio salida pos:
     * Vpilot = Vpos [11.8]
     * Vout = -12*(Vneg/82+Vpilot/100) = -12*(Vneg/82+Vpos/100) [0.266927]
     * Vpos = 100*(-Vout/12-Vneg/82)
     * ADC = 331
     */
    float Vpos = -0.00671386719f*mediaPos - 1.21951219512f*Vneg; //-mediaPos/4096*3.3/12*100 - Vneg/82*100;
    /*
     *  Coche conectado (2k74)
     *     VPilot = Vpos*2,74/3,74+0,6          [9.24492]
     *     Vout = -12*(Vneg/82+VPilot/100)      [0.573536]
     *     ADC = 711
     */
//    float Vpilot = 0.73262032f*Vpos+0.6f;
//    float Vout = -12*(Vneg/82.0f+0.73262032f*Vpos/100.0f+0.6/100);
//    uint16_t ADCc = (uint16_t) (4096.0f/3.3f*Vout);
    uint16_t ADCconec = (uint16_t) (-181.6408f*Vneg - 109.1204666f*Vpos - 89.367f);
    /*
     * - Coche pidiendo (0k882)
     *     VPilot = Vpos*0.882/1.882+0.6        [6.13596]
     *     Vout = -12*(Vneg/82+Vpilot/100)      [0.946612]
     *     ADC = 1175
     */
//    Vpilot = 0.468650372f*Vpos + 0.6f;
//    Vout = -12*(Vneg/82+0.468650372f*Vpos/100+0.6/100);
//    ADCc = (uint16_t) 4096.0f/3.3*Vout;
    uint16_t ADCpide = (uint16_t) (-181.6408f*Vneg - 69.80334268f*Vpos - 89.367f);
    /*
     *  Coche pidiendo con ventilacion (0k246)
     *     VPilot = Vpos*0.246/1.246+0.6        [2.9297]
     *     Vout = -12*(Vneg/82+Vpilot/100)      [1.3314]
     */
//    Vpilot = 0.1974317817f*Vpos + 0.6f;
//    Vout = -12*(Vneg/82+0.1974317817f*Vpos/100+0.6/100);
//    ADCc = (uint16_t) (4096.0f/3.3f*Vout);
    uint16_t ADCvent = (uint16_t) (-181.6408f*Vneg - 29.406566467f*Vpos - 89.367f);
    //
    chsnprintf(buffer,sizeof(buffer),"ADC nc:%d con:%d pide:%d vent:%d neg:%d",mediaPos,ADCconec,ADCpide,ADCvent, mediaNeg);
    nextion::enviaLog(NULL,buffer);
    // con bateria a 11.8V
    //uint16_t minSinCocheADC=324, maxSinCocheADC=654, minConectadoADC=655, maxConectadoADC=1027;
    //uint16_t minPidecargaADC=1028, maxPidecargaADC=1448, minPideVentilacionADC=1449, maxPideVentilacionADC=1875;

    // con fuente roja
    //uint16_t minSinCocheADC=250, maxSinCocheADC=689, minConectadoADC=690, maxConectadoADC=1092;
    //uint16_t minPidecargaADC=1093, maxPidecargaADC=1542, minPideVentilacionADC=1543, maxPideVentilacionADC=1999;
    maxSinCocheADC = (mediaPos+ADCconec)>>1;
    minSinCocheADC = mediaPos - (maxSinCocheADC-mediaPos);
    minConectadoADC = maxSinCocheADC+1;
    maxConectadoADC = (ADCconec+ADCpide)>>1;
    minPidecargaADC = maxConectadoADC+1;
    maxPidecargaADC = (ADCpide+ADCvent)>>1;
    minPideVentilacionADC = maxPidecargaADC+1;
    maxPideVentilacionADC = minPideVentilacionADC+2*(ADCvent-minPideVentilacionADC);
    // restauro ADC
    cargadorPtr->initADC();
    cargadorPtr->initCargador();
}


estadoRes_t calculaEstadoRes(int32_t valorADC)
{
    int32_t minWD, maxWD;
    estadoRes_t statusResis = RDESCONOCIDO;
    if (valorADC>=minSinCocheADC && valorADC<=maxSinCocheADC)
    {
        statusResis = RDESCONECTADO;
        ADC1->HTR1 = maxSinCocheADC;
        ADC1->LTR1 = minSinCocheADC;
    }
    else if (valorADC>=minConectadoADC && valorADC<=maxConectadoADC)
    {
        statusResis = RCONECTADO;
        ADC1->HTR1 = maxConectadoADC;
        ADC1->LTR1 = minConectadoADC;
    }
    else if (valorADC>=minPidecargaADC && valorADC<=maxPidecargaADC)
    {
        statusResis = RPIDECARGA;
        ADC1->HTR1 = maxPidecargaADC;
        ADC1->LTR1 = minPidecargaADC;
    }
    else if (valorADC>=minPideVentilacionADC && valorADC<=maxPideVentilacionADC)
    {
        statusResis = RPIDEVENTILACION;
        ADC1->HTR1 = maxPideVentilacionADC;
        ADC1->LTR1 = minPideVentilacionADC;
    }
    if (statusResis == RDESCONOCIDO) // valor raro
    {
        minWD = valorADC - 50;
        if (minWD<0) minWD = 0;
        maxWD = valorADC + 50;
        if (maxWD>0xFFF) maxWD = 0xFFF;
        // escribo limites en ADC
        ADC1->HTR1 = maxWD;//0xFFF;
        ADC1->LTR1 = minWD;
    }
    cargador::setEstadoRes((valorADC & 0xFFFF),statusResis);
    return statusResis;
}

void adcAWDCallback(void)
{
    uint32_t valorActual;
    valorActual = ADC1->DR;
    // esto podría ir a un thread especifico
    calculaEstadoRes(valorActual);
//    estadoRes_t statusResis = calculaEstadoRes(valorActual);

//    if (statusResis==RCONECTADO || statusResis==RPIDECARGA || statusResis==RPIDEVENTILACION)
//        cargador::sacaOscilador();
//    else
//        cargador::ocultaOscilador();
//    chSysLockFromISR();
//    if (chEvtIsListeningI(&haCambiadoADC_source))
//    {
//        chEvtBroadcastI(&haCambiadoADC_source);
//    }
//    chSysUnlockFromISR();
    //ADC1->SR = 0;
}

/* hay que modificar os/hal/ports/STM32/LLD/ADCv2/hal_adc_lld.c
  y poner
    if (sr & ADC_SR_AWD) {
    if (ADCD1.grpp != NULL) {
      _adc_isr_error_code(&ADCD1, ADC_ERR_WATCHDOG);
    }
    // new
    adcAWDCallback();
*/
// En Chibios V21 hay un HOOKUP que puede valer: STM32_ADC_ADC2_IRQ_HOOK, habria que poner:
//     if (sr & ADC_SR_AWD) adcAWDCallback();


void cargador::initADC(void)
{
//    // input en PB0ADC12IN8 ADC12_IN8
//    palSetLineMode(GPIOC_VPILOT, PAL_MODE_INPUT_ANALOG);
//
//    rccEnableADC12(FALSE);
//    rccEnableDAC1(false);
//    ADC1->CR = 0;
//    ADC1->CR = ADC_CR_ADEN;
//    // TIM1-CH3  (sin salida) Trigger para regular ADC
//    // TIM1-CH4  (sin salida) Trigger para Injected ADC
//    // ADC1->CR1   = ADC_CR1_SCAN | ADC_CR1_AWDEN | ADC_CR1_AWDIE;
//    ADC1->IER |= ADC_IER_AWD1IE;
//    ADC1->CFGR |= ADC_CFGR_JAWD1EN;
//    ADC1->CR |= ADC_CR_JADSTART;    // empezamos a convertir injected channels
//
//
//    ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_EXTEN_FALLING  | ADC_CR2_EXTSEL_SRC(0b0010) | (0b10<<20) | (0b000<<16); //ADC_CR2_JEXTEN_FALLING + JEXTSEL(TIM1CH4)
//    ADC1->SMPR1 = 0;//ADC_SMPR1_SMP_AN10(ADC_SAMPLE_144);
//    ADC1->SMPR2 = ADC_SMPR2_SMP_AN8(ADC_SAMPLE_480);
//    ADC1->SQR1  = ADC_SQR1_NUM_CH(1);
//    ADC1->SQR2  = 0;
//    ADC1->SQR3  = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN8);
//    ADC1->JSQR = ADC_JSQR_NUM_CH(1) | ADC_JSQR_JSQ4_N(ADC_CHANNEL_IN8);
//    // Activo Watchdog en todos los canales regulares
//    ADC1->HTR1 = 5; // para que se active enseguida, pongo un rango estrecho
//    ADC1->LTR1 = 3;
//    nvicEnableVector(STM32_ADC_NUMBER, STM32_ADC_IRQ_PRIORITY);
//   // estimaValoresADC();

}
