/*
 * pantallaNextion.cpp
 */

#include "ch.hpp"
#include "hal.h"


using namespace chibios_rt;

#include "string.h"
#include "chprintf.h"
//#include "lcd.h"
#include "colas.h"
//#include "manejoVars.h"
#include "nextion.h"
#include "parametros.h"
#include "bloques.h"
#include "dispositivos.h"

#include <stdio.h>
#include <stdlib.h>


extern "C"
{
    void enviaLogC(const char *valor);
}

void parseStr(char *cadena,char **parametros, const char *tokens,uint16_t *numParam);
void divideString(BaseSequentialStream *tty, char *buff, uint8_t *numPar, char *par[]);
//void divideEnPageyVarNextion(char *param, char **page, char **var);
void actualizaPozoConfig(void);
void initPlc(BaseSequentialStream *tty);
void GetTimeTm(struct tm *timp, uint16_t *ds);


extern mutex_t MtxNextionTx;
extern uint8_t statusNextion;
//extern varNOSI bloqueoAbusones;
//extern varNUMERO tiempoAbuso;
//extern varNUMERO sOlvido;
//extern varNUMERO dsMaxEntreMsgsPozo;
//extern varNUMERO parametroFrecuenciaMB;
//extern varNUMERO parametroPresionMB;
//extern varNUMERO parametroEstadoMB;

uint8_t nextionReset[6] = {0,0,0,0xFF,0xFF,0xFF};
uint8_t nextionSleep[4] =   {0x86,0xFF,0xFF,0xFF};
uint8_t nextionWakeUp[4] =  {0x87,0xFF,0xFF,0xFF};
uint8_t nextionReady[4]  =  {0x88,0xFF,0xFF,0xFF};
uint8_t nextionUpgrade[4] = {0x89,0xFF,0xFF,0xFF};

uint16_t tAbuso=300,IDBar=10121,IDkWh=20320;
uint8_t noAbusar=1;

extern event_source_t testMB_source;

uint8_t vectoresIguales(uint8_t *vector1, uint8_t *vector2, uint8_t numBytes)
{
    for (uint8_t i=0;i<numBytes;i++)
        if (vector1[i]!=vector2[i])
            return 0;
    return 1;
}

uint8_t endWithTres0xFF(uint8_t *vector,  uint8_t numBytes)
{
    if (numBytes<=3)
        return 0;
    for (uint8_t i=numBytes-3;i<numBytes;i++)
        if (vector[i]!=0xFF)
            return 0;
    return 1;
}

//void trataMsgInternoNextion(uint8_t *msgRx, uint8_t numBytes)
//{
//    if (numBytes==9 && msgRx[0]==0x68)
//        chLcdprintfFila(0,"nextion touch");
//    if (numBytes==6 && msgRx[0]==0 && msgRx[1]==0 && msgRx[0]==2)
//        chLcdprintfFila(0,"nextion reset");
//    if (numBytes==4 && msgRx[0]==0x86)
//        chLcdprintfFila(0,"nextion sleep");
//    if (numBytes==4 && msgRx[0]==0x87)
//        chLcdprintfFila(0,"nextion WakeUp");
//    if (numBytes==4 && msgRx[0]==0x88)
//        chLcdprintfFila(0,"nextion Ready");
//    if (numBytes==4 && msgRx[0]==0x89)
//        chLcdprintfFila(0,"nextion Upgrade");
//    if (numBytes==4 && msgRx[0]==0x89)
//        chLcdprintfFila(0,"nextion Upgrade");
//}

//void enviaEstado(const char *pageName, const char *varName, uint16_t valor)
//{
//    chMtxLock(&MtxNextionTx);
//    chprintf((BaseSequentialStream *)&SD1,"%s.%s.val=%d%c%c%c",pageName,varName,valor,0xFF,0xFF,0xFF);
//    chMtxUnlock(&MtxNextionTx);
//}
//
//void enviaEstado(uint16_t idPage, uint16_t idVar, uint16_t valor)
//{
//    chMtxLock(&MtxNextionTx);
//    chprintf((BaseSequentialStream *)&SD1,"%s.%s.val=%d%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
//    chMtxUnlock(&MtxNextionTx);
//}


//char logtxt[60];

//base.logTmp.txt=sms.logSms.txt
//sms.logSms.txt="Bueno \r"+base.logTmp.txt
//void enviaLog(BaseSequentialStream *tty, const char *valor, uint16_t msWait)
//{
//    char buffer[60];
//    uint16_t ds;
//    struct tm ahora;
//    if (tty!=NULL)
//    {
//        chprintf(tty,"%s\r\n", valor);
//    }
//
//    linHeadLog++;
//    if (linHeadLog>4)
//        linHeadLog=0;
//    GetTimeTm(&ahora,&ds);
//    chsnprintf(logtxt[linHeadLog],sizeof(logtxt[linHeadLog]),"%2d/%2d %2d:%2d:%2d %s",
//               ahora.tm_mday, ahora.tm_mon+1, ahora.tm_hour, ahora.tm_min, ahora.tm_sec,valor);
//    //strncpy(logtxt[linHeadLog],valor,sizeof(logtxt[linHeadLog]));
//    if (numLineasLog<5)
//        numLineasLog++;
//    chMtxLock(&MtxNextionTx);
//    uint8_t pos = linHeadLog;
//    for (uint8_t i=1;i<=numLineasLog;i++)
//    {
//        if (i==1)
//        {
//            chsnprintf(buffer,sizeof(buffer),"PLC.log.txt=\"%s\"%c%c%c",logtxt[pos],0xFF,0xFF,0xFF);
//            chprintf((BaseSequentialStream *)&SD1,buffer);
//            //chprintf((BaseSequentialStream *)&SD1,"riego.log.txt=\"%s\"%c%c%c",logtxt[pos],0xFF,0xFF,0xFF);
//        }
//        else
//            chprintf((BaseSequentialStream *)&SD1,"PLC.log.txt+=\"\\r%s\"%c%c%c",logtxt[pos],0xFF,0xFF,0xFF);
//        if (pos==0)
//            pos = 4;
//        else
//            pos--;
//    }
//    chMtxUnlock(&MtxNextionTx);
//    if (msWait>0)
//        chThdSleepMilliseconds(msWait);
//}

void selPage(const char *pagTxt)
{
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,"page %s%c%c%c",pagTxt,0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}

void enviaLogC(const char *valor)
{
    nextion::enviaLog(NULL,valor);
}

void enviaTxt(const char *pageName, const char *varName, const char *valor)
{
    if (strlen(pageName)==0)
        return;
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.txt=\"%s\"%c%c%c",pageName,varName,valor,0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.pco=0%c%c%c",pageName,varName,0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}

void enviaTxtSiEnPage(uint16_t idPage, const char *varName, const char *valor)
{
    if (idPage != nextion::diPagActual())
        return;
    enviaTxt(nombres::nomConId(idPage), varName, valor);
}

void enviaTxt(uint16_t idPage, uint16_t idVar, const char *valor)
{
    char buffer[80];
    if (idPage==0)
        return;
    chsnprintf(buffer,sizeof(buffer),"%s.%s.txt=\"%s\"%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.txt=\"%s\"%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.pco=0%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}

void enviaTxt(uint16_t idPage, const char *varName, uint16_t valor)
{
    char buffer[80];
    if (idPage==0)
        return;
    chsnprintf(buffer,sizeof(buffer),"%s.%s.txt=\"%d\"%c%c%c",nombres::nomConId(idPage),varName,valor,0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.txt=\"%d\"%c%c%c",nombres::nomConId(idPage),varName,valor,0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.pco=0%c%c%c",nombres::nomConId(idPage),varName,0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}

void ponColorEnTexto(uint16_t idPage, const char *varName, uint16_t color)
{
    char buffer[80];
    if (idPage==0)
        return;
    chsnprintf(buffer,sizeof(buffer),"%s.%s.pco=%d%c%c%c",nombres::nomConId(idPage),varName,color,0xFF,0xFF,0xFF);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.pco=%d%c%c%c",nombres::nomConId(idPage),varName,color,0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}

void enviaTxtSiEnPage(uint16_t idPage, uint16_t idVar, const char *valor)
{
    if (idPage != nextion::diPagActual())
        return;
    enviaTxt(idPage, idVar, valor);
}


void enviaTxt(uint16_t idPage, uint16_t idVar, uint16_t valor)
{
    char buffer[70];
    if (idPage==0)
        return;
    chMtxLock(&MtxNextionTx);
    chsnprintf(buffer,sizeof(buffer),"%s.%s.txt=\"%d\"%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.txt=\"%d\"%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.pco=0%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}


void enviaPic(uint16_t idPage, uint16_t idVar, uint16_t valor)
{
    char buffer[50];
    if (idPage==0)
        return;
    chsnprintf(buffer,sizeof(buffer),"%s.%s.pic=%d",nombres::nomConId(idPage),nombres::nomConId(idVar),valor);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.pic=%d%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}


void enviaPicc(uint16_t idPage, uint16_t idVar, uint16_t valor)
{
    char buffer[50];
    if (idPage==0)
        return;
    chsnprintf(buffer,sizeof(buffer),"%s.%s.picc=%d",nombres::nomConId(idPage),nombres::nomConId(idVar),valor);
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.picc=%d%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}

void enviaVal(const char *pageName, const char *varName, uint16_t valor)
{
    if (strlen(pageName)==0)
        return;
    chMtxLock(&MtxNextionTx);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.val=%d%c%c%c",pageName,varName,valor,0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.pco=0%c%c%c",pageName,varName,0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}

void enviaVal(uint16_t idPage, uint16_t idVar, uint16_t valor)
{
    char buff[60];
    if (idPage==0)
        return;
    chMtxLock(&MtxNextionTx);
    chsnprintf(buff,sizeof(buff),"%s.%s.val=%d%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.val=%d%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),valor,0xFF,0xFF,0xFF);
    chprintf((BaseSequentialStream *)&SD1,"%s.%s.pco=0%c%c%c",nombres::nomConId(idPage),nombres::nomConId(idVar),0xFF,0xFF,0xFF);
    chMtxUnlock(&MtxNextionTx);
}


void enviaValPic(uint16_t idPage, uint16_t idVar, uint8_t tipoNextion, uint8_t picBase, uint16_t valor)
{
    if (idPage==0 || tipoNextion==TIPONEXTIONSILENCIO)
        return;
    if (tipoNextion==TIPONEXTIONNUMERO)
        enviaVal(idPage, idVar, valor);
    else if (tipoNextion==TIPONEXTIONGRAFICO)
        enviaPic(idPage, idVar, picBase + valor);
    else if (tipoNextion==TIPONEXTIONCROP)
        enviaPicc(idPage, idVar, picBase + valor);
}


void trataRxNextion(uint8_t *msgRx, uint8_t numBytes)
{
    parametroFlash *paramFlash;
    char *params[10], *pagePtr, *varPtr;
    uint16_t numParam, idNextionPage, IdNextionVar, IdNextionVarWWW;
    uint8_t tipoNextion, picBase;
    if (numBytes==0)
        return;
//    if (endWithTres0xFF(msgRx,numBytes))
//        trataMsgInternoNextion(msgRx,numBytes);
    if (msgRx[0]=='@')
    {
        nextion::hayActividad();
        //divideString(NULL, (char *)msgRx, &numParam, params);
        parseStr((char *)msgRx,params, ", ",&numParam);
        // @set,test.tRetardo,18
        if (numParam==3 && !strcmp("@set",params[0]))
        {
            hallaNombreyDatosNextion(params[1], TIPONEXTIONSILENCIO, &idNextionPage, &IdNextionVar, &IdNextionVarWWW, &tipoNextion, &picBase);
            paramFlash = parametroFlash::findAndSet((const char *)nombres::nomConId(IdNextionVar), (const char *)params[2]);
            if (paramFlash!=NULL)
            {
                paramFlash->enviaToNextion();
            }
        }
        if (numParam==1 && !strcmp("@init",params[0]))
        {
            //parametroFlash::enviaTodoANextion();
        }
        // @iniciaADC
        if (numParam==1 && !strcmp("@iniciaADC",params[0]))
        {
            cargador::iniciaADC();
        }
        // @enviaHistADC
        if (numParam==1 && !strcmp("@enviaHistADC",params[0]))
        {
            cargador::enviaHistADC();
        }
        //@page,pozo
        if (numParam==2 && !strcmp("@page",params[0]))
        {
            nextion::setPagActual(nombres::incorpora(params[1]));
            //parametroFlash::enviaPaginaANextion(params[1]);
        }
        if (numParam>2 && !strcmp("@orden",params[0]))
        {
            bloque::ordenNextionBlq(params,numParam);
        }
        // @arrancaPLC,0
        if (numParam==2 && !strcmp("@arrancaPLC",params[0]))
        {
            initPlc(NULL);
        }        // @setEstado,riego.suspender,valor (0,1)
        if (numParam==3 && !strcmp("@setEstado",params[0]))
        {
  //          hallaNombreyDatosNextion(params[1], &idNextionPage, &IdNextionVar,&tipoNextion, &picBase);
            divideEnPageyVarNextion(params[1], &pagePtr, &varPtr);
            if (varPtr!=NULL)
            {
                uint16_t idEstado = estados::findIdEstado(varPtr);
                estados::ponEstado(idEstado,atoi(params[2]));
            }
        }
        // @testMB,modbusPage,outTxt,9600,1,1,88,22,00,BB\x00
        if (numParam>4 && !strcmp("@testMB",params[0]))
        {
            // mete los datos en la clase testMB
            testMB::leeDatosNextion(numParam, params);
            // activa a modbus para que lo lea
            chEvtBroadcast(&testMB_source);
        }
    }
}


