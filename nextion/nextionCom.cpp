/*
 * nextionCom.cpp
 *
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;

#include "chprintf.h"
#include "tty.h"
#include "gets.h"
#include "string.h"
//#include "lcd.h"
#include "manejoVars.h"
#include "colas.h"
#include "nextion.h"

extern "C"
{
    void initNextion(void);
}

static const SerialConfig ser_cfg = {
    115200, 0, 0, 0,
};

mutex_t MtxNextionTx;
thread_t *procesoNextionCom;
char bufferReceivedNextion[80];
event_source_t enviarNextion_source;
uint8_t statusNextion;

void  trataRxNextion(uint8_t *msgRxNextion, uint8_t numBytes);

/*
 * Proceso nextionCom
 * Escucha para eventos de enviar a Nextion
 */

static THD_WORKING_AREA(wathreadNextionCom, 3500);

static THD_FUNCTION(threadNextionCom, arg) {
    (void)arg;
    eventmask_t evt;
    eventflags_t flags;
    uint8_t huboTimeout;
    uint16_t numBytes;
    event_listener_t nextionSend_listener, receivedData;
    chRegSetThreadName("nextionCom");

    chEvtRegisterMask(&enviarNextion_source, &nextionSend_listener,EVENT_MASK(0));
    chEvtRegisterMaskWithFlags (chnGetEventSource(&NEXTIONSD),&receivedData, EVENT_MASK (1),CHN_INPUT_AVAILABLE);

    while (true)
    {
        evt = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100));
        if (chThdShouldTerminateX())
        {
            chEvtUnregister(&enviarNextion_source, &nextionSend_listener);
            chEvtUnregister(chnGetEventSource(&NEXTIONSD), &receivedData);
            chThdExit((msg_t) 1);
        }
        if (evt & EVENT_MASK(0)) // Evento enviar a Nextion
        {
            continue;
        }
        if (evt & EVENT_MASK(1)) // Algo ha entrado desde Nextion
        {
            flags = chEvtGetAndClearFlags(&receivedData);
            if (flags & CHN_INPUT_AVAILABLE)
            {
                chgetNextionNoEchoTimeOut((BaseChannel *)&NEXTIONSD, (uint8_t *) bufferReceivedNextion, sizeof(bufferReceivedNextion), TIME_MS2I(5),&numBytes,&huboTimeout);
                if (huboTimeout)
                    goto continuaRx;
                trataRxNextion((uint8_t *)bufferReceivedNextion, numBytes);
            }
            continuaRx:
            continue;
        }
    }
}


void stopNextionCom(void)
{
    if (procesoNextionCom!=NULL)
    {
        chThdTerminate(procesoNextionCom);
        chThdWait(procesoNextionCom);
        sdStop(&NEXTIONSD);
        procesoNextionCom = NULL;
    }
}

void initNextion(void)
{

    palClearLine(LINE_TX4);
    palSetLine(LINE_RX4);
    palSetLineMode(LINE_TX4, PAL_MODE_ALTERNATE(7));
    palSetLineMode(LINE_RX4, PAL_MODE_ALTERNATE(7));
    sdStart(&NEXTIONSD,&ser_cfg);
    statusNextion = 0;
    if (!procesoNextionCom)
        procesoNextionCom = chThdCreateStatic(wathreadNextionCom, sizeof(wathreadNextionCom), NORMALPRIO, threadNextionCom, NULL);
}


