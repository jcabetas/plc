/*
 * colas.h
 *
 *  Created on: 5 may. 2018
 *      Author: joaquin
 */

#ifndef COLAS_H_
#define COLAS_H_

#include "time.h"

struct queu_t {
    uint8_t numItems;
    uint8_t headQueu; // puntero al hueco
    uint8_t tailQueu; // puntero al primer elemento en cola
    uint8_t sizeQueu; // maximos elementos en cola
    void (*ptrFuncionPonerEnCola)(uint16_t posAGuardar, void *ptrStructOrigen);
    void (*ptrFuncionCogerDeCola)(uint16_t posALeer, void *ptrStructDestino);
    mutex_t *mtxQUEU;
};

struct cambiosPozo_t {
  time_t timet;
  uint8_t estLlam;
  uint8_t estActi;
  uint8_t estAbuso;
  uint8_t estErr;
  float m3Total;
  float kWhPunta;
  float kWhValle;
};

struct msgRx_t {
  time_t timet;
  uint8_t numBytes;
  int16_t rssi;
  uint8_t msg[30];
};

struct msgTx_t {
  uint8_t numBytes;
  uint8_t msg[30];
};

struct msgLcd_t {
    uint16_t msShow;
    uint8_t msg[21];
};

struct msgLog_t {
    time_t timet;
    char msg[80];
};

#define CAN_MEDIDA 1
#define CAN_ESTADO 2
#define CAN_CONFIG 3
#define CAN_SETPNT 4
#define CAN_ESTCHG 5
#define CAN_PHOME  6

struct msgCanTx_t {
    uint8_t msgEID;
    uint8_t msgDLC;
    uint8_t msg[8];
};

#define TXWWWESTADO 1
#define TXWWWESTADOVAR 2
#define TXWWWMEDIDAVAL 3
#define TXWWWZONAACTIVA 4
#define TXWWWTZONA 5
#define TXWWWPROGBOMBA 6
#define TXWWWPROGESPERA 7
#define TXWWWDOW 8
#define TXWWWHSTART 9
#define TXWWWMSTART 10
#define TXWWWESBSTART 11
#define TXCONECTARMQTT 12
#define TXWWWTFLUJOMAX 13

struct msgTxWWW_t {
  uint16_t idNombVariable;
  uint8_t accion;
  char valor[13];
};

//struct msgRxNextion_t {
//  uint8_t numBytes;
//  uint8_t msg[30];
//};
//
//struct msgTxNextion_t {
//  uint8_t numBytes;
//  uint8_t msg[30];
//};

uint8_t putQueu(struct queu_t *colaMed, void *ptrStructOrigen);
uint8_t getQueu(struct queu_t *colaMed, void *ptrStructDestino);
void clearQueu(struct queu_t *cola);


void initQueu(struct queu_t *cola, mutex_t *mutexCola, uint8_t numElementos,
              void (*ptrFuncionPonerEnColaPar)(uint16_t posAGuardar, void *ptrStructOrigen),
              void (*ptrFuncionCogerDeColaPar)(uint16_t posALeer, void *ptrStructDestino));

void ponerCambiosEnCola(uint16_t pos, void *ptrStructOrigen);
void cogerCambiosDeCola(uint16_t pos, void *ptrStructDestino);
void initColaPozo(void);

void ponerMsgRxEnCola(uint16_t pos, void *ptrStructOrigen);
void cogerMsgRxDeCola(uint16_t pos, void *ptrStructDestino);
void initColaMsgRx(void);

void ponerMsgTxEnCola(uint16_t pos, void *ptrStructOrigen);
void cogerMsgTxDeCola(uint16_t pos, void *ptrStructDestino);
void initColaMsgTx(void);

void ponerMsgTxCanEnCola(uint16_t pos, void *ptrStructOrigen);
void cogerMsgTxCanDeCola(uint16_t pos, void *ptrStructDestino);
void initColaMsgTxCan(void);

void ponerMsgLcdEnCola(uint16_t pos, void *ptrStructOrigen);
void cogerMsgLcdDeCola(uint16_t pos, void *ptrStructDestino);
void msgParaLCD(const char *msg,uint16_t msShow);
void initColaMsgLcd(void);

void ponerLogEnCola(uint16_t pos, void *ptrStructOrigen);
void cogerLogDeCola(uint16_t pos, void *ptrStructDestino);
void initColaLog(void);
void registraEnLog(const char *mensaje);

void ponerTxRaspEnCola(uint16_t pos, void *ptrStructOrigen);
void cogerTxRaspDeCola(uint16_t pos, void *ptrStructDestino);
void initColaMsgTxWWW(void);

//void ponerMsgTxNextionEnCola(uint16_t pos, void *ptrStructOrigen);
//void cogerMsgTxNextionDeCola(uint16_t pos, void *ptrStructDestino);
//void initColaMsgTxNextion(void);

#endif /* COLAS_H_ */
