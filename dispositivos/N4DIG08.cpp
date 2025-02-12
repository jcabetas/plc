/*
 * Mini placa 8 inputs.cpp
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
 * N4DIG08 INPALARMA 20    1
 *                   ds Address
 */
N4DIG08::N4DIG08(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=4)
    {
        nextion::enviaLog(tty,"#parametros N4DIG08");
        *hayError = 1;
        return; // error
    }
    idNombre = nombres::incorpora(pars[1]);
    minDs = atoi(pars[2]);
    address = atoi(pars[3]);
    for (uint8_t p=0;p<8;p++)
    {
        pinesInputMB[p] = NULL;
        estado[p] = 0xFF;
        numInput[p] = 0;
    }
    numPinesInput = 0;
    calendar::getFechaHora(&dateTimeLectAnterior);
    modbus::addDisp(this);
};


N4DIG08::~N4DIG08()
{
}

uint8_t N4DIG08::attachInputPlacaMadre(pinInput *inpPin, uint8_t numPar, char *pars[], uint8_t *hayError)
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
    if (nInput==0 || nInput>8)
    {
        nextion::enviaLog(NULL,"#input erroneo");
        *hayError = 1;
        return 1;
    }
    // compruebo que no estaba definida
    for (uint8_t p=1;p<numPinesInput;p++)
    {
        if (numInput[p]==nInput)
        {
            nextion::enviaLog(NULL,"#input ya usado");
            *hayError = 1;
            return 1;
        }
    }
    pinesInputMB[numPinesInput] = inpPin;
    numInput[numPinesInput] = nInput;
    numPinesInput++;
    return 0;
}


const char *N4DIG08::diTipo(void)
{
    return "N4DIG08";
}

uint16_t N4DIG08::diIdNombre(void)
{
    return idNombre;
}

int8_t N4DIG08::init(void)
{
    return 0;
}

void N4DIG08::leer(uint16_t *resultado, int16_t *error)
{
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t buffer[20];
    uint8_t bufferRx[26];
    int16_t errorRx;
    uint8_t entMax;

    // leo inputs  N4DIG08
    // es una castaña, cada bit de entrada es un registro, empezando en 0x81
    // ver cual es la entrad maxima que hay que leer
    /*
     *     pinInput *pinesInputMB[8];
    uint8_t numInput[8];
    uint8_t estado[8];
    uint8_t numPinesInput;
     */
    entMax = 0;
    for (uint8_t i=1;i<=numPinesInput;i++)
    {
        if (numInput[i-1]>entMax)
            entMax = numInput[i-1];
    }
    buffer[0] = address;
    buffer[1] = 3;  // function (03=read)
    buffer[2] = 0;// addrhi
    buffer[3] = 0x81; // addrlo (0x)C41 = 40001 (inputs 1-8)
    buffer[4] = 0;  // num. regs hi
    buffer[5] = entMax;    // lo
    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;
    osalThreadSleepMilliseconds(10);
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(50));
    errorRx = modbus::chReadStrRs485(bufferRx, 5+entMax*2, &bytesReceived, chTimeMS2I(100));
    // devuelve 1 2 2 0 0 185 184
    if (errorRx!=0 || bytesReceived!=(5+entMax*2))
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
    *resultado = 0;
    for (uint8_t p=0;p<entMax;p++)
        *resultado |= (bufferRx[4+2*p]<<p);
    *error = 0;
    return;
}

// retorna 1 si hay error
uint8_t N4DIG08::usaBus(void)
{
    int16_t error;
    uint16_t entradas;
    // seguimos por las entradas, si procede
    uint16_t dsDif = calendar::dsDiff(&dateTimeLectAnterior);
    if (numPinesInput==0 || dsDif < minDs)
        return 2;
    leer(&entradas, &error);
    if (error>0)
        return 1;
    calendar::getFechaHora(&dateTimeLectAnterior);
    for (uint8_t numPin=1;numPin<=numPinesInput;numPin++)
    {
        pinInput *pin = pinesInputMB[numPin-1];
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

