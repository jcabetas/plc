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

#include "bloques.h"
#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"


uint16_t CRC16(const uint8_t *nData, uint16_t wLength);



/*
 *
 //   numEntrada valor4mA valor20mA
protected:
    uint16_t idNombreDisp;
    uint16_t direccion;
    medida   *listMedidas[4];
    uint8_t  numEntradas[4];
    float    valor4mA[4];
    float    valor20mA[4];
    uint8_t  numMedidas;
public:
    adm4240(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~adm4240();
    uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(uint16_t canales[], int16_t *error);

 *
 *
 *   ADM4240 [NombreMedidor] [addr Modbus]
 *   ADM4240  MedidorPresion 1
 */
adm4240::adm4240(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros incorrecto ADM4240");
        *hayError = 1;
        return;
    }
    idNombreDisp = nombres::incorpora(pars[1]);
    direccion = atoi(pars[2]);
    numMedidas = 0;
    for (uint8_t m=0;m<4;m++)
    {
        listMedidas[m] = NULL;
        numEntradas[m] = 0;
        valor4mA[m] = 0.0f;
        valor20mA[m] = 0.0f;
    }
    modbus::addDisp(this);
};

adm4240::~adm4240()
{
}

uint16_t adm4240::diIdNombre(void)
{
    return idNombreDisp;
}

const char *adm4240::diTipo(void)
{
    return "dispositivoMedidas";
}


int8_t adm4240::init(void)
{
    // no necesita inicializacion
    return 0;
}


uint8_t adm4240::attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    /*
     *  parametros: numEntrada valor4mA valor20mA
     *  pars: 2 0.0 3.0
     */
    if (numMedidas>=4 || numPar!=3)
    {
        *hayError = 1;
        return 1;
    }
    uint8_t nEnt = atoi(pars[0]);
    if (nEnt==0 || nEnt>4)
    {
        nextion::enviaLog(NULL,"#canal ==0 o >4 en ADM4240");
        *hayError = 1;
        return 1;

    }
    // compruebo que no estaba definida
    for (uint8_t m=0;m<4;m++)
    {
        if (numEntradas[m]==nEnt)
        {
            nextion::enviaLog(NULL,"#canal ya usado ADM4240");
            *hayError = 1;
            return 1;
        }
    }
    listMedidas[numMedidas] = med;
    numEntradas[numMedidas] = nEnt;
    valor4mA[numMedidas] = atof(pars[1]);
    valor20mA[numMedidas] = atof(pars[2]);
    numMedidas++;
    return 0;
}

void adm4240::leer(uint16_t canales[], int16_t *error)
{
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t buffer[20];
    uint8_t bufferRx[20];

    /*
     * Example

    The following query will request Volts from an instrument with node address 1:

    Field Name Example(Hex)
    Slave Address 01
    Function 03
    Starting Address High 00
    Starting Address Low 00
    Number of Points High 00
    Number of Points Low 04
    Error Check Low 71
    Error Check High CB
     */
    buffer[0] = direccion;
    buffer[1] = 0x03;
    buffer[2] = 0;
    buffer[3] = 0;
    buffer[4] = 0;
    buffer[5] = 4;

    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;

    osalThreadSleepMilliseconds(5);
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(50));
    *error = modbus::chReadStrRs485(bufferRx, 13, &bytesReceived, chTimeMS2I(50));
    if (*error!=0 || bytesReceived!=13)
    {
        *error = -1;
        for (uint8_t can=1;can<=4;can++)
            canales[can] = 0;
        return;
    }
    msgCRC = CRC16(bufferRx, bytesReceived-2);
    rxCRC = (bufferRx[bytesReceived-1]<<8) + bufferRx[bytesReceived-2];
    if (msgCRC!=rxCRC)
    {
        *error = -2;
        for (uint8_t can=1;can<=4;can++)
            canales[can] = 0;
        return;
    }
    for (uint8_t can=1;can<=4;can++)
        canales[can-1] = (bufferRx[1+2*can]<<8) + bufferRx[2+2*can];
    *error = 0;
    return;
}

// retorna 1 si hay error
uint8_t adm4240::usaBus(void)
{
    uint16_t valores[4];
    int16_t error;
    uint8_t necesitoLeer = 0;
    for (uint8_t m=1;m<=numMedidas;m++)
    {
        medida *med = listMedidas[m-1];
        if (med==NULL)
            continue;
        if (med->necesitaUpdate())
            necesitoLeer = 1;
    }
    if (!necesitoLeer)
        return 2;
    // a leer (todas)
    leer(valores, &error);
    for (uint8_t m=1;m<=numMedidas;m++)
    {
        medida *med = listMedidas[m-1];
        if (med==NULL)
            continue;
        uint8_t numEntMenos1 = numEntradas[m-1] - 1;
        if (error!=0)
        {
            med->setValidez(0);
            med->set(&valor4mA[numEntMenos1], 0);
            continue;
        }
        uint16_t val = valores[numEntMenos1];
        if (val<3000) // <3 mA??
        {
            med->setValidez(0);
            med->set(&valor4mA[numEntMenos1], 0);
            continue;
        }
        if (val>23000) // >23 mA??
        {
            med->setValidez(0);
            med->set(&valor20mA[numEntMenos1], 0);
            continue;
        }
        float valor = valor4mA[numEntMenos1] + (valores[numEntMenos1]-4000)*(valor20mA[numEntMenos1] - valor4mA[numEntMenos1])/16000.0f;
        if (valor<valor4mA[numEntMenos1])
        {
            valor = valor4mA[numEntMenos1];
            med->setValidez(0);
        }
        else if (valor>valor20mA[numEntMenos1])
        {
            valor = valor20mA[numEntMenos1];
            med->setValidez(0);
        }
        else
        {
            med->setValidez(1);
        }
        med->set(&valor, 0);
    }
    return error;
}






