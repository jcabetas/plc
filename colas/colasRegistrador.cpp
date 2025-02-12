/*
 * colasRegistrador.c
 *
 *  Created on: 6/10/2019
 *      Author: jcabe
 */
#include "hal.h"
#include "ch.h"
#include "colas.h"

#define NUMCAMBIOSENCOLA  5


mutex_t MtxCambiosPozo;

struct queu_t colaCambiosPozo;
struct cambiosPozo_t cambiosPozo[NUMCAMBIOSENCOLA];


void ponerCambiosEnCola(uint16_t pos, void *ptrStructOrigen)
{
    struct cambiosPozo_t *ptrCambios = (struct cambiosPozo_t *)ptrStructOrigen;
    cambiosPozo[pos].timet = ptrCambios->timet;
    cambiosPozo[pos].estLlam = ptrCambios->estLlam;
    cambiosPozo[pos].estActi = ptrCambios->estActi;
    cambiosPozo[pos].estAbuso = ptrCambios->estAbuso;
    cambiosPozo[pos].estErr = ptrCambios->estErr;
    cambiosPozo[pos].m3Total = ptrCambios->m3Total;
    cambiosPozo[pos].kWhPunta = ptrCambios->kWhPunta;
    cambiosPozo[pos].kWhValle = ptrCambios->kWhValle;
}

void cogerCambiosDeCola(uint16_t pos, void *ptrStructDestino)
{
    struct cambiosPozo_t *ptrCambios = (struct cambiosPozo_t *)ptrStructDestino;
    ptrCambios->timet = cambiosPozo[pos].timet;
    ptrCambios->estLlam = cambiosPozo[pos].estLlam;
    ptrCambios->estActi = cambiosPozo[pos].estActi;
    ptrCambios->estAbuso = cambiosPozo[pos].estAbuso;
    ptrCambios->estErr = cambiosPozo[pos].estErr;
    ptrCambios->m3Total = cambiosPozo[pos].m3Total;
    ptrCambios->kWhPunta = cambiosPozo[pos].kWhPunta;
    ptrCambios->kWhValle = cambiosPozo[pos].kWhValle;
}

void initColaPozo(void)
{
    initQueu(&colaCambiosPozo, &MtxCambiosPozo, NUMCAMBIOSENCOLA, ponerCambiosEnCola, cogerCambiosDeCola);
}
