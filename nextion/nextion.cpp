/*
 * led.cpp
 *
 *  Created on: 28 jun. 2020
 *      Author: jcabe
 */


#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "bloques.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"


extern mutex_t MtxNextionTx;
//void GetTimeTm(struct tm *timp, uint16_t *ds);
void rtcGetTM(RTCDriver *rtcp, struct tm *tim, uint16_t *ds);

/*
 *   uint16_t idNombreTmpNxt;
 *   uint16_t idPageTmpNxt;
 */
uint16_t nextion::idNombreTmpNxt = 0;
uint16_t nextion::idNombreTmpNxtWWW = 0;
uint16_t nextion::idPageTmpNxt = 0;
uint16_t nextion::idNombreLog = 0;
uint16_t nextion::idNombreLogWWW = 0;
uint16_t nextion::idPageLog = 0;
uint16_t nextion::idPageBlank = 0;
uint16_t nextion::contDsInactividad = 0;
uint16_t nextion::maxDsInactividad = 0xFFFF;
uint32_t nextion::baud = 0;
uint16_t nextion::idPagActual = 0;

//NEXTION 115200 PLC.logPlc base.tmpVar blankPage 120
void nextion::setNextion(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    uint8_t tipoNxt, picBaseNxt;
    if (numPar!=6)
    {
        chprintf(tty,"#parametros incorrectos NEXTION\n\r");
        *hayError = 1;
        return; // error
    }
    nextion::baud = atoi(pars[1]);
    uint8_t error2 = hallaNombreyDatosNextion(pars[2], TIPONEXTIONSTR, &idNombreLog, &idNombreLogWWW, &idPageLog, &tipoNxt, &picBaseNxt);
    uint8_t error3 = hallaNombreyDatosNextion(pars[3], TIPONEXTIONSTR, &idNombreTmpNxt, &idNombreTmpNxtWWW, &idPageTmpNxt, &tipoNxt, &picBaseNxt);
    idPageBlank = nombres::incorpora(pars[4]);
    maxDsInactividad = 10*atoi(pars[5]);
    contDsInactividad = maxDsInactividad;
    if (error2 || error3)
        *hayError = 1;
    if (tipoNxt!=TIPONEXTIONSTR)
    {
        nextion::enviaLog(NULL,"Error campo nextion no STR");
        *hayError = 1;
    }
    nextion::clearLog();
}

void nextion::clearLog(void)
{
    char buffer[50];
    if (idNombreLog==0 || idPageLog==0)
        return;
    // sms.logSms.txt="Bueno \r"+base.logTmp.txt
    chsnprintf(buffer,sizeof(buffer),"%s.%s.txt=\"\"%c%c%c",nombres::nomConId(idPageLog), nombres::nomConId(idNombreLog), 0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,buffer);
    chMtxUnlock(&MtxNextionTx);
    chThdSleepMilliseconds(100);
}


void nextion::clearLog(campoNextion *campoLog)
{
    char buffer[50];
    if (campoLog==NULL)
        return;
    // sms.logSms.txt="Bueno \r"+base.logTmp.txt
    chsnprintf(buffer,sizeof(buffer),"%s.%s.txt=\"\"%c%c%c",nombres::nomConId(campoLog->diPageNxt()), nombres::nomConId(campoLog->diNombreNxt()),0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,buffer);
    chMtxUnlock(&MtxNextionTx);
}

/*
 * Gestion de actividad
 */
void nextion::setBlankPage(void)
{
    char buffer[30];
    // page blank
    chsnprintf(buffer,sizeof(buffer),"page %s%c%c%c",nombres::nomConId(idPageBlank),0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,buffer);
    chMtxUnlock(&MtxNextionTx);
    idPagActual = idPageBlank;
}

void nextion::setBasePage(void)
{
    char buffer[20];
    contDsInactividad = maxDsInactividad;
    if (idPagActual!=idPageBlank)
        return;
    chsnprintf(buffer,sizeof(buffer),"page base%c%c%c",0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,buffer);
    chsnprintf(buffer,sizeof(buffer),"dim=100%c%c%c",0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,buffer);
    chMtxUnlock(&MtxNextionTx);
}

void nextion::hayActividad(void)
{
    contDsInactividad = maxDsInactividad;
}

uint16_t nextion::diPagActual()
{
    return idPagActual;
}

void nextion::setPagActual(uint16_t idPag)
{
    idPagActual = idPag;
}

void nextion::incDs(void)
{
    if (contDsInactividad>1)
        contDsInactividad--;
    if (contDsInactividad == 1)
    {
        contDsInactividad--;
        setBlankPage();
    }
}

/*
 * base.logTmp.txt=sms.logSms.txt
 * sms.logSms.txt="Bueno \r"+base.logTmp.txt
 */
void nextion::enviaLog(BaseSequentialStream *tty, uint16_t idPageLog, uint16_t idNomLog, const char *msg)
{
    char buffer[135],msgFecha[100];
    if (idNomLog==0 || idPageLog==0 || idNombreTmpNxt==0 || idPageTmpNxt==0 || msg[0]==0)
        return;
    uint16_t ds;
    struct tm ahora;
    if (tty!=NULL)
    {
        chprintf(tty,"%s\r\n", msg);
    }
    rtcGetTM(&RTCD1, &ahora, &ds);
    // base.logTmp.txt=sms.logSms.txt
    chsnprintf(buffer,sizeof(buffer),"%s.%s.txt=%s.%s.txt%c%c%c",nombres::nomConId(idPageTmpNxt), nombres::nomConId(idNombreTmpNxt),
               nombres::nomConId(idPageLog), nombres::nomConId(idNomLog),0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,buffer);
    chMtxUnlock(&MtxNextionTx);
    // sms.logSms.txt="Bueno \r"+base.logTmp.txt
    chsnprintf(msgFecha,sizeof(msgFecha),"%2d/%02d %2d:%02d:%02d %s",
               ahora.tm_mday, ahora.tm_mon+1, ahora.tm_hour, ahora.tm_min, ahora.tm_sec,msg);
    if (strlen(msgFecha)>90)
    {
        strcpy(&msgFecha[86],"...");
        msgFecha[89] = 0;
    }
    chsnprintf(buffer,sizeof(buffer),"%s.%s.txt=\"%s\\r\"+%s.%s.txt%c%c%c",nombres::nomConId(idPageLog), nombres::nomConId(idNomLog),
               msgFecha, nombres::nomConId(idPageTmpNxt), nombres::nomConId(idNombreTmpNxt),0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,buffer);
    chMtxUnlock(&MtxNextionTx);
    chThdSleepMilliseconds(100);
}

void nextion::enviaLog(BaseSequentialStream *tty, campoNextion *campoLog, const char *msg)
{
    nextion::enviaLog(tty, campoLog->diPageNxt(),campoLog->diNombreNxt(),msg);
}

void nextion::enviaLog(BaseSequentialStream *tty, const char *msg)
{
    nextion::enviaLog(tty, idPageLog, idNombreLog, msg);
}
