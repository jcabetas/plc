/*
 * wwwThread.cpp
 *
 *  Created on: 24 ago. 2021
 *      Author: joaquin
 */


#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"
#include "tty.h"
#include "colas.h"
#include "bloques.h"

/*
 * nextionCom.cpp
 *
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;

#include "chprintf.h"
#include "tty.h"
#include "gets.h"
#include "string.h"
#include "colas.h"


thread_t *procesoWWWCom;
char bufferReceivedWWW[150];
event_source_t enviarWWWsource;
extern struct queu_t colaMsgTxWWW;
//extern mutex_t MtxTxPlc;
void getMutex(void);
void releaseMutex(void);

void  trataRxWWW(uint8_t *msgRxWWW, uint8_t numBytes);


static const SerialConfig ser_cfg = {
    115200, 0, 0, 0,
};


// mensajes desde nextion. Empiezan en @ y terminan en 0
void chgetRaspiNoEchoTimeOut(BaseChannel   *pSD, char *buffer, uint16_t bufferSize,systime_t timeout, uint16_t *numBytes, uint8_t *huboTimeout)
{
    uint8_t ch;
    *huboTimeout = 0;
    *numBytes = 0;
    while (1==1)
      {
      ch = chgetchTimeOut(pSD, timeout, huboTimeout);
      if (*huboTimeout) // timeout
         break;
      if (*numBytes<bufferSize)
        buffer[(*numBytes)++] = ch;
      if (*numBytes>1 && ch=='\r') // si es un mesaje adhoc mio
          break;                                 // no esperes al timeout
      }
    buffer[*numBytes] = 0;
}


/*
 * Proceso wwwCom
 * Escucha para eventos a enviar a WWW
 */

static THD_WORKING_AREA(wathreadWWWCom, 4500);

static THD_FUNCTION(threadWWWCom, arg) {
    (void)arg;

    uint8_t exito;
    struct msgTxWWW_t ptrDatTx;

    eventmask_t evt;
    uint8_t huboTimeout;
    uint16_t numBytes, numDsSinTx;
    event_listener_t wwwSend_listener, receivedData;
    chRegSetThreadName("wwwCom");

    chEvtRegisterMask(&enviarWWWsource, &wwwSend_listener,EVENT_MASK(0));
    chEvtRegisterMask(chnGetEventSource(&NEXTIONSD),&receivedData, EVENT_MASK (1));

    numDsSinTx = 0;
    // indico a Raspberry que estamos arrancando, y que borre todo
    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"borra\"}\r");
    while (true)
    {
        evt = chEvtWaitOneTimeout(ALL_EVENTS, TIME_MS2I(100));
        if (evt & EVENT_MASK(1)) // Algo ha entrado desde Raspberry
        {
            chgetRaspiNoEchoTimeOut((BaseChannel *)&NEXTIONSD, bufferReceivedWWW, sizeof(bufferReceivedWWW), TIME_MS2I(20),&numBytes,&huboTimeout);
            if (huboTimeout)
              continue;
            www::trataRxWWW(bufferReceivedWWW);
            continue;
        }
        // hay algun cambio que enviar?
        if (colaMsgTxWWW.numItems>0 || (evt & EVENT_MASK(0))) // Evento enviar a Raspi
        {
            // si el PLC no esta listo, limpia cola y aborta
            if (!www::estaActivo())
            {
                clearQueu(&colaMsgTxWWW);
                continue;
            }
            getMutex();
            //chMtxLock(&MtxTxPlc);
            uint8_t numVolcados = 0;
            // cabecera JSON de cambios
            chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"update\",\"datos\":[");
            do
            {
                exito = getQueu(&colaMsgTxWWW, &ptrDatTx);
                if (!exito)
                    break;
                uint8_t idNombVariable = ptrDatTx.idNombVariable;
                uint8_t accion = ptrDatTx.accion;
                if (numVolcados>0) // pon una coma
                    chprintf((BaseSequentialStream*)&NEXTIONSD,",");
                if (accion == TXWWWESTADO)
                {
                    // estado (es ajuste, con id)
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"estado\":%s}",
                                     nombres::nomConId(idNombVariable),(char *) ptrDatTx.valor);
                    numVolcados++;
                }
                else if (accion == TXWWWESTADOVAR)
                {
                    // estado (es ajuste, con id)
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"salida\":%s}",
                                     nombres::nomConId(idNombVariable),(char *) ptrDatTx.valor);
                    numVolcados++;
                }
                else if (accion == TXWWWMEDIDAVAL)
                {
                    // estado (es ajuste, con id)
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"valor\":%s}",
                                     nombres::nomConId(idNombVariable),(char *) ptrDatTx.valor);
                    numVolcados++;
                }
                else if (accion == TXWWWZONAACTIVA)
                {
                    // enviar zona activa de riego
                    //{"id":"riego","accion":"zona","zona":2},
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"zona\",\"zona\":%s}",
                                     nombres::nomConId(idNombVariable),(char *) ptrDatTx.valor);
                    numVolcados++;
                }
                else if (accion == TXWWWTZONA)
                {
                    // enviar tiempo de zona de riego
                    //{"id":"riego","accion":"setMinZ","zona":1,"codAB":"A","tiempo":"110"}
                    // variable=zonas[zz-1].Ta
                    // los parametros estan codificados "ZZ,A,TTTT"
                    uint8_t numPar;
                    char *par[15];
                    divideString(NULL, (char *) ptrDatTx.valor, &numPar, par);
                    if (numPar!=3)
                        continue;
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"setMinZ\",\"zona\":\"%s\",\"codAB\":\"%s\",\"tiempo\":\"%s\"}",
                                     nombres::nomConId(idNombVariable),par[0],par[1],par[2]);
                    numVolcados++;
                }
                else if (accion == TXWWWPROGBOMBA)
                {
                    // enviar estado de bomba
                    //{"id":"riego","accion":"bomba","estado":1}
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"bomba\",\"estado\":%s}",
                                     nombres::nomConId(idNombVariable),(char *) ptrDatTx.valor);
                    numVolcados++;
                }
                else if (accion == TXWWWPROGESPERA)
                {
                    // enviar estado de espera
                    //{"id":"riego","accion":"espera","estado":1}
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"espera\",\"estado\":%s}",
                                     nombres::nomConId(idNombVariable),(char *) ptrDatTx.valor);
                    numVolcados++;
                }
                else if (accion == TXWWWTFLUJOMAX)
                {
                    // enviar flujo maximo de zona de riego
                    //{"id":"riego","accion":"flujoMax","zona":"1","flujo":"250"}
                    // los parametros estan codificados "ZZ,TTTT"
                    uint8_t numPar;
                    char *par[15];
                    divideString(NULL, (char *) ptrDatTx.valor, &numPar, par);
                    if (numPar!=2)
                        continue;
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"flujoMax\",\"zona\":\"%s\",\"flujo\":\"%s\"}",
                                     nombres::nomConId(idNombVariable),par[0],par[1]);
                    numVolcados++;
                }
                else if (accion == TXWWWDOW)
                {
                    // enviar DOW de un arranque
                    ////{"id":"riego","accion":"setDOW","numArranque":1,"DOW":"LXV"}
                    // los parametros estan codificados "numArr,DOW"
                    uint8_t numPar;
                    char *par[15];
                    divideString(NULL, (char *) ptrDatTx.valor, &numPar, par);
                    if (numPar!=2)
                        continue;
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"setDOW\",\"numArranque\":%s,\"DOW\":\"%s\"}",
                                     nombres::nomConId(idNombVariable),par[0],par[1]);
                    numVolcados++;
                }
                else if (accion == TXWWWHSTART)
                {
                    // enviar DOW de un arranque
                    ////{"id":"riego","accion":"setHoraStart","numArranque":1,"valor":9}
                    // los parametros estan codificados "numArr,hora"
                    uint8_t numPar;
                    char *par[15];
                    divideString(NULL, (char *) ptrDatTx.valor, &numPar, par);
                    if (numPar!=2)
                        continue;
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"setHoraStart\",\"numArranque\":%s,\"valor\":\"%s\"}",
                                     nombres::nomConId(idNombVariable),par[0],par[1]);
                    numVolcados++;
                }
                else if (accion == TXWWWMSTART)
                {
                    // enviar DOW de un arranque
                    ////{"id":"riego","accion":"setMinStart","numArranque":1,"valor":30}
                    // los parametros estan codificados "numArr,min"
                    uint8_t numPar;
                    char *par[15];
                    divideString(NULL, (char *) ptrDatTx.valor, &numPar, par);
                    if (numPar!=2)
                        continue;
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"setMinStart\",\"numArranque\":%s,\"valor\":\"%s\"}",
                                     nombres::nomConId(idNombVariable),par[0],par[1]);
                    numVolcados++;
                }
                else if (accion == TXWWWESBSTART)
                {
                    // enviar DOW de un arranque
                    ////{"id":"riego","accion":"setEsBStart","numArranque":1,"valor":1}
                    // los parametros estan codificados "numArr,esB"
                    uint8_t numPar;
                    char *par[15];
                    divideString(NULL, (char *) ptrDatTx.valor, &numPar, par);
                    if (numPar!=2)
                        continue;
                    chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"id\":\"%s\",\"accion\":\"setEsBStart\",\"numArranque\":%s,\"valor\":\"%s\"}",
                                     nombres::nomConId(idNombVariable),par[0],par[1]);
                    numVolcados++;
                }
            } while (exito);
            // cola JSON de cambios
            chprintf((BaseSequentialStream*)&NEXTIONSD,"]}\r");
            releaseMutex();
            //chMtxUnlock(&MtxTxPlc);
            numDsSinTx = 0;
            continue;
        }
        if (chThdShouldTerminateX())
        {
            chEvtUnregister(&enviarWWWsource, &wwwSend_listener);
            chEvtUnregister(chnGetEventSource(&NEXTIONSD), &receivedData);
            chThdExit((msg_t) 1);
        }
        if (++numDsSinTx>100 && www::estaActivo())
        {
            // enviar heartbat {"evento":"heartbeat"}
            chprintf((BaseSequentialStream*)&NEXTIONSD,"{\"evento\":\"heartbeat\"}\r");
            numDsSinTx = 0;
        }
    }
}

void www::mataWWWCom(void)
{
    if (procesoWWWCom!=NULL)
    {
        chThdTerminate(procesoWWWCom);
        chThdWait(procesoWWWCom);
        procesoWWWCom = NULL;
    }
}

void www::initWWWThread(void)
{

    // hardware init
    palClearLine(LINE_TX1);
    palSetLine(LINE_RX1);
    palSetLineMode(LINE_TX1, PAL_MODE_ALTERNATE(8));
    palSetLineMode(LINE_RX1, PAL_MODE_ALTERNATE(8));
    sdStart(&NEXTIONSD,&ser_cfg);
    if (!procesoWWWCom)
        procesoWWWCom = chThdCreateStatic(wathreadWWWCom, sizeof(wathreadWWWCom), NORMALPRIO, threadWWWCom, NULL);
}



