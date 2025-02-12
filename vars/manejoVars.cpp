/*
 * manejoVars.cpp
 *
 *  Created on: 1/11/2019
 *      Author: jcabe
 */

#include "ch.hpp"
#include "hal.h"

extern "C" {
  time_t GetTimeUnixSecC(void);
}

using namespace chibios_rt;

#include "lcd.h"
#include "string.h"
#include "chprintf.h"
#include "tipoVars.h"
#include "manejoVars.h"

extern varSTR50 telef1, telef2, telef3, telef4, telef5, telef6, telef7, telef8;
extern varSTR50 nombreAdm, nombre1, nombre2, nombre3, nombre4, nombre5, nombre6, nombre7, nombre8;

using namespace chibios_rt;

extern int32_t contarMin, contarMax;
extern volatile int32_t contar;
//extern event_source_t qeCursorEvent, qeSwitchEvent;
extern event_source_t qeCursorEvent, qeSwitchShortEvent, qeSwitchLongEvent;
int chLcdprintfFila(uint8_t fila, const char *fmt, ...);

void devuelveTlfyNombre(uint8_t numEst, char *tlf, uint8_t sizetlf, char *nmbr, uint8_t sizenmbr)
{
    varSTR50 *telef, *nombre;
    tlf[0] = 0;
    nmbr[0] = 0;
    if (numEst==0 || numEst>8)
        return;
    switch (numEst)
    {
    case 1:
        telef = &telef1;
        nombre = &nombre1;
        break;
    case 2:
        telef = &telef2;
        nombre = &nombre2;
        break;
    case 3:
        telef = &telef3;
        nombre = &nombre3;
        break;
    case 4:
        telef = &telef4;
        nombre = &nombre4;
        break;
    case 5:
        telef = &telef5;
        nombre = &nombre5;
        break;
    case 6:
        telef = &telef6;
        nombre = &nombre6;
        break;
    case 7:
        telef = &telef7;
        nombre = &nombre7;
        break;
    case 8:
        telef = &telef8;
        nombre = &nombre8;
        break;
    }
    strncpy(tlf, telef->getValor(),sizetlf);
    strncpy(nmbr, nombre->getValor(),sizenmbr);
}

uint8_t findEstPorTelefono(char *tlf)
{
    char telef[15], nombre[15];
    for (uint8_t numEst=1;numEst<8;numEst++)
    {
        devuelveTlfyNombre(numEst, telef, sizeof(telef), nombre,sizeof(nombre));
        if (!strncmp(telef,tlf,sizeof(telef)))
            return numEst;
    }
    return 0;
}

int32_t leeNum(int32_t valorPar, int32_t valorMinPar, int32_t valorMaxPar, const char descPar[])
{
    chLcdprintfFila(0,"UxTime:%d",valorPar);

    eventmask_t mask;
    contar = valorPar;
    contarMin = valorMinPar;
    contarMax = valorMaxPar;
    chLcdprintfFila(0,"%s",descPar);
    chLcdprintfFila(1,"");
    chLcdprintfFila(2,"");
    chLcdprintfFila(3,"%d",contar);
    do
    {
        mask = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(10000));
        if (mask==0)
        {
            return valorPar;
        }
        if (mask & EVENT_MASK(0)) // Cursor
        {
            chLcdprintfFila(3,"%d",contar);
        }
        if (mask & EVENT_MASK(1)) // Switch
        {
            return contar;
        }
    } while (TRUE);
}

void printMenuSelsCPP(uint8_t *linIni,variableSelects *varSel)
{
    uint8_t linMax, l, cursor;
    chLcdprintfFila(0,"%s",varSel->getDesc());
    uint8_t posSel = varSel->getValorNum();
    if (posSel>*linIni+2) // se sale por abajo? (p.e. posSel=5, linIni=0 => linIni = 5-3 = 2
        *linIni = posSel-2;
    if (posSel<*linIni)   // o bien por arriba?
        *linIni = posSel;
    linMax = *linIni+2;         // si linIni=5 y numItemsMenu=7, maxLinea=8 (no entra la ultima)
    if (linMax>=varSel->maxValor)   //
        linMax = varSel->maxValor;
    // imprime desde linIni a linMax;
    for (l=*linIni;l<=linMax;l++)
    {
        if (l==posSel)
            cursor = '>';
        else
            cursor = ' ';
        chLcdprintfFila(l-*linIni+1,"%c%s",cursor,varSel->getStr(l));
    }
    // imprime desde linMax al final;
    for (l=linMax-*linIni+1;l<=2;l++)
      chLcdprintfFila(l+1,"");
}

void gestionaMenuSelsCPP(variableSelects *varSel)
{
//    event_listener_t qeCursorLis, qeSwitchLis;
    uint8_t linIni;
    eventmask_t mask;
    contar = varSel->getValorNum();
    contarMin = 0;
    contarMax = varSel->maxValor;
    linIni = 0;
    printMenuSelsCPP(&linIni, varSel);
//    chEvtRegister(&qeCursorEvent, &qeCursorLis, 0);
//    chEvtRegister(&qeSwitchEvent, &qeSwitchLis, 1);
    do
    {
        chEvtGetAndClearEvents(ALL_EVENTS);
        mask = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100000));
        if (mask==0)
        {
//            chEvtUnregister(&qeCursorEvent, &qeCursorLis);
//            chEvtUnregister(&qeSwitchEvent, &qeSwitchLis);
            return;
        }
        if (mask & EVENT_MASK(0)) // Cursor
        {
            varSel->setVal(contar);
            printMenuSelsCPP(&linIni, varSel);
        }
        if (mask & EVENT_MASK(1)) // Switch
        {
//            chEvtUnregister(&qeCursorEvent, &qeCursorLis);
//            chEvtUnregister(&qeSwitchEvent, &qeSwitchLis);
            return;
        }
    } while (TRUE);
}


void printMenuVarsCPP(uint8_t *linIni, uint8_t posSel, uint8_t numItemsMenu, menuItem *menuVars[])
{
    uint8_t linMax, l, cursor;
    if (posSel>=numItemsMenu)
        posSel = numItemsMenu-1;
    if (posSel>*linIni+3) // se sale por abajo? (p.e. posSel=5, linIni=0 => linIni = 5-3 = 2
        *linIni = posSel-3;
    if (posSel<*linIni)   // o bien por arriba?
        *linIni = posSel;
    linMax = *linIni+3;         // si linIni=5 y numItemsMenu=7, maxLinea=8 (no entra la ultima)
    if (linMax>=numItemsMenu)   //
        linMax = numItemsMenu-1;
    // imprime desde linIni a linMax;
    for (l=*linIni;l<=linMax;l++)
    {
        if (l==posSel)
            cursor = '>';
        else
            cursor = ' ';
        chLcdprintfFila(l-*linIni,"%c%s",cursor,menuVars[l]->dispMenu());//->getDesc(),menuVars[l]->getValorStr());
//        chLcdprintfFila(l-*linIni,"%s",menuVars[l]->getDesc());
    }
    // imprime desde linMax al final;
    for (l=linMax-*linIni+1;l<=3;l++)
      chLcdprintfFila(l,"");
}


int8_t gestionaMenuVarsCPP(uint8_t numItemsMenu, menuItem *menuVars[])
{
    uint8_t linIniVars, contarVars;
    event_listener_t qeCursorLis, qeSwitchShortEventLis, qeSwitchLongEventLis;
    eventmask_t mask;
    linIniVars = 0;
    contarVars = 0;
    contar = linIniVars;
    chEvtRegister(&qeCursorEvent, &qeCursorLis, 0);
    chEvtRegister(&qeSwitchShortEvent, &qeSwitchShortEventLis, 1);
    chEvtRegister(&qeSwitchLongEvent, &qeSwitchLongEventLis, 2);
    do
    {
        contarMin = 0;
        contarMax = numItemsMenu-1;
        printMenuVarsCPP(&linIniVars, contar, numItemsMenu, menuVars);
//        chEvtGetAndClearFlags(&qeCursorLis);
//        chEvtGetAndClearFlags(&qeSwitchLis);
        mask = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100000));
        if (mask==0)
        {
            chEvtUnregister(&qeCursorEvent, &qeCursorLis);
            chEvtUnregister(&qeSwitchShortEvent, &qeSwitchShortEventLis);
            chEvtUnregister(&qeSwitchLongEvent, &qeSwitchLongEventLis);
            return -1;
        }
        if (mask & EVENT_MASK(0)) // Cursor
        {
            //printMenuVarsCPP(contar, numItemsMenu, menuVars);
        }
        if (mask & EVENT_MASK(1)) // short Switch
        {
//            chEvtUnregister(&qeCursorEvent, &qeCursorLis);
//            chEvtUnregister(&qeSwitchEvent, &qeSwitchLis);
            contarVars = contar;
            menuVars[contarVars]->clickMenu();//gestionaMenuSelsCPP(menuVars[contar]);
            contar = contarVars;
            chEvtGetAndClearEvents(ALL_EVENTS);
//            chEvtRegister(&qeCursorEvent, &qeCursorLis, 0);
//            chEvtRegister(&qeSwitchEvent, &qeSwitchLis, 1);
        }
        if (mask & EVENT_MASK(2)) // Long Switch
        {
            chEvtUnregister(&qeCursorEvent, &qeCursorLis);
            chEvtUnregister(&qeSwitchShortEvent, &qeSwitchShortEventLis);
            chEvtUnregister(&qeSwitchLongEvent, &qeSwitchLongEventLis);
            return 0;
        }
    } while (TRUE);
}





