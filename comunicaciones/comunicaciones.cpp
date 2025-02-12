/*
 * comunicaciones.c
 *
 *  Created on: 1/8/2017
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"

using namespace chibios_rt;
extern "C" {
    void startSMS(void);
}

#include "chprintf.h"
#include "string.h"
#include "comunicaciones.h"
#include "mqtt.h"
#include "manejoVars.h"
#include "colas.h"

mutex_t MtxTocandoComms;
uint8_t wifiOk;
extern uint8_t callReady, smsReady, estadoCREG, rssiGPRS;

extern char pendienteSMS[200];
extern uint8_t telefonoEnvio[16];
extern thread_t *procesoSMS;


extern varNOSI hayWifi;




void startWifi(uint8_t verbose)
{
    uint8_t error;
    mataWifi();
    if (hayWifi.getValorNum()==1)
    {
        error = initEsp8266((BaseChannel  *)&UARTD1);
        if (error) return;
        error = conectarEsp8266TCPMqqt((BaseChannel  *)&UARTD1);
        if (error) return;
        error = startMqtt((BaseChannel  *)&UARTD1, verbose);
        if (error) return;
        startEnvioMedidasMqtt();
    }
}



