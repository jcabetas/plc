/*
 * colasRegistrador.c
 *
 *  Created on: 6/10/2019
 *      Author: jcabe
 */

#include "hal.h"
#include "ch.h"
#include "colas.h"
#include "string.h"

#define NUMMSGTXNEXTIONENCOLA  3

mutex_t MtxMsgTxNextion;

struct queu_t colaMsgTxNextion;
struct msgTx_t msgTxNextion[NUMMSGTXNEXTIONENCOLA];

void ponerMsgTxNextionEnCola(uint16_t pos, void *ptrStructOrigen)
{
    struct msgTxNextion_t *ptrMsg = (struct msgTxNextion_t *)ptrStructOrigen;
    msgTxNextion[pos].numBytes = ptrMsg->numBytes;
    memcpy(msgTx[pos].msg, ptrMsg->msg,ptrMsg->numBytes);
}

void cogerMsgTxNextionDeCola(uint16_t pos, void *ptrStructDestino)
{
    struct msgTxNextion_t *ptrMsg = (struct msgTxNextion_t *)ptrStructDestino;
    ptrMsg->numBytes = msgTx[pos].numBytes;
    memcpy(ptrMsg->msg, msgTx[pos].msg,msgTx[pos].numBytes);
}

void initColaMsgTxNextion(void)
{
    initQueu(&colaMsgTxNextion, &MtxMsgTxNextion, NUMMSGTXNEXTIONENCOLA, ponerMsgTxNextionEnCola, cogerMsgTxNextionDeCola);
}
