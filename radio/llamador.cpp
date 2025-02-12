/*
 * llamador.c
 *
 *  Created on: 12/12/2019
 *      Author: jcabe
 */

#include "ch.hpp"
#include "hal.h"


using namespace chibios_rt;

#include "string.h"
#include "chprintf.h"
#include <stdio.h>
#include "colas.h"
#include "RH_RF95.h"
//#include "../radio/pozo.h"
//#include "tipoVars.h"
#include "bloques.h"
#include "radio.h"
#include "nextion.h"
#include "calendar.h"

//uint32_t msEntreFechas(RTCDateTime *fechaNew, RTCDateTime *fechaOld);
//time_t GetTimeUnixSec(void);
int32_t randomNum(int32_t numMin, int32_t numMax);
void int2str(uint8_t valor, char *string);

extern struct queu_t colaMsgTx;
extern event_source_t newMsgTx_source;
extern struct queu_t colaMsgRx;

extern struct msgRx_t ultMsg;
//extern event_source_t hayRxParaLCD_source;
//extern event_source_t hayCambiosLCD_source;




//uint8_t estadoBombaRequeridobah(void)
//{
//    //return palReadPad(GPIOE, 0);
//    estados::diEstado(numInput);
//    return peticionAgua.getValorNum();
//}

/*
 *   Los cambios se detectan por los mensajes del pozo
 *   Si hay cambios en estados, graba en SD y resetea
 */
/*
 *     uint16_t numInput;
    uint8_t bombaPozoOn;
    int32_t msAleatorioMinEntreMsgsLllamador;
    int32_t msAleatorioMaxEntreMsgsLllamador;
    RTCDateTime dateTimeEnvioAnterior;
    time_t timeUltConexion[NUMSATELITES];
    // tratamiento de errores de Jandro
    uint8_t numErrorAviso[MAXERRORESAVISO];
    uint8_t idEstacionAviso[MAXERRORESAVISO];
    uint8_t mensajeAviso[MAXERRORESAVISO][21];
    time_t timeInicioAvisoError[MAXERRORESAVISO];
    parametroU16Flash *idLlamador;
    parametroU16Flash *dsMaxEntreMsgsLlamador;
    parametroU16Flash *dsMinEntreMsgsLlamador;

    TODO: poner TimeOut para comunicacion con pozo sOlv

 */
// LLAMADOR zona3 [idLlam 1] [dsMin 5] [dsMax 100] [sOlvido 30] pozo.logPozo.txt bOn.radio.pic.0 comOk.radio.pic.0 llam.radio.txt act.radio.txt abu.radio.txt
llamador::llamador(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    uint8_t tipoNextion, picBase;
    if (numPar!=12)
    {
        nextion::enviaLog(tty,"#parametros incorrectos LLAMADOR");
        *hayError = 1;
        return;
    }
    modoRadio = MODOLLAMADOR;
    bombaPozoOn = 0;
    bombaPozoSolicitada = 0;
    calendar::getFechaHora(&dateTimeEnvioAnterior);
    numInput = estados::addEstado(tty, pars[1], 0, hayError);
    idLlamador = parametro::addParametroU16FlashMinMax(tty, pars[2],1,8,hayError);
    dsMinEntreMsgsLlamador = parametro::addParametroU16FlashMinMax(tty, pars[3],2,60,hayError);
    dsMaxEntreMsgsLlamador = parametro::addParametroU16FlashMinMax(tty, pars[4],50,3600,hayError);
    sOlvido = parametro::addParametroU16FlashMinMax(tty, pars[5],20,3600,hayError);
    hallaNombreyDatosNextion(pars[6], TIPONEXTIONSTR, &idNombreLog, &idNombreLogWWW, &idPageLog, &tipoNextion, &picBase);
    numEstadoBomba = estados::addEstado(tty, pars[7], 1, hayError);
    numEstadoComOk = estados::addEstado(tty, pars[8], 1, hayError);
    hallaNombreyDatosNextion(pars[9], TIPONEXTIONSTR, &idNombreLlamadores, &idNombreLlamadoresWWW, &idPageLlamadores, &tipoNextion, &picBase);
    if (tipoNextion!=TIPONEXTIONSTR)
    {
        nextion::enviaLog(tty,"LLAMADOR llamadores no txt");
        *hayError = 1;
    }
    // nextion activos
    hallaNombreyDatosNextion(pars[10], TIPONEXTIONSILENCIO, &idNombreActivos, &idNombreActivosWWW, &idPageActivos, &tipoNextion, &picBase);
    if (tipoNextion!=TIPONEXTIONSTR)
    {
        nextion::enviaLog(tty,"LLAMADOR activos no txt");
        *hayError = 1;
    }
    // nextion abusones
    hallaNombreyDatosNextion(pars[11], TIPONEXTIONSILENCIO, &idNombreAbusones, &idNombreAbusonesWWW, &idPageAbusones, &tipoNextion, &picBase);
    if (tipoNextion!=TIPONEXTIONSTR)
    {
         nextion::enviaLog(tty,"LLAMADOR abusones no txt");
         *hayError = 1;
    }
    radioPtr = this;

}



void llamador::reseteaVariablesEspecificas(void)
{
    dsAleatorioMinEntreMsgsLlamador= dsMinEntreMsgsLlamador->valor()+randomNum(0,10); //dsMinEntreMsgsLlamadorValor()
    dsAleatorioMaxEntreMsgsLlamador= dsMaxEntreMsgsLlamador->valor()+randomNum(0,10); //dsMinEntreMsgsLlamadorValor()
}


/*
 * Envia MSG_STATUSLLAMACIONLOCAL
 * Byte 0: MSG_STATUSLLAMACIONLOCAL
 * Byte 1: Id del originador: 0=Pozo, 1 a 8 satelites llamadores
 * Byte 2: Estado llamacion local '0' '1'
 * Llamador hace su peticien de bomba
 */
void llamador::enviaStatusLlamacion(void)
{
    struct msgTx_t msgTx;
    msgTx.numBytes = 3;
    msgTx.msg[0] = MSG_STATUSLLAMACIONLOCAL;
    msgTx.msg[1] = idLlamador->valor();    //idLlamadorValor();
    if (bombaPozoSolicitada) // peticion activacion?
        msgTx.msg[2] = '1';
    else
        msgTx.msg[2] = '0';
    //putQueu(&colaMsgTx, &msgTx);
    //chEvtBroadcast(&newMsgTx_source);
    RH_RF95_send(msgTx.msg,msgTx.numBytes);
    RHGenericDriver_waitPacketSent(100);
    RH_RF95_setModeRx();
    calendar::getFechaHora(&dateTimeEnvioAnterior);
}



void llamador::trataObsoletoLlamador(void)
{
    // ha pasado mucho tiempo sin recibir?
    if (calendar::sDiff(&dateTimeRxPozoAnterior)>sOlvido->valor())
    {
        estados::ponEstado(numEstadoComOk, 0);
    }
    // tengo que refrescar datos al pozo?
    if (calendar::dsDiff(&dateTimeEnvioAnterior) > dsAleatorioMaxEntreMsgsLlamador)
    {
        enviaStatusLlamacion();
        dsAleatorioMaxEntreMsgsLlamador= dsMaxEntreMsgsLlamador->valor()+randomNum(0,10);
    }
}




/*
 *  Trata la recepcion de una notificacion del pozo
 *  Devuelve 1 si hay cambios
 */
uint8_t llamador::gestionaEstadoPozo(uint8_t petBombaMsg, uint8_t estadoLlamacionesMsg, uint8_t estadoActivosMsg)
{
    uint8_t estadoLlamacionesOld, estadoActivosOld, estadoAbusonesOld, petBombaOld;
    estadoLlamacionesOld = estadoLlamaciones;
    estadoActivosOld = estadoActivos;
    petBombaOld = bombaPozoOn; //estados::diEstado(numInput);//petBomba;
    estadoLlamaciones = estadoLlamacionesMsg;
    uint8_t miId = idLlamador->valor();      // Id llamador
    uint8_t estoyConectadoOld = (estadoActivos>>(miId-1)) & 1;
    estadoActivos = estadoActivosMsg;
    estadoAbusonesOld = estadoAbusones;
    bombaPozoOn = petBombaMsg;
    uint8_t yoEstabaPidiendoMsg = (estadoLlamacionesMsg>>(miId-1)) & 1;
    uint8_t estoyConectadoMsg = (estadoActivosMsg>>(miId-1)) & 1;
    // miro si el pozo tiene mal mi peticion y le tengo que informar de mi estado, o bien dice que no existo
    if ((yoEstabaPidiendoMsg!=bombaPozoSolicitada || !estoyConectadoMsg))
    {
        if (calendar::dsDiff(&dateTimeEnvioAnterior)>(uint32_t)dsAleatorioMinEntreMsgsLlamador)
        {
            enviaStatusLlamacion();
            dsAleatorioMinEntreMsgsLlamador= dsMinEntreMsgsLlamador->valor()+randomNum(0,10);
        }
    }
    if (estoyConectadoMsg!=estoyConectadoOld)
    {
        estados::ponEstado(numEstadoComOk, estoyConectadoMsg);
        estados::actualizaNextion(numEstadoComOk);
    }
    if(estoyConectadoOld!=estoyConectadoMsg)
    {
        estados::ponEstado(numEstadoBomba, bombaPozoOn);
        estados::actualizaNextion(numEstadoBomba); // todo bombaPozoOn);
        // OJO enviaValPic(idPageComOk, idNombreComOk, tipComOk, picComOk, estoyConectadoMsg);
    }
    if (estadoLlamaciones!=estadoLlamacionesOld)
        radio::registraCambiosPeticion(estadoLlamaciones, estadoLlamacionesOld);  // registra cambios en memoria permanente
    if (estadoLlamaciones!=estadoLlamacionesOld || estadoActivos!=estadoActivosOld || bombaPozoOn!=petBombaOld)
    {
        ponEnColaRegistrador();     // guardar en SD
        actualizaNextion(estadoLlamacionesOld, estadoActivosOld, estadoAbusonesOld, petBombaOld);
        return 1;
    }
    else
        return 0;
}

void llamador::trataRx(struct msgRx_t *msgRx)
{
    uint8_t msgId = msgRx->msg[0];
    uint8_t estProblematica;
    uint16_t numBytes;
    uint8_t bufError[25];
    char buffer[40];
    char varName[20];
    struct tm fechHora;
    // Formato del mensaje:
    //    Byte 0: MSG_STATUSPOZO
    //    Byte 1: Id del originador: 0=Pozo, 1 a 8 satelites llamadores
    //    Byte 2: Estado bomba del Pozo '0' '1'
    //    Byte 3: Array de 8 bits indicando si el Pozo reconoce como activos a cada uno de los llamadores. Para ello el satelite ha de transmitir como mucho cada 15 segundos
    //    Byte 4: Array de 8 bits indicando si el Pozo ha recibido una senyal de llamacion de un satelite. Se distribuye a todo el mundo, para que todos puedan saber la situacion
    if (msgId==MSG_STATUSPOZO && msgRx->numBytes==5 && msgRx->msg[1]==0 && (msgRx->msg[2] == '0' || msgRx->msg[2] =='1'))
    {
        uint8_t numEstMsg = msgRx->msg[1];
        uint8_t petBombaMsg = msgRx->msg[2]-'0';
        uint8_t estadoActivosMsg = msgRx->msg[3];
        uint8_t estadoLlamacionesMsg = msgRx->msg[4];
        calendar::getFechaHora(&dateTimeRxPozoAnterior);
        estados::ponEstado(numEstadoComOk, 1);
        if (numEstMsg==0 && (petBombaMsg==0 || petBombaMsg==1))
        {
            gestionaEstadoPozo(petBombaMsg, estadoLlamacionesMsg, estadoActivosMsg);
            memcpy(&ultMsg, msgRx, sizeof(ultMsg));
            calendar::getFecha(&fechHora);
            chsnprintf(buffer,sizeof(buffer),"%2d:%02d:%02d Msg pozo. Bomba:%d RSSI:%d",fechHora.tm_hour, fechHora.tm_min, fechHora.tm_sec, petBombaMsg, msgRx->rssi);
            uint8_t varNext = numEstMsg;
            if (varNext>6) varNext=6;
            chsnprintf(varName, sizeof(varName),"%s%d",nombres::nomConId(idNombreLog),numEstMsg);
            enviaTxtSiEnPage(idPageLog,varName, buffer);
        }
    }
    if (msgId==MSG_STATUSLLAMACIONLOCAL && msgRx->numBytes==3)
    {
        uint8_t numEstMsg = msgRx->msg[1];
        uint8_t petBombaMsg = msgRx->msg[2]-'0';
        // incorpora peticion
        if (numEstMsg>=1 && numEstMsg<=8 && (petBombaMsg==0 || petBombaMsg==1))
        {
            memcpy(&ultMsg, msgRx, sizeof(ultMsg));
            calendar::getFecha(&fechHora);
            chsnprintf(buffer,sizeof(buffer),"%2d:%02d:%02d Msg de #%d. Llamacion:%d RSSI:%d",fechHora.tm_hour, fechHora.tm_min, fechHora.tm_sec, numEstMsg,petBombaMsg, msgRx->rssi);
//            enviaTxtSiEnPage(idPageLog,idNombreLog, buffer);
            uint8_t varNext = numEstMsg;
            if (varNext>6) varNext=6;
            chsnprintf(varName, sizeof(varName),"%s%d",nombres::nomConId(idNombreLog),numEstMsg);
            enviaTxtSiEnPage(idPageLog,varName, buffer);
        }
    }
    // Tipo de Mensaje MSG_ERROR. Longitud=variable, de 4 a un maximo de 24 bytes
    // Este mensaje es un beacon que se envia desde el satelite, indicando errores
    //
    // Formato del mensaje:
    //    Byte 0: MSG_ERROR
    //    Byte 1: Id del originador: 0=Pozo, 1 a 8 satelites llamadores
    //    Byte 2: Numero de error
    //    Byte 3: Id estacion con problemas. 0=Pozo, 1 a 8 satelites llamadores
    //    Byte 4: Numero de bytes del texto. Max 20.
    //    Texto variable: de 1 a 21 bytes de texto (incluyendo el terminador 0)
    if (msgId==MSG_ERROR && msgRx->numBytes>=5)
    {
        uint8_t numEst = msgRx->msg[1];
        uint8_t numError = msgRx->msg[2];
        if (numEst!=0 || numError!=1)
            return;
        uint8_t estadoAbusonesOld = estadoAbusones;
        estProblematica = msgRx->msg[3];
        estadoAbusones |= (1<<(estProblematica-1));
        numBytes = msgRx->msg[4];
        if (numBytes>sizeof(bufError))
            numBytes = sizeof(bufError);
        memcpy(bufError, &msgRx->msg[5],numBytes);
        bufError[numBytes] = 0; // fin de cadena
        actualizoErrorDesdeLlamador(estProblematica, numError, bufError);
        memcpy(&ultMsg, msgRx, msgRx->numBytes);
        if (estadoAbusones != estadoAbusonesOld)
        {
            int2str(estadoAbusones, buffer);
            enviaTxt(idPageAbusones, idNombreAbusones, buffer);
            calendar::getFecha(&fechHora);
            chsnprintf(buffer,sizeof(buffer),"%2d:%02d:%02d Abusa #%d msg:'%s'",fechHora.tm_hour, fechHora.tm_min, fechHora.tm_sec,estProblematica,bufError);
            chsnprintf(varName, sizeof(varName),"%s%d",nombres::nomConId(idNombreLog),numEst);
            enviaTxtSiEnPage(idPageLog,varName, buffer);
            //enviaTxt(idPageLog,idNombreLog, buffer);
        }
    }
    // Tipo de Mensaje MSG_CLEARERROR. Longitud=4
    // Este mensaje lo envia el Pozo si ha desaparecido un error de una estacion
    //
    // Formato del mensaje:
    //    Byte 0: MSG_CLEARERROR
    //    Byte 1: Id del originador: 0=Pozo, 1 a 8 satelites llamadores
    //    Byte 2: Nemero de error
    //    Byte 3: Id estacion con problemas resueltos. 0=Pozo, 1 a 8 satelites llamadores
    //
    if (msgId==MSG_CLEARERROR && msgRx->numBytes==4)
    {
        uint8_t numEst = msgRx->msg[1];
        uint8_t numError = msgRx->msg[2];
        if (numEst!=0 || numError!=1)
            return;
        uint8_t estadoAbusonesOld = estadoAbusones;
        estProblematica = msgRx->msg[3];
        estadoAbusones &= ~(1<<(estProblematica-1));
        radio::limpiaError(estProblematica, numError);
        memcpy(&ultMsg, msgRx, msgRx->numBytes);
        if (estadoAbusones != estadoAbusonesOld)
        {
            int2str(estadoAbusones, buffer);
            enviaTxt(idPageAbusones, idNombreAbusones, buffer);
            calendar::getFecha(&fechHora);
            chsnprintf(buffer,sizeof(buffer),"%2d:%02d:%02d Deja de abusar #%d",fechHora.tm_hour, fechHora.tm_min, fechHora.tm_sec, estProblematica);
            chsnprintf(varName, sizeof(varName),"%s%d",nombres::nomConId(idNombreLog),numEst);
            enviaTxtSiEnPage(idPageLog,varName, buffer);
            //enviaTxt(idPageLog,idNombreLog, buffer);
        }
    }
}

void llamador::trataRxRf95(eventmask_t evt)
{
    struct msgRx_t msgRx;
    struct msgTx_t msgTx;
    if (evt==0) // timeout, llamo a rutinas
    {
        trataObsoletoLlamador();
    }
    if (evt & EVENT_MASK(0)) // He recibido un mensaje rf95
    {
        while (getQueu(&colaMsgRx, &msgRx))
        {
            if (++cnt>99) cnt=0;
            trataRx(&msgRx);
        }
    }
    if (evt & EVENT_MASK(1)) // Tengo que enviar mensajes rf95
    {
        while (getQueu(&colaMsgTx, &msgTx))
        {
            RH_RF95_send(msgTx.msg,msgTx.numBytes);
            RHGenericDriver_waitPacketSent(100);
            RH_RF95_setModeRx();
        }
    }
}



llamador::~llamador()
{
    paraRadio();
}

const char *llamador::diTipo(void)
{
    return "LLAMADOR";
}

const char *llamador::diNombre(void)
{
    return "LLAMADOR";
}

int8_t llamador::init(void)
{
    arrancaRadio();
    calendar::getFechaHora(&dateTimeRxPozoAnterior);
    estados::ponEstado(numEstadoComOk, 0);
    actualizaNextion(99,99,99,99);
    return 0;
}

void llamador::stop(void)
{
    paraRadio();
}

void llamador::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t )
{
    uint8_t estatusInput = estados::diEstado(numInput);
    if (estatusInput != bombaPozoSolicitada)
    {
        if (calendar::dsDiff(&dateTimeEnvioAnterior)>(uint32_t)dsAleatorioMinEntreMsgsLlamador)
        {
            bombaPozoSolicitada = estatusInput;
            enviaStatusLlamacion();
            dsAleatorioMinEntreMsgsLlamador= dsMinEntreMsgsLlamador->valor()+randomNum(0,10);
        }
    }
}

void llamador::print(BaseSequentialStream *tty)
{
    char buffer[60];
    chsnprintf(buffer,sizeof(buffer),"LLAMADOR input:%s-%d  bOn:%s-%d commOk:%s-%d",estados::nombre(numInput),numInput,
                   estados::nombre(numEstadoBomba),numEstadoBomba, estados::nombre(numEstadoComOk),numEstadoComOk);
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}







