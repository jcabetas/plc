/*
 * rf95.c
 *
 *  Created on: 21 jul. 2019
 *      Author: joaquin
 */

#include "hal.h"
#include "ch.hpp"
#include "string.h"
#include "chprintf.h"
//#include "lcd.h"
#include "nextion.h"
#include "colas.h"
#include "calendar.h"
#include <RH_RF95.h>

using namespace chibios_rt;


/*
 * Las entradas usadas son
 * PF2: INT
 * PF5: CS
 * PF7: SPI5SCK
 * PF8: SPI5MISO
 * PF9: SPI5MOSI
 */
// ver ChibiStudio/chibios_stable-21.11.x/testhal/STM32/multi/SPI/cfg/stm32h743_nucleo144/portab.c

/*
 * Maximum speed SPI configuration (27MHz -BR0==Fpclk/4-, CPHA=0, CPOL=0, MSb first).
  * Low speed SPI configuration (421.875kHz, CPHA=0, CPOL=0, MSb first).
 */
//  SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA,
//SPI_CR1_BR_0 | SPI_CR1_BR_1,
static const SPIConfig spicfg = {
  false,
  false,
  NULL,
  NULL,
  GPIOD,
  GPIOD_NSS,
  SPI_CFG1_MBR_DIV8 | SPI_CFG1_DSIZE_VALUE(7),  // antes SPI_CR1_BR_0 | SPI_CR1_BR_1,
  0
};

extern event_source_t rf95int_event;
extern event_source_t newMsgRx_source;
thread_t *procesoRf95Int, *procesoRf95rx;
extern uint8_t _buf[RH_RF95_MAX_PAYLOAD_LEN];
extern volatile int16_t _lastRssi;
extern volatile uint8_t _bufLen;

extern uint8_t checkRf95int;

extern event_source_t newMsgRx_source;
extern event_source_t newMsgTx_source;

time_t GetTimeUnixSec(void);
uint8_t putQueu(struct queu_t *colaMed, void *ptrStructOrigen);
uint8_t getQueu(struct queu_t *colaMed, void *ptrStructDestino);
void ponEnLCD(uint8_t fila, char const msg[]);



extern struct queu_t colaMsgRx;

/*
 * Interrupciones rf95. Activa proceso rf95int para leer datos
 */
static void f2_cb(void *arg) {
  (void)arg;
  chSysLockFromISR();
  // activa gestor interrupciones
  chEvtBroadcastI(&rf95int_event);
  chSysUnlockFromISR();
}

/*
 * rf95int thread.
 * gestiona interrupciones
 * lee datos de rf95 y los almacena en cola de mensajes
 */
static THD_WORKING_AREA(rf95int_wa, 1224);
static THD_FUNCTION(rf95int, p) {
  (void)p;
  event_listener_t el0;
  paquete_t tipoPaq;
  struct msgRx_t msgRx;
  chRegSetThreadName("rf95int");
  chEvtRegister(&rf95int_event, &el0, 0);
  while(!chThdShouldTerminateX()) {
    eventmask_t evt = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(100));
    checkRf95int = 1;
    if (chThdShouldTerminateX())
        chThdExit((msg_t) 1);
    if (evt == 0)  // timeout
        continue;
    tipoPaq = RH_RF95_handleInterrupt();
    RH_RF95_setModeRx();
    if (tipoPaq==paqRx)
    {
        uint8_t size = _bufLen-4;
        if (size>sizeof(msgRx.msg))
            size = sizeof(msgRx.msg);
        msgRx.timet = calendar::getSecUnix();
        msgRx.numBytes = size;
        msgRx.rssi = _lastRssi;
        memcpy(msgRx.msg, &_buf[4], size);
        putQueu(&colaMsgRx, &msgRx);
        chEvtBroadcast(&newMsgRx_source);
        _bufLen = 0;
    }
  }
  chEvtUnregister(&rf95int_event, &el0);
  procesoRf95rx = NULL;
}



void initRF95(void)
{
    chEvtObjectInit(&rf95int_event);
    // defino los pines
    /*
     * Las entradas usadas son
     * PF2: INT
     * PF5: CS
     * PE2: RESET
     * PF7: SPI5SCK
     * PF8: SPI5MISO
     * PF9: SPI5MOSI
     */
    palClearLine(LINE_LORARESET);
    palSetLineMode(LINE_LORARESET, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(LINE_LORANSS, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(LINE_LORARESET);
    chThdSleepMilliseconds(2);
    palSetLineMode(LINE_LORARESET, PAL_MODE_INPUT);

    palEnableLineEvent(LINE_LORA_DIO, PAL_EVENT_MODE_RISING_EDGE);
    palSetLineCallback(LINE_LORA_DIO, f2_cb, NULL);

    spiStart(&SPID2, &spicfg);

    chEvtObjectInit(&newMsgRx_source);
    chEvtObjectInit(&newMsgTx_source);


    if (!procesoRf95Int)
        procesoRf95Int = chThdCreateStatic(rf95int_wa, sizeof(rf95int_wa), NORMALPRIO + 7,  rf95int, NULL);

    if (!RH_RF95_init())
    {
        nextion::enviaLog(NULL,"RF95init failed!!");
        //ponEnLCD(0,"RF95init failed!!");
        //osalThreadSleepMilliseconds(2000);
    }
    RH_RF95_setFrequency(434.0f);
    RH_RF95_setModeRx();
}



