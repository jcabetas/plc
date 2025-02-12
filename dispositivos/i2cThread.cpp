/*
 * modbus.cpp
 *
 *  Created on: 17 abr. 2021
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"

#define IOGPIOA             0x12
#define PORTI2CIO           (I2CD2)
#define RESET_IO_PORT       (GPIOE)
#define RESET_IO_PIN        (15)
#define IODIRA                  0x0         /* Supuesto BANK=0 */
#define IODIRB                  0x1
#define IOGPIOA                 0x12
#define IOGPIOB                 0x13
#define IOGPPUA                 0x0C
#define IOGPPUB                 0x0D


uint8_t i2cthread::definido = 0;
i2cthread *i2cthread::i2cPtr = NULL;
thread_t *i2cthread::procesoI2C = NULL;
dispositivo *i2cthread::listDispositivosI2C[MAXDISPOSITIVOSI2C] = {0};
uint8_t i2cthread::errorEnDispI2C[MAXDISPOSITIVOSI2C] = {0};
uint16_t i2cthread::numDispositivosI2C = 0;
mutex_t i2cthread::MtxI2C;

/*
 * Thread i2c
 */
static THD_WORKING_AREA(threadI2C_wa, 2048);
static THD_FUNCTION(i2cThrd, arg) {
    (void)arg;
    uint16_t iter = 0;
    chRegSetThreadName("i2c");
    while (true) {
        if (++iter == 100)
        {
            i2cthread::leeTodos(1);
            iter = 0;
        }
        i2cthread::leeTodos(0);
        chThdSleepMilliseconds(50);
        if (chThdShouldTerminateX())
            chThdExit((msg_t) 1);
    }
}


void i2cthread::lockI2C(void)
{
    chMtxLock(&i2cthread::MtxI2C);
}

void i2cthread::unlockI2C(void)
{
    chMtxUnlock(&i2cthread::MtxI2C);
}

// I2CBUS
i2cthread::i2cthread(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    (void) pars;
    if (numPar!=1)
    {
        if (tty!=NULL)
            chprintf(tty,"#parametros incorrecto I2CBUS\n\r");
        else
            nextion::enviaLog(tty,"#parametros I2C");
        *hayError = 1;
        return; // error
    }
    if (definido)
    {
        nextion::enviaLog(tty,"#I2CBUS repetido");
        *hayError = 1;
    }
    for (uint8_t d=1;d<MAXDISPOSITIVOSI2C;d++)
    {
        listDispositivosI2C[d-1] = NULL;
        errorEnDispI2C[d-1] = 0;
    }
    numDispositivosI2C = 0;
    procesoI2C = NULL;
    i2cPtr = this;
    definido = 1;
};


void i2cthread::addDisp(dispositivo *disp)
{
    if (numDispositivosI2C>=MAXDISPOSITIVOSI2C)
    {
        nextion::enviaLog(NULL,"#demasiados dispositivos I2C");
        return;
    }
    listDispositivosI2C[numDispositivosI2C] = disp;
    errorEnDispI2C[numDispositivosI2C] = 0;
    numDispositivosI2C++;
}


void i2cthread::leeTodos(uint8_t incluyeErroneos)
{
    uint8_t error;
    for (uint8_t d=1;d<=numDispositivosI2C;d++)
    {
        if (incluyeErroneos || errorEnDispI2C[d-1]==0)
        {
            dispositivo *disp = listDispositivosI2C[d-1];
            if (disp!=NULL)
            {
                error = disp->usaBus();
                errorEnDispI2C[d-1] = error;
            }
        }
    }
}

void i2cthread::writeOutput(uint16_t dirTarjeta, uint16_t valorAEscribir, uint8_t *error)
{
    msg_t errorI2C;
    uint8_t mcp23017_tx_data[6];
    uint8_t mcp23017_rx_data[6];
    systime_t tmo = TIME_MS2I(10);
    mcp23017_tx_data[0] = IOGPIOA; /* Send value of IOGPIOA */
    mcp23017_tx_data[1] = ((0xFF00 & valorAEscribir)>>8);
    mcp23017_tx_data[2] = (0xFF & valorAEscribir);
    i2cAcquireBus(&PORTI2CIO);
    errorI2C = i2cMasterTransmitTimeout(&PORTI2CIO, dirTarjeta, mcp23017_tx_data, 3, mcp23017_rx_data, 0, tmo);
    i2cReleaseBus(&PORTI2CIO);
    if (errorI2C!=MSG_OK)
        *error = 1;
}

void i2cthread::initIODIR(uint16_t dirTarjeta, uint8_t dirIO, uint8_t *error)
{
    msg_t errorI2C;
    uint8_t mcp23017_tx_data[6];
    uint8_t mcp23017_rx_data[6];
    systime_t tmo = TIME_MS2I(10);
    /* Configure IODIRA and IODIRB as outputs */
    /* Send I2C1 START condition */
    mcp23017_tx_data[0] = IODIRA; /* Send value of IOGPIOA */
    mcp23017_tx_data[1] = dirIO;
    mcp23017_tx_data[2] = dirIO;
    i2cAcquireBus(&PORTI2CIO);
    errorI2C = i2cMasterTransmitTimeout(&PORTI2CIO, dirTarjeta, mcp23017_tx_data, 3, mcp23017_rx_data, 0, tmo);
    i2cReleaseBus(&PORTI2CIO);
    if (errorI2C!=MSG_OK)
        *error = 1;
}

uint32_t i2cthread::leeIODIR(uint16_t dirTarjeta, uint8_t *error)
{
    msg_t errorI2C;
    uint8_t mcp23017_tx_data[6];
    uint8_t mcp23017_rx_data[6];
    systime_t tmo = TIME_MS2I(10);
    mcp23017_tx_data[0] = IODIRA; /* Send value of IODIRA */
    i2cAcquireBus(&PORTI2CIO);
    errorI2C = i2cMasterTransmitTimeout(&PORTI2CIO, dirTarjeta, mcp23017_tx_data, 1, mcp23017_rx_data, 2, tmo);
    i2cReleaseBus(&PORTI2CIO);
    if (errorI2C!=MSG_OK)
        *error = 1;
    return ((mcp23017_rx_data[0]<<8) | mcp23017_rx_data[1]);
}

uint32_t i2cthread::leePullUp(uint16_t dirTarjeta, uint8_t *error)
{
    msg_t errorI2C;
    uint8_t mcp23017_tx_data[6];
    uint8_t mcp23017_rx_data[6];
    systime_t tmo = TIME_MS2I(10);
    mcp23017_tx_data[0] = IOGPPUA; /* Send value of IOGPPUA */
    i2cAcquireBus(&PORTI2CIO);
    errorI2C = i2cMasterTransmitTimeout(&PORTI2CIO, dirTarjeta, mcp23017_tx_data, 1, mcp23017_rx_data, 2, tmo);
    i2cReleaseBus(&PORTI2CIO);
    if (errorI2C!=MSG_OK)
        *error = 1;
    return ((mcp23017_rx_data[0]<<8) | mcp23017_rx_data[1]);
}

void  i2cthread::initPullUp(uint16_t dirTarjeta, uint8_t *error)
{
    msg_t errorI2C;
    uint8_t mcp23017_tx_data[6];
    uint8_t mcp23017_rx_data[6];
    systime_t tmo = TIME_MS2I(10);
    mcp23017_tx_data[0] = IOGPPUA; /* Send value of IOGPPUA (0xFF => pullup active) */
    mcp23017_tx_data[1] = 0xFF;
    mcp23017_tx_data[2] = 0xFF;
    i2cAcquireBus(&PORTI2CIO);
    errorI2C = i2cMasterTransmitTimeout(&PORTI2CIO, dirTarjeta, mcp23017_tx_data, 3, mcp23017_rx_data, 0, tmo);
    i2cReleaseBus(&PORTI2CIO);
    if (errorI2C!=MSG_OK)
        *error = 1;
}

void i2cthread::writeAndRead2Bytes(uint16_t dirTarjeta, uint16_t *valor, uint8_t *error)
{
    msg_t errorI2C;
    uint8_t mcp23017_tx_data[6];
    uint8_t mcp23017_rx_data[6];
    systime_t tmo = TIME_MS2I(10);
    mcp23017_tx_data[0] = IOGPIOA; /* Send value of IOGPIOA */
    i2cAcquireBus(&PORTI2CIO);
    errorI2C = i2cMasterTransmitTimeout(&PORTI2CIO, dirTarjeta, mcp23017_tx_data, 1, mcp23017_rx_data, 2, tmo);
    i2cReleaseBus(&PORTI2CIO);
    *valor = ((mcp23017_rx_data[0]<<8) | mcp23017_rx_data[1]);
    if (errorI2C!=MSG_OK)
        *error = 1;
}

void i2cthread::resetI2C(void)
{
    /* Reset MCP */
    palClearPad(RESET_IO_PORT,RESET_IO_PIN);
    chThdSleepMilliseconds(5);
    palSetPad(RESET_IO_PORT,RESET_IO_PIN);
    nextion::enviaLog(NULL,"Reseteado I2C");
}


// i2c clock = 48 MHz
// Según RM0433, pag. 1.984, para 400 kHz:
// - PRESC: 0x5
// - SCLL:  0x9
// - tscll: 1.250 ns
// - SCLH: 3
// - SDADEL: 3
// - SCLDEL: 3

static const I2CConfig i2ccfg = {
  5<<I2C_TIMINGR_PRESC_Pos | 3<<I2C_TIMINGR_SCLDEL_Pos | 3<<I2C_TIMINGR_SDADEL_Pos | 3<<I2C_TIMINGR_SCLH_Pos | 9<<I2C_TIMINGR_SCLL_Pos,
  0,    //cr1
  0     //cr2
};


int8_t i2cthread::init(void)
{

/*
 * Configuracion en PLC:
 * Reset: PE15
 * I2C2SCL: PB10
 * I2C2SDA: PB11
 */
    char buffer[60];
    uint8_t error;
    if (procesoI2C != NULL)
        return 0;
    if (i2cthread::numDispositivosI2C==0)
        return 0;

    i2cStart(&I2CD2, &i2ccfg);
    chMtxObjectInit(&MtxI2C);
    resetI2C();
    for (uint8_t i=1;i<=numDispositivosI2C;i++)
    {
        dispositivo *dispI2C = i2cthread::listDispositivosI2C[i-1];
        error = dispI2C->init();
        if (error)
        {
            chsnprintf(buffer,sizeof(buffer)," Error en I2C %s",nombres::nomConId(listDispositivosI2C[i-1]->diIdNombre()));
            nextion::enviaLog(NULL,buffer);
            return 1;
        }
    }
    procesoI2C = chThdCreateStatic(threadI2C_wa, sizeof(threadI2C_wa), NORMALPRIO,  i2cThrd, NULL);
    return 0;
}


void i2cthread::stop(void)
{
   if (procesoI2C!=NULL)
    {
        chThdTerminate(procesoI2C);
        chThdWait(procesoI2C);
        procesoI2C = NULL;
    }
    i2cStop(&I2CD2);
}


i2cthread::~i2cthread()
{
    for (uint8_t d=0;d<MAXDISPOSITIVOSI2C;d++)
        listDispositivosI2C[d] = NULL;
    numDispositivosI2C = 0;
    i2cPtr = NULL;
    definido = 0;
}


const char *i2cthread::diTipo(void)
{
    return "i2cthread";
}

const char *i2cthread::diNombre(void)
{
    return "I2CTHREAD";
}

void i2cthread::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  )
{
}

void i2cthread::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"I2CTHREAD %d disp.",numDispositivosI2C);
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}



