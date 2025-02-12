/*
 * colasRegistrador.c
 *
 *  Created on: 6/10/2019
 *      Author: jcabe
 */
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

#define NUMMSGLCDENCOLA  5

mutex_t MtxMsgLcd;

event_source_t hayMsgParaLCD_source;
struct queu_t colaMsgLcd;
struct msgLcd_t msgLcd[NUMMSGLCDENCOLA];

void ponerMsgLcdEnCola(uint16_t pos, void *ptrStructOrigen)
{
    struct msgLcd_t *ptrMsg = (struct msgLcd_t *)ptrStructOrigen;
    strncpy((char *)msgLcd[pos].msg, (char *)ptrMsg->msg,sizeof(msgLcd[pos].msg));
}

void cogerMsgLcdDeCola(uint16_t pos, void *ptrStructDestino)
{
    struct msgLcd_t *ptrMsg = (struct msgLcd_t *)ptrStructDestino;
    strncpy((char *)ptrMsg->msg, (char *)msgLcd[pos].msg,sizeof(ptrMsg->msg));
}

void msgParaLCD(const char *msg,uint16_t msShow)
{
    struct msgLcd_t msgLcd;
    msgLcd.msShow = msShow;
    strncpy((char *)msgLcd.msg, msg, sizeof(msgLcd.msg));
    putQueu(&colaMsgLcd, &msgLcd);
    chEvtBroadcast(&hayMsgParaLCD_source);
}


void initColaMsgLcd(void)
{
    initQueu(&colaMsgLcd, &MtxMsgLcd, NUMMSGLCDENCOLA, ponerMsgLcdEnCola, cogerMsgLcdDeCola);
}
