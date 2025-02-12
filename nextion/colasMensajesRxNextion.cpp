/*
 * colasNextion.c
 *
 *  Created on: 6/10/2019
 *      Author: jcabe
 */
#include "hal.h"
#include "ch.h"
#include "colas.h"
#include "string.h"

#define NUMMSGRXNEXTIONENCOLA  3

mutex_t MtxMsgRxNextion;

struct queu_t colaMsgRxNextion;
struct msgRxNextion_t msgRxNextion[NUMMSGRXNEXTIONENCOLA];

void ponerMsgRxNextionEnCola(uint16_t pos, void *ptrStructOrigen)
{
    struct msgRxNextion_t *ptrMsg = (struct msgRxNextion_t *)ptrStructOrigen;
    strncpy((char *)msgRxNextion[pos].msg, (char *)ptrMsg->msg,sizeof(msgRxNextion[pos].msg));
}

void cogerMsgRxNextionDeCola(uint16_t pos, void *ptrStructDestino)
{
    struct msgRxNextion_t *ptrMsg = (struct msgRxNextion_t *)ptrStructDestino;
    strncpy((char *)ptrMsg->msg, (char *)msgRxNextion[pos].msg,sizeof(ptrMsg->msg));
}

void initColaMsgRxNextion(void)
{
    initQueu(&colaMsgRxNextion, &MtxMsgRxNextion, NUMMSGRXNEXTIONENCOLA, ponerMsgRxNextionEnCola, cogerMsgRxNextionDeCola);
}
