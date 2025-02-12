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
 * RELESMB RelesCocina 3
 */
relesMB::relesMB(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros PLACARELES MB");
        *hayError = 1;
        return; // error
    }
    idNombre = nombres::incorpora(pars[1]);
    address = atoi(pars[2]);
    for (uint8_t p=0;p<16;p++)
    {
        pinesMB[p] = NULL;
        estadoEnviado[p] = 0xFF;
        numRele[p] = 0;
    }
    numPines = 0;
    calendar::getFechaHora(&dateTimeLectAnterior);
    modbus::addDisp(this);
};


relesMB::~relesMB()
{
}

uint8_t relesMB::attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    /*
     *  parametros: numRele
     *  pars: 2
     */
    if (numPar!=1)
    {
        nextion::enviaLog(NULL,"#parametros pinout en releMB!=1");
        *hayError = 1;
        return 1;
    }
    uint8_t nRele = atoi(pars[0]);
    if (nRele==0 || nRele>16)
    {
        nextion::enviaLog(NULL,"#rele erroneo");
        *hayError = 1;
        return 1;
    }
    // compruebo que no estaba definida
    for (uint8_t p=1;p<numPines;p++)
    {
        if (numRele[p]==nRele)
        {
            nextion::enviaLog(NULL,"#rele ya usado");
            *hayError = 1;
            return 1;
        }
    }
    pinesMB[numPines] = outPin;
    numRele[numPines] = nRele;
    numPines++;
    return 0;
}


const char *relesMB::diTipo(void)
{
    return "relesMB";
}

uint16_t relesMB::diIdNombre(void)
{
    return idNombre;
}

int8_t relesMB::init(void)
{
    return 0;
}

void relesMB::escribir(uint8_t numPin, uint8_t valorDeseado, int16_t *error)
{
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t buffer[20];
    uint8_t bufferRx[20];
    uint8_t comando;
    int16_t errorRx;

    comando = 0x02; // apagar
    if (valorDeseado==1)
        comando = 0x06;  // encender con "delay". Mantiene activa la salida "delay" sec.
    // enciendo reles
    buffer[0] = address;
    buffer[1] = 0x06; // function: control_command
    buffer[2] = 0;       // rele Hi
    buffer[3] = numPin;  // rele Lo
    buffer[4] = comando;
    buffer[5] = 60;      // delay por 60 segundos
    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;
    osalThreadSleepMilliseconds(10);
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(50));
    errorRx = modbus::chReadStrRs485(bufferRx, 8, &bytesReceived, chTimeMS2I(50));
    if (errorRx!=0 || bytesReceived!=8)
    {
        *error = 1;
        return;
    }
    msgCRC = CRC16(bufferRx, bytesReceived-2);
    rxCRC = (bufferRx[bytesReceived-1]<<8) + bufferRx[bytesReceived-2];
    if (msgCRC!=rxCRC)
    {
        *error = 2;
        return;
    }
    *error = 0;
    return;
}


// retorna 1 si hay error
uint8_t relesMB::usaBus(void)
{
    uint8_t recordar;
    uint8_t heEnviado;
    int16_t error;
    uint16_t dsDif = calendar::dsDiff(&dateTimeLectAnterior);
    // cada 10s envia todos los valores activos
    recordar = 0;
    heEnviado = 0;
    if (dsDif > 100)
    {
        recordar = 1;
        calendar::getFechaHora(&dateTimeLectAnterior);
    }
    for (uint8_t numPin=1;numPin<=numPines;numPin++)
    {
        pinOut *pin = pinesMB[numPin-1];
        if (pin==NULL)
            continue;
        uint8_t valorDeseado = pin->getValor();
        if (estadoEnviado[numPin-1] != valorDeseado || (recordar && valorDeseado==1))
        {
//   JCF         escribir(numPin, valorDeseado, &error);
            escribir(numRele[numPin-1], valorDeseado, &error);
            if (!error)
                estadoEnviado[numPin-1] = valorDeseado;
            else
                return 1; // no tolero errores
            heEnviado = 1;
        }
    }
    if (heEnviado)
        return 0;
    else
        return 2;
}

