/*
 * tratamientoMsgRF95.c
 *
 *  Created on: 28/12/2019
 *      Author: jcabe
 */

#include "ch.hpp"
#include "hal.h"

using namespace chibios_rt;

#include <RH_RF95.h>
#include "string.h"
#include "chprintf.h"
#include <stdio.h>
#include "colas.h"
//#include "../radio/pozo.h"
//#include "tipoVars.h"
#include "bloques.h"
#include "nextion.h"
#include "radio.h"

extern struct queu_t colaMsgRx;
extern struct queu_t colaMsgTx;
extern event_source_t newMsgRx_source;
extern event_source_t newMsgTx_source;
//extern RTCDateTime dateTimeEnvioAnterior;
extern uint8_t cnt;
//extern varMODOPOZO modopozo;
extern uint8_t checkTrataMsgRf95;

void initRF95(void);

thread_t *procesoMsgRf95;
extern thread_t *procesoRf95Int;


//void radio::trataRxRf95(eventmask_t evt)
//{
//    struct msgRx_t msgRx;
//    struct msgTx_t msgTx;
//    if (evt==0) // timeout, llamo a rutinas
//    {
//        switch (modoRadio)
//        {
//        case 1:
//            trataObsoletoLlamador();
//            break;
//        case 2:
//            trataObsoletoPozo();
//            break;
//        }
//    }
//    if (evt & EVENT_MASK(0)) // He recibido un mensaje rf95
//    {
//        while (getQueu(&colaMsgRx, &msgRx))
//        {
//            if (++cnt>99) cnt=0;
//            switch (modoRadio)
//            {
//            case 0:
//            case 1:
//                trataRxRegistradoryLlamador(&msgRx);
//                break;
//            case 2:
//                trataRxPozo(&msgRx);
//                break;
//            }
//        }
//    }
//    if (evt & EVENT_MASK(1)) // Tengo que enviar mensajes rf95
//    {
//        while (getQueu(&colaMsgTx, &msgTx))
//        {
//            RH_RF95_send(msgTx.msg,msgTx.numBytes);
//            RHGenericDriver_waitPacketSent(100);
//            RH_RF95_setModeRx();
//        }
//    }
//
//}

/*
 * Trata mensajes recibidos por rf95     (en la colaRx)
 * Envia mensajes que pidan ser enviados (en la colaTx)
 * Llama cada segundo a rutinas para vigilar cambios
 */
static THD_WORKING_AREA(trataMensajesRf95_wa,1200);
static THD_FUNCTION(trataMensajesRf95, p) {
    (void)p;
    event_listener_t newMsgRx_lis, newMsgTx_lis;

    chRegSetThreadName("trataMsgRf95");
    RH_RF95_setModeRx();
    //    msMaxEntreMsgsLlamador = randomNum(100*dsMaxEntreMsgsLlamadorValor()-2000,100*dsMaxEntreMsgsLlamadorValor());
    chEvtRegister(&newMsgRx_source, &newMsgRx_lis, 0);
    chEvtRegister(&newMsgTx_source, &newMsgTx_lis, 1);
    do {
        eventmask_t evt = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100));
        checkTrataMsgRf95 = 1;
        radio::trataRxRf95Radio(evt);
        if (chThdShouldTerminateX())
            chThdExit((msg_t) 1);
    } while (1==1);
}

void radio::paraRadio(void)
{
    if (procesoMsgRf95 != NULL)
    {
        chThdTerminate(procesoMsgRf95);
        chThdWait(procesoMsgRf95);
        procesoMsgRf95 = NULL;
    }
    if (procesoRf95Int != NULL)
    {
        chThdTerminate(procesoRf95Int);
        chThdWait(procesoRf95Int);
        procesoRf95Int = NULL;
        palDisableLineEvent(LINE_LORA_DIO);
        spiStop(&SPID2);
        return;
    }
    //BaseSequentialStream *tty, uint16_t idPageLog, uint16_t idNomLog, const char *msg
    nextion::enviaLog(NULL,"Radio parada");
}

void radio::arrancaRadio(void)
{
    radio::reseteaVariables();
    if (!procesoMsgRf95)
    {
        initRF95();
        procesoMsgRf95 = chThdCreateStatic(trataMensajesRf95_wa, sizeof(trataMensajesRf95_wa), NORMALPRIO,  trataMensajesRf95, NULL);
    }
}

















