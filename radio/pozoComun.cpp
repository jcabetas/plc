/*
 * pozoComun.c
 *
 *  Created on: 12/12/2019
 *      Author: jcabe
 */

#include "ch.hpp"
#include "hal.h"

using namespace chibios_rt;

//#include "lcd.h"
//#include "tipoVars.h"
//#include "../radio/pozo.h"
#include "string.h"
#include "chprintf.h"

#include "colas.h"
#include "bloques.h"
#include "nextion.h"
#include "radio.h"
#include "calendar.h"

thread_t *procesoDisplayPozo, *procesoMsgPozo;
event_source_t enviaLlamacionMsg_source;
event_source_t enviaPozoMsg_source;
event_source_t registraMsgPozo_source;
event_source_t rf95int_event;
event_source_t newMsgRx_source;
event_source_t newMsgTx_source;

//time_t GetTimeUnixSec(void);

extern struct queu_t colaCambiosPozo;
//time_t timeInicioPeticion[NUMSATELITES];
//time_t timeUltConexion[NUMSATELITES];
//uint8_t estadoLlamaciones, estadoActivos, estadoAbusones;

//float kWhPuntaInicioPeticion[NUMSATELITES];
//float kWhValleInicioPeticion[NUMSATELITES];

//uint8_t estadoLlamacionesOld, estadoActivosOld, estadoAbusonesOld;
//RTCDateTime dateTimeEnvioAnterior;
//int32_t msAleatorioMinEntreMsgsLllamador;
//int32_t msAleatorioMaxEntreMsgsLllamador;
//uint8_t petBomba = 0;
//uint8_t cnt;

// tratamiento de errores de Jandro
//uint8_t numErrorAviso[MAXERRORESAVISO];
//uint8_t idEstacionAviso[MAXERRORESAVISO];
//uint8_t mensajeAviso[MAXERRORESAVISO][21];
//time_t timeInicioAvisoError[MAXERRORESAVISO];

uint32_t msEntreFechas(RTCDateTime *fechaNew, RTCDateTime *fechaOld);
int32_t randomNum(int32_t numMin, int32_t numMax);
uint8_t esValle(void);


extern float  kWhtOld, kW;
//extern mutex_t MtxMedidas;

event_source_t hayRxParaLCD_source;
event_source_t hayCambiosLCD_source;

//extern varNUMERO dsMinEntreMsgsLlamador;
//extern varNUMERO dsMaxEntreMsgsLlamador;
extern parametroU16Flash *montarSD;
//extern varMODOPOZO modopozo;



//void radio::trataRxRegistradoryLlamador(struct msgRx_t *msgRx)
//{
//    uint8_t msgId = msgRx->msg[0];
//    uint8_t estProblematica;
//    uint16_t numBytes;
//    uint8_t bufError[25];
//    // Formato del mensaje:
//    //    Byte 0: MSG_STATUSPOZO
//    //    Byte 1: Id del originador: 0=Pozo, 1 a 8 sat�lites llamadores
//    //    Byte 2: Estado bomba del Pozo '0' '1'
//    //    Byte 3: Array de 8 bits indicando si el Pozo reconoce como activos a cada uno de los llamadores. Para ello el sat�lite ha de transmitir como mucho cada 15 segundos
//    //    Byte 4: Array de 8 bits indicando si el Pozo ha recibido una se�al de llamaci�n de un sat�lite. Se distribuye a todo el mundo, para que todos puedan saber la situaci�n
//    if (msgId==MSG_STATUSPOZO && msgRx->numBytes==5 && msgRx->msg[1]==0 && (msgRx->msg[2] == '0' || msgRx->msg[2] =='1'))
//    {
//        uint8_t numEstMsg = msgRx->msg[1];
//        uint8_t petBombaMsg = msgRx->msg[2]-'0';
//        uint8_t estadoActivosMsg = msgRx->msg[3];
//        uint8_t estadoLlamacionesMsg = msgRx->msg[4];
//        if (numEstMsg==0 && (petBomba==0 || petBomba==1))
//        {
//            radio::gestionaEstadoPozo(petBombaMsg, estadoLlamacionesMsg, estadoActivosMsg);
//            memcpy(&ultMsg, msgRx, sizeof(ultMsg));
//            chEvtBroadcast(&hayRxParaLCD_source);
//        }
//    }
//    if (msgId==MSG_STATUSLLAMACIONLOCAL && msgRx->numBytes==3)
//    {
//        uint8_t numEstMsg = msgRx->msg[1];
//        uint8_t petBombaMsg = msgRx->msg[2]-'0';
//        // incorpora peticion
//        if (numEstMsg>=1 && numEstMsg<=8 && (petBombaMsg==0 || petBombaMsg==1))
//        {
//            memcpy(&ultMsg, msgRx, sizeof(ultMsg));
//            chEvtBroadcast(&hayRxParaLCD_source);
//        }
//    }
//    // Tipo de Mensaje MSG_ERROR. Longitud=variable, de 4 a un maximo de 24 bytes
//    // Este mensaje es un beacon que se envia desde el sat�lite, indicando errores
//    //
//    // Formato del mensaje:
//    //    Byte 0: MSG_ERROR
//    //    Byte 1: Id del originador: 0=Pozo, 1 a 8 satelites llamadores
//    //    Byte 2: Numero de error
//    //    Byte 3: Id estacion con problemas. 0=Pozo, 1 a 8 sat�lites llamadores
//    //    Byte 4: N�mero de bytes del texto. Max 20.
//    //    Texto variable: de 1 a 21 bytes de texto (incluyendo el terminador 0)
//    if (msgId==MSG_ERROR && msgRx->numBytes>=5)
//    {
//        uint8_t numEst = msgRx->msg[1];
//        uint8_t numError = msgRx->msg[2];
//        if (numEst!=0 || numError!=1)
//            return;
//        estProblematica = msgRx->msg[3];
//        estadoAbusones |= (1<<(estProblematica-1));
//        numBytes = msgRx->msg[4];
//        if (numBytes>sizeof(bufError))
//            numBytes = sizeof(bufError);
//        memcpy(bufError, &msgRx->msg[5],numBytes);
//        bufError[numBytes] = 0; // fin de cadena
//        actualizoErrorDesdeLlamador(estProblematica, numError, bufError);
//        memcpy(&ultMsg, msgRx, msgRx->numBytes);
//        chEvtBroadcast(&hayRxParaLCD_source);
//    }
//    // Tipo de Mensaje MSG_CLEARERROR. Longitud=4
//    // Este mensaje lo env�a el Pozo si ha desaparecido un error de una estaci�n
//    //
//    // Formato del mensaje:
//    //    Byte 0: MSG_CLEARERROR
//    //    Byte 1: Id del originador: 0=Pozo, 1 a 8 sat�lites llamadores
//    //    Byte 2: N�mero de error
//    //    Byte 3: Id estaci�n con problemas resueltos. 0=Pozo, 1 a 8 sat�lites llamadores
//    //
//    if (msgId==MSG_CLEARERROR && msgRx->numBytes==4)
//    {
//        uint8_t numEst = msgRx->msg[1];
//        uint8_t numError = msgRx->msg[2];
//        if (numEst!=0 || numError!=1)
//            return;
//        estProblematica = msgRx->msg[3];
//        estadoAbusones &= ~(1<<(estProblematica-1));
//        limpiaError(estProblematica, numError);
//        memcpy(&ultMsg, msgRx, msgRx->numBytes);
//        chEvtBroadcast(&hayRxParaLCD_source);
//    }
//}




/*
 * Registra cambios segun estadoLlamaciones
 */
void radio::registraCambiosPeticion(uint8_t estLlamaciones, uint8_t estLlamacionesOld)
{
    time_t ahora = calendar::getSecUnix();
    for (uint8_t est=0;est<8;est++)
    {
        uint8_t estadoOld = (estLlamacionesOld>>est) & 1;
        uint8_t estadoNew = (estLlamaciones>>est) & 1;
        if (estadoNew && !estadoOld) // empieza a pedir
            timeInicioPeticion[est] = ahora;
        if (!estadoNew && estadoOld) // deja de pedir
        {
            // registro en memoria permanente
//            uint32_t tiempoOn = ahora - timeInicioPeticion[est];
// ToDo JCF         struct datosPozoGuardados *datos = (struct datosPozoGuardados *) BKPSRAM_BASE;
//            datos->datosId[est].segundosPeticion += tiempoOn;
//            datos->datosId[est].numPeticiones += 1;
        }
    }
}


//void radio::ponStatus(sms *smsPtr)
//{
//    struct datosIdGuardados *datoId;
//    char buff[50];
//    struct datosPozoGuardados *datos = (struct datosPozoGuardados *) BKPSRAM_BASE;
//    if (datos->seedInicial != SEEDHISTORICO)
//    {
//          chsnprintf(buff,sizeof(buff),"Historico corrompido, lo reinicio\n");
//         // reseteaHistoriaPozo();
//          return;
//    }
//    chsnprintf(buff,sizeof(buff),"Id NumPet Min kWhP kWhV\n");
//    smsPtr->addMsgRespuesta(buff);
//    for (uint8_t id=0;id<8;id++)
//    {
//        datoId = &datos->datosId[id];
//        if (datoId->numPeticiones==0)
//            continue;
//        uint32_t duracion = datoId->segundosPeticion;
//        uint32_t min = duracion/60;
//        chsnprintf(buff,sizeof(buff),"%d %d %d %.1f %.1f\n",id+1, datoId->numPeticiones,min,datoId->kWhPunta, datoId->kWhValle);
//        smsPtr->addMsgRespuesta(buff);
//    }
//}
