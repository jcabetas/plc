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
#include "calendar.h"


void int2str(uint8_t valor, char *string);
uint16_t CRC16(const uint8_t *nData, uint16_t wLength);

/*
 *                   ds addr
 * MB16DI EntradasMB 2   4
 */
mb16di::mb16di(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=4)
    {
        nextion::enviaLog(tty,"#parametros MB16DI");
        *hayError = 1;
        return; // error
    }
    idNombre = nombres::incorpora(pars[1]);
    minDs = atoi(pars[2]);
    calendar::getFechaHora(&dateTimeLectAnterior);
    address = atoi(pars[3]);

    for (uint8_t p=0;p<16;p++)
    {
        pinesMB[p] = NULL;
        estado[p] = 0xFF;
        numInput[p] = 0;
    }
    numPines = 0;
    modbus::addDisp(this);
};


mb16di::~mb16di()
{
}

uint8_t mb16di::attachInputPlacaMadre(pinInput *inpPin, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    /*
     *  parametros: numEntrada
     *  pars: 2
     */
    if (numPar!=1)
    {
        nextion::enviaLog(NULL,"#parametros pinInput en releMB!=1");
        *hayError = 1;
        return 1;
    }
    uint8_t nInput = atoi(pars[0]);
    if (nInput==0 || nInput>16)
    {
        nextion::enviaLog(NULL,"#input erroneo");
        *hayError = 1;
        return 1;
    }
    // compruebo que no estaba definida
    for (uint8_t p=1;p<numPines;p++)
    {
        if (numInput[p]==nInput)
        {
            nextion::enviaLog(NULL,"#input ya usado");
            *hayError = 1;
            return 1;
        }
    }
    pinesMB[numPines] = inpPin;
    numInput[numPines] = nInput;
    numPines++;
    return 0;
}


const char *mb16di::diTipo(void)
{
    return "mb16di";
}

uint16_t mb16di::diIdNombre(void)
{
    return idNombre;
}

int8_t mb16di::init(void)
{
    return 0;
}

void mb16di::leer(uint16_t *resultado, int16_t *error)
{
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t buffer[20];
    uint8_t bufferRx[20];
    int16_t errorRx;

    // leo inputs  MBSL16DI
    // ver https://es.aliexpress.com/item/32735641541.html
    buffer[0] = address;
    buffer[1] = 2;  // function (03=read)
    buffer[2] = 0;// addrhi
    buffer[3] = 0; // addrlo (0x)C41 = 40001 (inputs 1-8)
    buffer[4] = 0;  // num. bytes hi
    buffer[5] = 16;    // lo
    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;
    osalThreadSleepMilliseconds(5);
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(50));
    errorRx = modbus::chReadStrRs485(bufferRx, 7, &bytesReceived, chTimeMS2I(50));
    // devuelve 1 2 2 0 0 185 184
    if (errorRx!=0 || bytesReceived!=7)
    {
        *resultado = 0;
        *error = 1;
        return;
    }
    msgCRC = CRC16(bufferRx, bytesReceived-2);
    rxCRC = (bufferRx[bytesReceived-1]<<8) + bufferRx[bytesReceived-2];
    if (msgCRC!=rxCRC)
    {
        *resultado = 0;
        *error = 2;
        return;
    }
    *resultado = (uint16_t) (bufferRx[4]<<8) + bufferRx[3];
    *error = 0;
    return;
}


// retorna 1 si hay error
uint8_t mb16di::usaBus(void)
{
    int16_t error;
    uint16_t entradas;
    uint16_t dsDif = calendar::dsDiff(&dateTimeLectAnterior);
    if (dsDif < minDs)
        return 2;
    leer(&entradas, &error);
    if (error>0)
        return 1;
    calendar::getFechaHora(&dateTimeLectAnterior);
    for (uint8_t numPin=1;numPin<=numPines;numPin++)
    {
        pinInput *pin = pinesMB[numPin-1];
        if (pin==NULL)
            continue;
        uint8_t nPin = numInput[numPin-1];
        if (nPin==0)
            continue;
        nPin--;
        uint8_t valorPin = (entradas>>nPin) & 0x1;
        pin->setValor(valorPin);
    }
    return 0;
}

