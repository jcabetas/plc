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

/*
 *
 * placaI2COut outsI2C 33
 */
placaI2COut::placaI2COut(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros PLACAI2COut");
        *hayError = 1;
        return; // error
    }
    numEstado = estados::addEstado(tty, pars[1],1, hayError);
    dirI2C = atoi(pars[2]);
    for (uint8_t p=0;p<16;p++)
    {
        pinesI2C[p] = NULL;
        estadoEnviado[p] = 0xFF;
        numRele[p] = 0;
    }
    numPines = 0;
    checkIOIters = 0;
    i2cthread::addDisp(this);
};


placaI2COut::~placaI2COut()
{
}

uint8_t placaI2COut::attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    /*
     *  parametros: numSalida
     *  pars: 2
     */
    if (numPar!=1)
    {
        nextion::enviaLog(NULL,"#parametros pinout en releI2C!=1");
        *hayError = 1;
        return 1;
    }
    uint8_t nRele = atoi(pars[0]);
    if (nRele==0 || nRele>16)
    {
        nextion::enviaLog(NULL,"#salida I2C erroneo");
        *hayError = 1;
        return 1;
    }
    // compruebo que no estaba definida
    for (uint8_t p=1;p<numPines;p++)
    {
        if (numRele[p]==nRele)
        {
            nextion::enviaLog(NULL,"#salida I2C ya usada");
            *hayError = 1;
            return 1;
        }
    }
    pinesI2C[numPines] = outPin;
    numRele[numPines] = nRele;
    numPines++;
    return 0;
}


const char *placaI2COut::diTipo(void)
{
    return "placaI2COut";
}

uint16_t placaI2COut::diIdNombre(void)
{
    return estados::diIdNombre(numEstado);
}

int8_t placaI2COut::init(void)
{
    uint8_t error = 0;
    i2cthread::lockI2C();
    i2cthread::initIODIR(dirI2C,0x00,&error);
    i2cthread::unlockI2C();
    return error;
}


// retorna 1 si hay error
uint8_t placaI2COut::usaBus(void)
{
    uint16_t valorOut = 0;
    uint8_t error;
    for (uint8_t numPin=1;numPin<=numPines;numPin++)
    {
        pinOut *pin = pinesI2C[numPin-1];
        if (pin==NULL)
            continue;
        uint8_t valorDeseado = pin->getValor();
        if (valorDeseado == 1)
            valorOut |=  (1<<(numPin-1));
        else
            valorOut &= ~(1<<(numPin-1));
    }
    i2cthread::lockI2C();
    i2cthread::writeOutput(dirI2C,valorOut,&error);
    i2cthread::unlockI2C();
    // cada 1000 iteraciones comprueba IO config
    if (checkIOIters++ > 1000)
    {
        checkIOIters = 0;
        i2cthread::lockI2C();
        uint16_t valorIO = i2cthread::leeIODIR(dirI2C,&error);
        i2cthread::unlockI2C();
        if (valorIO != 0)
        {
            i2cthread::lockI2C();
            i2cthread::initIODIR(dirI2C,0x00,&error);
            i2cthread::unlockI2C();
            nextion::enviaLog(NULL,"ConfigIO-I2C mal!");
            return 1;
        }
    }
    return 0;
}

