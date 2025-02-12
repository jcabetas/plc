/*
 * relesModbus.cpp
 */


#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"
#include "string.h"

#include "bloques.h"
#include "parametros.h"
#include "nextion.h"
#include "stdlib.h"
#include "dispositivos.h"


void int2str(uint8_t valor, char *string);
uint16_t CRC16(const uint8_t *nData, uint16_t wLength);


uint16_t numEstadoPlaca; // indica si esta ok (cuando esta a 1)
pinInput *pinesI2C[16];
uint8_t numInput[16];
uint8_t estado[16];
uint8_t numPines;
uint16_t checkIOIters;
/*
 *
 * placaI2CInput inputsI2C 32
 */
placaI2CInput::placaI2CInput(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros PLACAI2CInput");
        *hayError = 1;
        return; // error
    }
    numEstado = estados::addEstado(tty, pars[1],1, hayError);
    dirI2C = atoi(pars[2]);
    for (uint8_t p=0;p<16;p++)
    {
        pinesI2C[p] = NULL;
        numInput[p] = 0;
    }
    numPines = 0;
    checkIOIters = 0;
    i2cthread::addDisp(this);
};


placaI2CInput::~placaI2CInput()
{
}

uint8_t placaI2CInput::attachInputPlacaMadre(pinInput *inpPin, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    /*
     *  parametros: numInput
     *  pars: 2
     */
    if (numPar!=1)
    {
        nextion::enviaLog(NULL,"#parametros pininput en placaI2CInput!=1");
        *hayError = 1;
        return 1;
    }
    uint8_t nInput = atoi(pars[0]);
    if (nInput==0 || nInput>16)
    {
        nextion::enviaLog(NULL,"#entradas I2C erroneo");
        *hayError = 1;
        return 1;
    }
    // compruebo que no estaba definida
    for (uint8_t p=1;p<numPines;p++)
    {
        if (numInput[p]==nInput)
        {
            nextion::enviaLog(NULL,"#entrada I2C ya definida");
            *hayError = 1;
            return 1;
        }
    }
    pinesI2C[numPines] = inpPin;
    numInput[numPines] = nInput;
    numPines++;
    return 0;
}


const char *placaI2CInput::diTipo(void)
{
    return "placaI2CInput";
}

uint16_t placaI2CInput::diIdNombre(void)
{
    return estados::diIdNombre(numEstado);
}

int8_t placaI2CInput::init(void)
{
    uint8_t error = 0;
    i2cthread::lockI2C();                // comprueba direccion salidas
    i2cthread::initIODIR(dirI2C,0xFF,&error);
    i2cthread::initPullUp(dirI2C,&error);
    i2cthread::unlockI2C();
    return error;
}


// retorna 1 si hay error
uint8_t placaI2CInput::usaBus(void)
{
    uint16_t valorI2C;
    uint8_t error;

    i2cthread::lockI2C();
    i2cthread::writeAndRead2Bytes(dirI2C, &valorI2C,&error);
    i2cthread::unlockI2C();

    if (error)
    {
        valorI2C = 0;
        estados::ponEstado(numEstado, 1);
    }
    else
        estados::ponEstado(numEstado, 0);
    for (uint8_t numPin=1;numPin<=numPines;numPin++)
    {
        pinInput *pin = pinesI2C[numPin-1];
        if (pin==NULL)
            continue;
        uint8_t nPin = numInput[numPin-1];
        if (nPin==0)
            continue;
        nPin--;
        uint8_t valorPin = (valorI2C>>nPin) & 0x1;
        pin->setValor(valorPin);
    }
    if (checkIOIters++ > 1000)
    {
        checkIOIters = 0;
        i2cthread::lockI2C();
        uint32_t valorIO= i2cthread::leeIODIR(dirI2C,&error);
        i2cthread::unlockI2C();
        if (valorIO != 0xFFFF)
        {
            i2cthread::lockI2C();
            i2cthread::initIODIR(dirI2C,0xFF,&error);
            i2cthread::unlockI2C();
            nextion::enviaLog(NULL,"ConfigIO-I2C mal!");
        }
        i2cthread::lockI2C();
        valorIO = i2cthread::leePullUp(dirI2C,&error);
        i2cthread::unlockI2C();
        if (valorIO != 0xFFFF)
        {
            i2cthread::lockI2C();
            i2cthread::initPullUp(dirI2C,&error);
            i2cthread::unlockI2C();
            nextion::enviaLog(NULL,"ConfigIO-I2C mal!");
        }
    }
    return error;
}

