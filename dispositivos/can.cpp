/*
 * can.cpp
 */
#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"
#include "tty.h"
#include "colas.h"


uint8_t can::definido = 0;
uint8_t can::idCan = 1;
can *can::canPtr = NULL;
thread_t *can::procesoCANRX = NULL;
thread_t *can::procesoCANTX = NULL;
medida  *can::can2med[MAXCAN2MED] = {0};
uint16_t can::idCan2Med[MAXCAN2MED] = {0};
uint8_t  can::numCan2Med = 0;
uint16_t can::can2state[MAXCAN2STAT] = {0};
uint16_t can::idCan2state[MAXCAN2STAT] = {0};
uint16_t can::numCan2state = 0;
uint16_t can::state2can[MAXSTAT2CAN] = {0};
uint16_t can::idState2can[MAXSTAT2CAN] = {0};
uint16_t can::numState2can = 0;

event_source_t sendMsgCAN_source;
extern struct queu_t colaMsgTxCan;

extern "C"
{
    void testTxCAN(void);
    void testRxCAN(void);
}
// ver https://github.com/ChibiOS/ChibiOS/blob/master/testhal/STM32/STM32H7xx/CAN/main.c
//ChibiOS/testhal/STM32/STM32H7xx/CAN/main.c



/*
 * Internal loopback mode, 500KBaud (BRP=6) ahora 100 kBaud (BTR=34), automatic wakeup, automatic recover
 * from abort mode.
 * See section 22.7.7 on the STM32 reference manual.
 *
 * Formato:
 * - SID: id estacion
 * - Data
 *   Byte 1: tipo de mensaje (1: medida, 2: estado, 3: confCargador, 4: fijarSP,
 *                            5: estado, 6: potVE, 7: potCasa)
 *      Tipo 1:
 *          Byte 2..5 valor float
 *          Byte 6..7 idMedCan
 *      Tipo 2:
 *          Byte 2    valor uint8_t
 *          Byte 3..4 IdStateCan
 *      Tipo 3 (config Cargador):
 *          Byte 2: Imax
 *          Byte 3: NumFasesMax
 *      Tipo 4 (fijar SP):
 *          Byte 2: iD estación destino
 *          Byte 3..4: potencia objetivo en W
 *      Tipo 5 (estado cargador):
 *          Byte 2: estado R coche conectado (0: desconectado, 1, conectado, 2: pide, 3:ventilacion)
 *          Byte 3: estado control (0: fijo, 1: esclavo, 2: maestro)
 *          Byte 4: numFasesReal
 *          Byte 5..6: potencia minima en W
 *          Byte 7..8: potencia maxima en W
 *      Tipo 6 (potencia VE consumida en W)
 *          Byte 2..3: potencia consumida por VE
 *      Tipo 7 (medida de casa en W)
 *          Byte 2..3: potencia de casa en W
 */

// mis calculos 500 kB (APB=42 MHz)
//static const CANConfig cancfg = {
//  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
//  CAN_BTR_SJW(1) | CAN_BTR_TS2(1) |
//  CAN_BTR_TS1(10) | CAN_BTR_BRP(5)
//};
static const CANConfig cancfg = {
  51U | 10U<<8 | 7U<<16, //  FDCAN_CONFIG_NBTP_NTSEG2(51U) |  FDCAN_CONFIG_NBTP_NTSEG1(10U) |  FDCAN_CONFIG_NBTP_NBRP(7U),      /* NBTP */
  3U | 3U<<4 | 10U<<8 | 7U<<16, // FDCAN_CONFIG_DBTP_DSJW(3U) |  FDCAN_CONFIG_DBTP_DTSEG2(3U) |  FDCAN_CONFIG_DBTP_DTSEG1(10U) |  FDCAN_CONFIG_DBTP_DBRP(7U),      /* DBTP */
  0,                               /* CCCR */
  0,                               /* TEST */
  0                                /* GFC */
};
//// mis calculos 125 kB (APB=42 MHz)
//static const CANConfig cancfg = {
//  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
//  CAN_BTR_SJW(1) | CAN_BTR_TS2(1) |
//  CAN_BTR_TS1(12) | CAN_BTR_BRP(20)
//};



/*
 * Receiver thread.
 */
static THD_WORKING_AREA(can_rx_wa, 750);
static THD_FUNCTION(can_rx, p) {
//  event_listener_t el;
  CANRxFrame rxmsg;
  msg_t result;
  (void)p;
  chRegSetThreadName("canRx");
  // no acaba de funcionar chEvtRegister(&CAND1.rxfull_event, &el, EVENT_MASK(0));
  //  msg_t result;
  //eventmask_t evt;
  while(true) {
    result = canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_MS2I(100));
    if (result==MSG_OK)
    {
        can::leeMsg(rxmsg);
    }
    else
        if (chThdShouldTerminateX())
            {
            chThdExit((msg_t) 1);
            }
//    evt = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100));
//    if (chThdShouldTerminateX())
//    {
//        chEvtUnregister(&CAND1.rxfull_event, &el);
//        chThdExit((msg_t) 1);
//    }
//    if (evt & EVENT_MASK(0))
//    {
//        while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK) {
//            /* Process message.*/
//            can::leeMsg(rxmsg);
//        }
//    }
  }
}


/*
 * Transmitter thread.
 */
static THD_WORKING_AREA(can_tx_wa, 800);
static THD_FUNCTION(can_tx, p) {
  (void)p;
  event_listener_t nuevaMedida_listener;
  eventmask_t evt;

  chRegSetThreadName("canTx");
  chEvtRegisterMask(&sendMsgCAN_source, &nuevaMedida_listener,EVENT_MASK(0));

  while (true) {
	evt = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(500));
    if (chThdShouldTerminateX())
    {
        chEvtUnregister(&sendMsgCAN_source, &nuevaMedida_listener);
        chThdExit((msg_t) 1);
    }
	if (evt & EVENT_MASK(0))
	{
	    can::sendMsg();
	}
  }
}

uint8_t can::sendMsg(void)
{
    uint8_t exito;
    msg_t result;
    CANTxFrame txmsg;
    struct msgCanTx_t ptrDatTx;
    exito = getQueu(&colaMsgTxCan, &ptrDatTx);
    if (!exito)
        return 1;

    txmsg.ext.EID = ptrDatTx.msgEID;
    txmsg.common.XTD = 1;  /* Extended ID. */
    txmsg.DLC =  ptrDatTx.msgDLC;
    txmsg.FDF = 0;         /* CAN frame format. */
    memcpy(txmsg.data8,ptrDatTx.msg, txmsg.DLC);
    txmsg.ext.EID = ptrDatTx.msgEID;
    txmsg.DLC = ptrDatTx.msgDLC;
    memcpy(txmsg.data8,ptrDatTx.msg, txmsg.DLC);
    result = canTransmitTimeout(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
    if (result==MSG_OK)
        return 0;
    else
        return 1;
}

uint8_t can::med2msg(uint16_t idMedCan, float *valorPtr)
{
    uint8_t error;
    /*
     * struct msgCanTx_t {
    uint8_t msgEID;
    uint8_t msgDLC;
    uint8_t msg[8];
     */
    /*
    *   Byte 1: tipo de mensaje (2: can2med, 4: can2state)
    *      Tipo 2:
    *          Byte 2..5 valor float
    *          Byte 6..7 idMedCan
    *      Tipo 4:
    *          Byte 2    valor uint8_t
    *          Byte 3..4 IdStateCan
    */
    struct msgCanTx_t message;
    message.msgEID = cargador::diId();
    message.msgDLC = 7;
    message.msg[0] = medidaCan;  //2; // can2med
    memcpy(&message.msg[1],valorPtr, 4);
    memcpy(&message.msg[5],&idMedCan, 2);
    error = putQueu(&colaMsgTxCan, &message);
    chEvtBroadcast(&sendMsgCAN_source);
    return error;
}


// CANBUS 1 [identificador]
can::can(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=2)
    {
        if (tty!=NULL)
            chprintf(tty,"#parametros incorrecto CANBUS\n\r");
        else
            nextion::enviaLog(tty,"#parametros CANBUS");
        *hayError = 1;
        return; // error
    }
    if (definido)
    {
        nextion::enviaLog(tty,"#CANBUS repetido");
        *hayError = 1;
    }
    idCan = atoi(pars[1]);
    canPtr = this;
    procesoCANRX = NULL;
    procesoCANTX = NULL;
    numCan2Med = 0;
    numCan2state = 0;
    numState2can = 0;
    definido = 1;
    initColaMsgTxCan();
    palSetLineMode(LINE_CANSILENT, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(LINE_CANSILENT);
};


uint8_t can::attachCan2Med(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    char buffer[50];
    /*
     * Cuando llegue una medida por CAN, tiene que llamar a la medida para poner valor
     * CAN2MED 22 NivelGasoil
     */
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros CAN2MED");
        *hayError = 1;
        return 1; // error
    }
    if (numCan2Med>=MAXCAN2MED)
    {
        nextion::enviaLog(tty,"#demasiados CAN2MED");
        *hayError = 2;
        return 2; // error
    }
    idCan2Med[numCan2Med] = atoi(pars[1]);
    // se supone que la medida existe
    can2med[numCan2Med] = medida::findMedida(pars[2]);
    if (can2med[numCan2Med]==NULL)
    {
        chsnprintf(buffer,sizeof(buffer),"No encuentro medida %s",pars[2]);
        nextion::enviaLog(tty,buffer);
        *hayError = 3;
        return 3; // error
    }
    numCan2Med++;
    return 0;
}

uint8_t can::attachCan2State(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    /*
     * Cuando llegue un estado por CAN, tiene que actualizar el estado
     * CAN2STATE 22 bombaOn
     */
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros CAN2STATE");
        *hayError = 1;
        return 1; // error
    }
    if (numCan2state>=MAXCAN2STAT)
    {
        nextion::enviaLog(tty,"#demasiados CAN2STATE");
        *hayError = 2;
        return 2; // error
    }
    idCan2state[numCan2state] = atoi(pars[1]);
    can2state[numCan2state] = estados::addEstado(tty, pars[2], 1, hayError);
    numCan2state++;
    return 0;
}


uint8_t can::attachState2Can(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    /*
     * Cuando cambie un estado,lo pasa al CAN
     * STATE2CAN bombaOn 22
     * uint16_t can::state2can[MAXSTAT2CAN] = {0};
uint16_t can::idState2can[MAXSTAT2CAN] = {0};
uint16_t can::numState2can = 0;
     */
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros STATE2CAN");
        *hayError = 1;
        return 1; // error
    }
    if (numState2can>=MAXSTAT2CAN)
    {
        nextion::enviaLog(tty,"demasiados STATE2CAN");
        *hayError = 2;
        return 2; // error
    }
    state2can[numState2can] = estados::addEstado(tty, pars[1], 0, hayError);
    idState2can[numState2can] = atoi(pars[2]);
    numState2can++;
    return 0;
}


//uint8_t can::attachMed2Can(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
//{
//    /*
//     * Cuando llegue una nueva medida, llamar a can::transmit
//     * MED2CAN NivelGasoil 22
//     */
//    if (numPar!=3)
//    {
//        nextion::enviaLog(tty,"#parametros MED2CAN");
//        *hayError = 1;
//        return 1; // error
//    }
//    if (numCan2Med>=MAXCAN2MED)
//    {
//        nextion::enviaLog(tty,"#demasiados MED2CAN");
//        *hayError = 2;
//        return 2; // error
//    }
//    /*
//     *      static uint16_t  *med2can[MAXMED2CAN];
//            static uint16_t idMed2Can[MAXMED2CAN];
//            static uint8_t  numMed2Can;
//     */
//    nomMed2Can[numMed2Can] = nombres::incorpora(pars[1]);
//    idMed2Can[numMed2Can] = atoi(pars[2]);
//    numMed2Can++;
//    return 0;
//}

int8_t can::init(void)
{
    if (definido && canPtr!=NULL && procesoCANRX==NULL && procesoCANTX==NULL)
    {
        uint8_t error = med2can::initMed2Can();
        if (error)
            return 1;
        initColaMsgTxCan();
        palSetLineMode(LINE_CANSILENT, PAL_MODE_OUTPUT_PUSHPULL);
        palClearLine(LINE_CANSILENT);
        canStart(&CAND1, &cancfg);
        if (!procesoCANRX)
            procesoCANRX = chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 7,  can_rx, NULL);
        if (!procesoCANTX)
            procesoCANTX = chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7,  can_tx, NULL);
        return 0;
    }
    return 1;
}

void can::stop(void)
{
    if (procesoCANRX!=NULL)
    {
        chThdTerminate(procesoCANRX);
        chThdWait(procesoCANRX);
        procesoCANRX = NULL;
    }
    if (procesoCANTX!=NULL)
    {
        chThdTerminate(procesoCANTX);
        chThdWait(procesoCANTX);
        procesoCANTX = NULL;
    }
    canStop(&CAND1);
}

can::~can()
{
    stop();
    numCan2Med = 0;
    numCan2state = 0;
    numState2can = 0;
    definido = 0;
}

const char *can::diTipo(void)
{
    return "can";
}

const char *can::diNombre(void)
{
    return "CAN";
}



void can::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  )
{
}

void can::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"CAN #%d",idCan);
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}


// envia lo medida 33.0f por CAN. IdCan = 21;
/*
 *     *   Byte 1: tipo de mensaje (2: can2med, 4: can2state)
    *      Tipo 2:
    *          Byte 2..5 valor float
    *          Byte 6..7 idMedCan
 */
void testTxCAN(void)
{
  float valor;
  CANTxFrame txmsg;
  uint16_t idMed;
  char buffer[30];
  palSetLineMode(LINE_CANSILENT, PAL_MODE_OUTPUT_PUSHPULL);
  palClearLine(LINE_CANSILENT);
  nextion::enviaLog(NULL,buffer);
//  txmsg.IDE = CAN_IDE_STD;
//  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 7;
  canStart(&CAND1, &cancfg);
  valor = 33.0f;
  idMed = 21;
  do
  {
    txmsg.std.SID = 2;
    // txmsg.data32[0] = valor;
    txmsg.data8[1] = 2; // tipo can2med
    memcpy(&txmsg.data8[1],(void *) &valor, 4);
    memcpy(&txmsg.data8[5],(void *) &idMed, 2);
    canTransmitTimeout(&CAND1, CAN_ANY_MAILBOX, &txmsg, TIME_MS2I(100));
    valor += 0.5f;
    osalThreadSleepMilliseconds(100);
  } while (TRUE);
}

void testRxCAN(void)
{
  CANRxFrame rxmsg;
  msg_t result;
  palSetLineMode(LINE_CANSILENT, PAL_MODE_OUTPUT_PUSHPULL);
  palClearLine(LINE_CANSILENT);
  canStart(&CAND1, &cancfg);
  do
  {
      result = canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_MS2I(1000));
      if (result==MSG_OK)
      {
          nextion::enviaLog(NULL,"Recibido CAN");
          can::leeMsg(rxmsg);
      }
  } while (TRUE);
}

