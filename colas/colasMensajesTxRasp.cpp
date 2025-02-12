/*
 *  Created on: 6/10/2019
 *      Author: jcabe
 */
#include "hal.h"
#include "ch.h"
#include "colas.h"
#include "string.h"

#define NUMMSGTXENCOLA  10

mutex_t MtxMsgTxWWW;
struct queu_t colaMsgTxWWW;
struct msgTxWWW_t msgTxWWW[NUMMSGTXENCOLA];


void ponerMsgTxWWWEnCola(uint16_t pos, void *ptrStructOrigen)
{
    struct msgTxWWW_t *ptrMsg = (struct msgTxWWW_t *)ptrStructOrigen;
    msgTxWWW[pos].idNombVariable = ptrMsg->idNombVariable;
    msgTxWWW[pos].accion = ptrMsg->accion;
    memcpy(msgTxWWW[pos].valor, ptrMsg->valor,13);
}

void cogerMsgTxWWWDeCola(uint16_t pos, void *ptrStructDestino)
{
    struct msgTxWWW_t *ptrMsg = (struct msgTxWWW_t *)ptrStructDestino;
    ptrMsg->idNombVariable = msgTxWWW[pos].idNombVariable;
    ptrMsg->accion = msgTxWWW[pos].accion;
    memcpy(ptrMsg->valor, msgTxWWW[pos].valor,13);
}

void initColaMsgTxWWW(void)
{
    initQueu(&colaMsgTxWWW, &MtxMsgTxWWW, NUMMSGTXENCOLA, ponerMsgTxWWWEnCola, cogerMsgTxWWWDeCola);
}
