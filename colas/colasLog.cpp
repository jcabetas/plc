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
#include "chprintf.h"
#include "calendar.h"

#define NUMLOGENCOLA  3

mutex_t MtxLog;

struct queu_t colaLog;
struct msgLog_t msgLog[NUMLOGENCOLA];
event_source_t registraLog_source;
time_t GetTimeUnixSec(void);
void registraEnLog(const char *mensaje);

extern "C" {
    void registraEnLogC(const char *mensaje);
}

void registraEnLogC(const char *mensaje)
{
    registraEnLog(mensaje);
}

void registraEnLog(const char *mensaje)
{
    struct msgLog_t msgLog;
    msgLog.timet = calendar::getSecUnix();
    chsnprintf(msgLog.msg,sizeof(msgLog.msg),"%s",mensaje);
    msgLog.msg[sizeof(msgLog.msg)-1] = 0;
    putQueu(&colaLog,&msgLog);
    chEvtBroadcast(&registraLog_source);
}

void ponerLogEnCola(uint16_t pos, void *ptrStructOrigen)
{
    struct msgLog_t *ptrMsg = (struct msgLog_t *)ptrStructOrigen;
    msgLog[pos].timet = ptrMsg->timet;
    strncpy(msgLog[pos].msg, ptrMsg->msg,sizeof(msgLog[pos].msg));
}

void cogerLogDeCola(uint16_t pos, void *ptrStructDestino)
{
    struct msgLog_t *ptrMsg = (struct msgLog_t *)ptrStructDestino;
    ptrMsg->timet = msgLog[pos].timet;
    strncpy(ptrMsg->msg, msgLog[pos].msg,sizeof(ptrMsg->msg));
}

void initColaLog(void)
{
    initQueu(&colaLog, &MtxLog, NUMLOGENCOLA, ponerLogEnCola, cogerLogDeCola);
}
