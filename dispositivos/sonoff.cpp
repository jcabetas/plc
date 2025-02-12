/*
 * sonoff.cpp
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
 * MEDIDA PCocina"P Cocina" 20 0
 * MEDIDA EnergCocina"kWh Cocina" 50 1
 *
 * SONOFF 3E5176 PCocina EnergCocina
 *           nombre
 * MQTT 192.168.8.30 1883 joaquin JeW31ZT9Rdx
 * PINOUT 3E5176 calefOut

 */
sonoff::sonoff(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=4)
    {
        nextion::enviaLog(tty,"#parametros SONOFF");
        *hayError = 1;
        return; // error
    }
    idNombre = nombres::incorpora(pars[1]);
    medidaP = medida::findMedida(pars[2]);
    if (medidaP==NULL)
    {
        nextion::enviaLog(NULL,"No encuentro medP en SONOFF");
        *hayError = 1;
    }
    medidaEner = medida::findMedida(pars[3]);
    if (medidaEner==NULL)
    {
        nextion::enviaLog(NULL,"No encuentro medEner en SONOFF");
        *hayError = 1;
    }
    estadoSolicitado = 0;
    estadoReportado = 0;
    erroresSeguidos = 0;
    dsOutDiferente = 0;
    pinSalida = NULL;
    suscrito = 0;
    numDispMqtt = mqttRaspi::addDisp(this);
    if (numDispMqtt<0)
    {
        nextion::enviaLog(NULL,"No puedo incorporar dispositivo SONOFF");
        *hayError = 1;
    }
};


sonoff::~sonoff()
{
}

uint8_t sonoff::attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char **, uint8_t *hayError)
{
    /*
     *  parametros: ninguno
     */
    if (numPar!=0)
    {
        nextion::enviaLog(NULL,"#pinout en SONOFF no tiene parametros!");
        *hayError = 1;
        return 1;
    }
    // compruebo que no estaba definida
    if (pinSalida!=NULL)
    {
        nextion::enviaLog(NULL,"#pinOut en SONOFFF ya definido");
        *hayError = 1;
        return 1;
    }
    pinSalida = outPin;
    return 0;
}

const char *sonoff::diTipo(void)
{
    return "sonoff";
}

uint16_t sonoff::diIdNombre(void)
{
    return idNombre;
}

void sonoff::trataRx(const char *valor, const char *info)
{
    if (!strcasecmp(info,"SW")) // puede ser ON o OFF
    {
        if (!strcasecmp(valor,"OFF"))
            estadoReportado = 0;
        if (!strcasecmp(valor,"ON"))
            estadoReportado = 1;
    }
    if (!strcasecmp(info,"Power")) // puede ser ON o OFF
    {
        if (!strcasecmp(valor,"OFF"))
            estadoReportado = 0;
        if (!strcasecmp(valor,"ON"))
            estadoReportado = 1;
    }
    if (!strcasecmp(info,"ENERGY.Power")) // Potencia en W
    {
        float P = atof(valor);
        medidaP->set(&P, 1);
    }
    if (!strcasecmp(info,"ENERGY.TOTAL")) // Energia en Wh
    {
        float Ener = atof(valor);
        medidaEner->set(&Ener, 1);
    }
}

void sonoff::enviaSolicitudSusc(void)
{
    // enviando -t "cmnd/3E5176/POWER" -m "", devuelve estado del interruptor en stat/3E5176/RESULT con info "POWER".
    // Idem -t "cmnd/3E5176/Status" -m "10", devuelve el estado de los sensores
    // PLC=>RASPI {"evento":"mqttSuscribe","idDisp":"3E5176","reports":
    //                 [{"idReport":"SW","topic":"stat/3E5176/RESULT","info":"POWER"},
    //                  {"idReport":"SW","topic":"tele/3E5176/STATE","info":"POWER"},
    //                  {"idReport":"P","topic":"tele/3E5176/SENSOR","info":"ENERGY.Power"},
    //                  {"idReport":"Wh","topic":"tele/3E5176/SENSOR","info":"ENERGY.TOTAL"}]};

    //chMtxLock(&MtxTxPlc);
    const char *nombDisp = nombres::nomConId(idNombre);
    getMutex();
//    chprintf((BaseSequentialStream*)&SD6,"{\"evento\":\"mqttSuscribe\",\"tipo\":\"sonoff\",\"idDisp\":\"%d\",\"reports\":\[",numDispMqtt);
//    chprintf((BaseSequentialStream*)&SD6,  "{\"idReport\":\"SW\",\"topic\":\"stat/%s/RESULT\",\"info\":\"POWER\"}",nombDisp);
//    chprintf((BaseSequentialStream*)&SD6, ",{\"idReport\":\"SW\",\"topic\":\"tele/%s/STATE\",\"info\":\"POWER\"}",nombDisp);
//    chprintf((BaseSequentialStream*)&SD6, ",{\"idReport\":\"P\",\"topic\":\"tele/%s/SENSOR\",\"info\":\"ENERGY.Power\"}",nombDisp);
//    chprintf((BaseSequentialStream*)&SD6, ",{\"idReport\":\"Wh\",\"topic\":\"tele/%s/SENSOR\",\"info\":\"ENERGY.Total\"}]}\r",nombDisp);

    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"mqttSuscribe\",\"idDisp\":\"%d\",\"topic\":\"stat/%s/RESULT\",\"info\":\"POWER\"}\r",numDispMqtt,nombDisp);
    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"mqttSuscribe\",\"idDisp\":\"%d\",\"topic\":\"tele/%s/STATE\",\"info\":\"POWER\"}\r",numDispMqtt,nombDisp);
    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"mqttSuscribe\",\"idDisp\":\"%d\",\"topic\":\"tele/%s/SENSOR\",\"info\":\"ENERGY.Power\"}\r",numDispMqtt,nombDisp);
    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"mqttSuscribe\",\"idDisp\":\"%d\",\"topic\":\"tele/%s/SENSOR\",\"info\":\"ENERGY.Total\"}\r",numDispMqtt,nombDisp);


    //chMtxUnlock(&MtxTxPlc);
    releaseMutex();
}

void sonoff::suscribir(uint8_t *todoSuscrito)
{
    if (!suscrito)
    {
        *todoSuscrito = 0;
        enviaSolicitudSusc();
    }
}


void sonoff::actualizaEstSusc(uint8_t estadoSusc)
{
    suscrito = estadoSusc;
}

void sonoff::ponSalida(uint8_t valorDeseado)
{
    //envia peticion de salida
    //chMtxLock(&MtxTxPlc);
    if (valorDeseado>1)
        return;
    char msgs[2][4] = {"OFF","ON"};
    getMutex();
    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"mqttSend\",\"topic\":\"cmnd/%s/Power\",\"msg\":\"%s\"}\r",
                     nombres::nomConId(idNombre),msgs[valorDeseado]);
    //chMtxUnlock(&MtxTxPlc);
    releaseMutex();
    // esto me parece raro, el estado solo debería actualizarse por mensajes recibidos
    //estadoSolicitado = valorDeseado;
    dsOutDiferente = 0;
}


void sonoff::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t )
{
    // retorna 1 si hay error
    if (pinSalida==NULL || !suscrito)
        return;
    uint16_t difTimeDs = calendar::dsDiff(&dateTimeEnvioAnterior);
    // cada minuto envia todos los valores activos
    uint8_t valorDeseado = pinSalida->getValor();
    if ((valorDeseado != estadoReportado && difTimeDs>50) || difTimeDs > 600)
    {
        ponSalida(valorDeseado);
        calendar::getFechaHora(&dateTimeEnvioAnterior);
        return;
    }
    return;
}


void sonoff::initMqtt(void)
{
    suscrito = 0;
}

int8_t sonoff::init(void)
{
    if (pinSalida==NULL)
    {
        nextion::enviaLog(NULL,"#pinSalida no definido en SONOFF!");
        return 1;
    }
    return 0;
}



uint8_t sonoff::usaBus(void)
{
    return 0;
}

void sonoff::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"SONOFF %s",
               nombres::nomConId(idNombre));
    nextion::enviaLog(tty, buffer);
}


