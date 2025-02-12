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

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "bloques.h"
#include "parametros.h"
#include "nextion.h"
#include "sms.h"
#include "stdlib.h"
#include "dispositivos.h"
#include "calendar.h"


#define ADC_GRP_NUM_CHANNELS   1
#define ADC_GRP_BUF_DEPTH      2


static adcsample_t samples[2];//ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

medida *vbat::medVBat = NULL;

/*
 * ADC streaming callback.
 */
//static void adccallback(ADCDriver *adcp)
//{
//  if (adcIsBufferComplete(adcp))
//  {
//      vbat::asignaVBat(samples[0]);
//  }
//}


/*
 * ADC conversion group.
 * Mode:        Continuous, 2 samples of 4 channels, SW triggered.
 * Channels:    IN2, IN3, IN4, IN5
 */
static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  ADC_GRP_NUM_CHANNELS,
  NULL,
  NULL,
  0,                        /* CR1 */
  ADC_CR2_SWSTART,          /* CR2 */
  ADC_SMPR1_SMP_VBAT(ADC_SAMPLE_144),  /* SMPR1 */
  0,                        /* SMPR2 */
  0,                        /* HTR */
  0,                        /* LTR */
  0,                        /* SQR1 */
  0,                        /* SQR2 */
  ADC_SQR3_SQ1_N(ADC_CHANNEL_VBAT)
};


///*
// * Thread to read A/D
// */
//static THD_WORKING_AREA(waAD, 518);
//static THD_FUNCTION(ThreadAD, arg) {
//  (void)arg;
//  event_listener_t arrancaADC_listener, finMedidaADC_listener;
//  eventmask_t evt;
//  chRegSetThreadName("A/D");
//  /*
//   * Starts an ADC continuous conversion.
//   */
//  chEvtRegisterMask(&arrancaADC_source, &arrancaADC_listener,EVENT_MASK(0));
//  chEvtRegisterMask(&finMedidaADC_source, &finMedidaADC_listener,EVENT_MASK(1));
//  do
//  {
//      evt = chEvtWaitAny(ALL_EVENTS);
//      if (evt & EVENT_MASK(0)) {  /* Piden conversion */
//          adcStartConversion(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);
//      }
//      if (evt & EVENT_MASK(1)) {  /* Hay que recalcular */
//          preparaTrama();
//      }
//    } while (TRUE);
//}

/*
* VBAT medVBat.plc
*/
vbat::vbat(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
   if (numPar!=2)
   {
       nextion::enviaLog(tty,"#parametros VBAT");
       *hayError = 1;
       return; // error
   }
   medVBat = new medida(tty, pars[1], 2, 600, hayError);
};

vbat::~vbat()
{

}

void vbat::asignaVBat(uint16_t valorADCBat)
{
    if (medVBat!=NULL)
    {
        float vbateria = valorADCBat*0.0016f; //3.3f/2048.0f;
        medVBat->set(&vbateria);
    }
    else
        medVBat->setValidez(0);
}

/*
 * A/D init
 */
int8_t vbat::init(void)
{
    adcStart(&ADCD1, NULL);
    adcSTM32EnableVBATE();
    adcStartConversion(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);
    asignaVBat(samples[0]);
    return 0;
}


// mide cada minuto
void vbat::addTime(uint16_t , uint8_t , uint8_t , uint8_t seg, uint8_t ds)
{
    if (seg==0 && ds==0)
    {
        adcStartConversion(&ADCD1, &adcgrpcfg, samples, ADC_GRP_BUF_DEPTH);
        asignaVBat(samples[0]);
    }
}

void vbat::print(BaseSequentialStream *tty)
{
    char buffer[80];
    if (medVBat==NULL)
        return;
    chsnprintf(buffer,sizeof(buffer),"VBAT en %s",medVBat->diNombre());
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}


const char *vbat::diTipo(void)
{
    return "vbat";
}

const char *vbat::diNombre(void)
{
    if (medVBat==NULL)
        return "VBAT";
    else
        return medVBat->diNombre();
}

