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
#include "dispositivos.h"

#define NUMMSGTXENCOLA  5

mutex_t MtxMsgTxCan;
struct queu_t colaMsgTxCan;
struct msgCanTx_t msgTxCan[NUMMSGTXENCOLA];

void ponerMsgTxCanEnCola(uint16_t pos, void *ptrStructOrigen)
{
    struct msgCanTx_t *ptrMsg = (struct msgCanTx_t *)ptrStructOrigen;
    msgTxCan[pos].msgEID = ptrMsg->msgEID;
    msgTxCan[pos].msgDLC = ptrMsg->msgDLC;
    memcpy(msgTxCan[pos].msg, ptrMsg->msg,8);
}

void cogerMsgTxCanDeCola(uint16_t pos, void *ptrStructDestino)
{
    struct msgCanTx_t *ptrMsg = (struct msgCanTx_t *)ptrStructDestino;
    ptrMsg->msgEID = msgTxCan[pos].msgEID;
    ptrMsg->msgDLC = msgTxCan[pos].msgDLC;
    memcpy(ptrMsg->msg, msgTxCan[pos].msg,8);
}

void initColaMsgTxCan(void)
{
    initQueu(&colaMsgTxCan, &MtxMsgTxCan, NUMMSGTXENCOLA, ponerMsgTxCanEnCola, cogerMsgTxCanDeCola);
}
