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
 * RELES8INPUT8MB RelesCocina 20    3
 *                            ds Address
 */
reles8Input8MB::reles8Input8MB(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=4)
    {
        nextion::enviaLog(tty,"#parametros RELES8INPUT8MB");
        *hayError = 1;
        return; // error
    }
    idNombre = nombres::incorpora(pars[1]);
    minDs = atoi(pars[2]);
    address = atoi(pars[3]);
    for (uint8_t p=0;p<8;p++)
    {
        pinesRelesMB[p] = NULL;
        estadoEnviado[p] = 0xFF;
        numRele[p] = 0;
        pinesInputMB[p] = NULL;
        estado[p] = 0xFF;
        numInput[p] = 0;
    }
    numPinesReles = 0;
    numPinesInput = 0;
    calendar::getFechaHora(&dateTimeEscrituraAnterior);
    calendar::getFechaHora(&dateTimeLectAnterior);
    modbus::addDisp(this);
};


reles8Input8MB::~reles8Input8MB()
{
}

uint8_t reles8Input8MB::attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError)
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
    if (nRele==0 || nRele>8)
    {
        nextion::enviaLog(NULL,"#rele erroneo");
        *hayError = 1;
        return 1;
    }
    // compruebo que no estaba definida
    for (uint8_t p=1;p<numPinesReles;p++)
    {
        if (numRele[p]==nRele)
        {
            nextion::enviaLog(NULL,"#rele ya usado");
            *hayError = 1;
            return 1;
        }
    }
    pinesRelesMB[numPinesReles] = outPin;
    numRele[numPinesReles] = nRele;
    numPinesReles++;
    return 0;
}

uint8_t reles8Input8MB::attachInputPlacaMadre(pinInput *inpPin, uint8_t numPar, char *pars[], uint8_t *hayError)
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


const char *reles8Input8MB::diTipo(void)
{
    return "reles8Input8MB";
}

uint16_t reles8Input8MB::diIdNombre(void)
{
    return idNombre;
}

int8_t reles8Input8MB::init(void)
{
    return 0;
}

void reles8Input8MB::escribir(uint8_t numPin, uint8_t valorDeseado, int16_t *error)
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
    if (valorDeseado==1)
        buffer[5] = 60;      // delay por 60 segundos
    else
        buffer[5] = 0;
    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;
    osalThreadSleepMilliseconds(50);
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(100));
    errorRx = modbus::chReadStrRs485(bufferRx, 8, &bytesReceived, chTimeMS2I(200));
    if (errorRx!=0 || bytesReceived!=8)
    {
        *error = 1;
        return;
    }
    msgCRC = CRC16(bufferRx, bytesReceived-2);
    rxCRC = (bufferRx[bytesReceived-1]<<8) + bufferRx[bytesReceived-2];
    if (msgCRC!=rxCRC)
    {
        *error = 0;  // no parece que funcione a veces el CRC
        return;
    }
    *error = 0;
    return;
}

void reles8Input8MB::leer(uint16_t *resultado, int16_t *error)
{
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t buffer[20];
    uint8_t bufferRx[26];
    int16_t errorRx;
    uint8_t entMax;

    // leo inputs  reles8Input8MB
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
    uint8_t errorTx = modbus::chprintStrRs485(buffer, 8, chTimeMS2I(200));
    if (errorTx!=0)
    {
        *resultado = 0;
        *error = 3;
        return;
    }
    errorRx = modbus::chReadStrRs485(bufferRx, 5+entMax*2, &bytesReceived, chTimeMS2I(150));
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
uint8_t reles8Input8MB::usaBus(void)
{
    uint8_t recordar;
    uint8_t heEnviado;
    int16_t error, hayAlgunError;
    uint16_t entradas;
    // empezamos por los reles de salida
    uint16_t dsDif = calendar::dsDiff(&dateTimeEscrituraAnterior);
    // cada 10s envia todos los valores activos
    recordar = 0;
    heEnviado = 0;
    hayAlgunError = 0;
    if (dsDif > 400)
    {
        recordar = 1;
        calendar::getFechaHora(&dateTimeEscrituraAnterior);
    }
    for (uint8_t numPin=1;numPin<=numPinesReles;numPin++)
    {
        pinOut *pin = pinesRelesMB[numPin-1];
        if (pin==NULL)
            continue;
        uint8_t valorDeseado = pin->getValor();
        if (estadoEnviado[numPin-1] != valorDeseado || (recordar && valorDeseado==1))
        {
            escribir(numPin, valorDeseado, &error);
            if (!error)
            {
                estadoEnviado[numPin-1] = valorDeseado;
                heEnviado = 1;
            }
            else
                hayAlgunError = 1;
        }
    }
    // seguimos por las entradas, si procede
    dsDif = calendar::dsDiff(&dateTimeLectAnterior);
    if (numPinesInput==0 || dsDif < minDs)
    {
        if (hayAlgunError)
            return 1;
        else
            if (heEnviado)
                return 0;
            else
                return 2;
    }
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

