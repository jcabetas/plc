/*
 * mqtt2status.cpp
 */


#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"
#include "string.h"

#include "bloques.h"
#include "parametros.h"
#include "nextion.h"
#include "stdlib.h"
#include "dispositivos.h"

//extern mutex_t MtxTxPlc;
void getMutex(void);
void releaseMutex(void);
/*
 *
 * MQTT2STATUS /villagolf/PresenciaSala hayGente
 *
 * mensaje recibido MQTT, topic: pelayos/estado1  msg: 1
   ==>  {"evento":"mqttMsg","idDisp":"2","info":"pelayos/estado1","valor":"1"}
 *
 */
mqtt2status::mqtt2status(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros MQTT2STATUS");
        *hayError = 1;
        return; // error
    }
    idNombre = nombres::incorpora(pars[1]);
    numEstado = estados::addEstado(tty, pars[2], 1, hayError);
    erroresSeguidos = 0;
    dsSinRx = 0;
    suscrito = 0;
    numDispMqtt = mqttRaspi::addDisp(this);
    if (numDispMqtt<0)
    {
        nextion::enviaLog(NULL,"No puedo incorporar dispositivo MQTT2STATUS");
        *hayError = 1;
    }
};


mqtt2status::~mqtt2status()
{
}


const char *mqtt2status::diTipo(void)
{
    return "mqtt2status";
}

uint16_t mqtt2status::diIdNombre(void)
{
    return idNombre;
}

//void mqtt2status::trataRx(const char *idReport, const char *valor)
// * mensaje recibido MQTT, topic: pelayos/estado1  msg: 1
//==>  {"evento":"mqttMsg","idDisp":"2","info":"pelayos/estado1","valor":"1"}
void mqtt2status::trataRx(const char *valor, const char *)
{
    if (valor[0] != 0)
    {
        estados::ponEstado(numEstado, atoi(valor));
    }
    dsSinRx = 0;
}

void mqtt2status::enviaSolicitudSusc(void)
{
    const char *topic = nombres::nomConId(idNombre);
    getMutex();
    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"mqttSuscribe\",\"idDisp\":\"%d\",\"topic\":\"%s\",\"info\":\"\"}\r",numDispMqtt,topic);
    releaseMutex();
}

void mqtt2status::suscribir(uint8_t *todoSuscrito)
{
    if (!suscrito)
    {
        *todoSuscrito = 0;
        enviaSolicitudSusc();
    }
}


void mqtt2status::actualizaEstSusc(uint8_t estadoSusc)
{
    suscrito = estadoSusc;
}

void mqtt2status::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t )
{
    // 30s sin Rx, pongo a cero la variable
    if (dsSinRx < 300)
        ++dsSinRx;
    if (dsSinRx >= 300)
       estados::ponEstado(numEstado, 0);
    return;
}


void mqtt2status::initMqtt(void)
{
    suscrito = 0;
}

int8_t mqtt2status::init(void)
{
    estados::ponEstado(numEstado, 0);
    return 0;
}



uint8_t mqtt2status::usaBus(void)
{
    return 0;
}

void mqtt2status::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"mqtt2status %s",
               nombres::nomConId(idNombre));
    nextion::enviaLog(tty, buffer);
}


