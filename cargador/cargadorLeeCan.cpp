/*
 * cargadorLeeCan.cpp
 *
 *  Created on: 31 oct. 2021
 *      Author: joaquin
 */

/*
 * cargador.cpp
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "bloques.h"
#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"

extern char tipoMaestroStr[4][10];
extern char tipoControlPStr[4][10];

/*
    uint8_t numCoches;
    float   iMax[MAXCARGADORES];
    uint8_t numFasesMax[MAXCARGADORES];
    uint8_t idCharger[MAXCARGADORES];
    float   setPointP[MAXCARGADORES];
    float   minP[MAXCARGADORES];
    float   maxP[MAXCARGADORES];
    estadoRes_t statusResis[MAXCARGADORES];
    uint8_t tipoControl[MAXCARGADORES];
    uint8_t numFases[MAXCARGADORES];
    float   PCoche[MAXCARGADORES];
 */

uint8_t cargador::buscoCargador(uint8_t id)
{
    if (id==0)
        return 99;
    // miro si ya esta incorporado ese cargador
    for (uint8_t i=0;i<MAXCARGADORES;i++)
        if (cargadorPtr->idCharger[i]==id)
            return i;
    // no existia, busco hueco
    for (uint8_t i=1;i<MAXCARGADORES;i++)
        if (cargadorPtr->idCharger[i]==0)
        {
            cargadorPtr->numCoches++;
            return i;
        }
    return 99; // no hay huecos
}

void cargador::canCambiaID(uint8_t id, uint8_t nuevaID)
{
    char buffer[5];
    uint8_t posCargador = buscoCargador(id);
    if (posCargador==99)
        return;
    cargadorPtr->idCharger[posCargador] = nuevaID;
    limpiaCoche(posCargador);
    chsnprintf(buffer,sizeof(buffer),"ID%d",posCargador+1);
    enviaTxt(cargadorPtr->idPagecoches, buffer, nuevaID);
}

void cargador::canSetConfig(uint8_t id, uint8_t iMaxNueva, uint8_t numFasesMaxNueva)
{
    uint8_t posCargador = buscoCargador(id);
    if (posCargador==99)
        return;
    cargadorPtr->iMax[posCargador] = iMaxNueva;
    cargadorPtr->numFasesMax[posCargador] = numFasesMaxNueva;
}

void cargador::canSetSP(uint8_t idMaestro, uint8_t idDestino, uint16_t potSP)
{
    char buffer[5];
    uint8_t posCargador = buscoCargador(idMaestro);
    if (posCargador==99)
        return;
    uint8_t posCargadorDest = buscoCargador(idDestino);
    if (posCargadorDest==99)
        return;
    // si es otro maestro con ID menor, cambio mi maestro por esclavo
    if ((tipoMaestro_t) cargadorPtr->tipoMaestroSelf==TMMAESTRO && idMaestro<cargadorPtr->idChargerSelf->valor())
    {
        cargadorPtr->tipoMaestroSelf = TMESCLAVO;
        enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "cont1", tipoMaestroStr[cargadorPtr->tipoMaestroSelf]);
        enviaEstadoCargador2can();
    }
    cargadorPtr->setPointP[posCargadorDest] = potSP;
    chsnprintf(buffer,sizeof(buffer),"SP%d",posCargadorDest+1);
    enviaTxt(cargadorPtr->idPagecoches, buffer, potSP);
    // si es para nosotros, actualizo la medida SP
    if (posCargadorDest==0)
    {
        float pCoche = (float) potSP;
        cargadorPtr->medPSetPoint->set(&pCoche,1);
    }
}

extern char tipoMaestroStr[4][10];
extern char tipoControlPStr[4][10];
extern char estResStr[5][10];

void cargador::canSetEstadoEnNextion(uint8_t id, uint8_t estR, uint8_t estContP, uint8_t estMaestro,
                            uint8_t numFasesReal, uint16_t minPot, uint16_t maxPot)
{
    char buffer[8];
    uint8_t posCargador = buscoCargador(id);
    if (posCargador==99)
        return;
    cargadorPtr->statusResis[posCargador] = (estadoRes_t) estR;
    cargadorPtr->tipoControlP[posCargador] = (tipoControlP_t) estContP;
    cargadorPtr->tipoMaestro[posCargador] = (tipoMaestro_t) estMaestro;
    cargadorPtr->numFases[posCargador] = numFasesReal;
    cargadorPtr->minP[posCargador] = minPot;
    cargadorPtr->maxP[posCargador] = maxPot;
    chsnprintf(buffer,sizeof(buffer),"est%d",posCargador+1);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, (char *) estResStr[estR]);
    chsnprintf(buffer,sizeof(buffer),"cont%d",posCargador+1);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, (char *) tipoControlPStr[estContP]);
    chsnprintf(buffer,sizeof(buffer),"mast%d",posCargador+1);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, (char *) tipoMaestroStr[estMaestro]);
    chsnprintf(buffer,sizeof(buffer),"fases%d",posCargador+1);
    enviaTxt(cargadorPtr->idPagecoches, buffer, numFasesReal);
}

void cargador::canSetPVE(uint8_t id, uint16_t potVE)
{
    char buffer[6];
    uint8_t posCargador = buscoCargador(id);
    if (posCargador==99)
        return;
    cargadorPtr->PCoche[posCargador] = potVE;
    chsnprintf(buffer,sizeof(buffer),"PVE%d",posCargador+1);
    enviaTxt(cargadorPtr->idPagecoches, buffer, potVE);
}

void cargador::canSetIVE(uint8_t id, float intenVE)
{
    char buffer[6], buffVal[7];
    uint8_t posCargador = buscoCargador(id);
    if (posCargador==99)
        return;
    cargadorPtr->iVE[posCargador] = intenVE;
    chsnprintf(buffer,sizeof(buffer),"I%d",posCargador+1);
    chsnprintf(buffVal,sizeof(buffVal),"%.1f",intenVE);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, buffVal);
}

void cargador::canSetPCasa(uint16_t potCasa)
{
    float pCasa = (float) potCasa;
    cargadorPtr->medPTotal->set(&pCasa,0);
}

void cargador::canSetPContratadas(uint16_t pPunta, uint16_t pValle)
{
    cargadorPtr->pContratadaPunta->set(pPunta);
    cargadorPtr->pContratadaPunta->enviaToNextion();
    cargadorPtr->pContratadaValle->set(pValle);
    cargadorPtr->pContratadaValle->enviaToNextion();
}


