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

////xymd02
//class xymd02 : public dispositivo
//{
//    // xymd02 tempSalon 2
//protected:
//    uint16_t idNombreDisp;
//    uint16_t direccion;
//    medida   *medidaTemp;
//    medida   *medidaHum;
//    uint8_t  numMedidas;
//    uint8_t  erroresSeguidos;
//public:
//    xymd02(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
//    ~xymd02();
//    uint8_t usaBus(void);
//    uint16_t diIdNombre(void);
//    const char *diTipo(void);
//    int8_t init(void);
//    void leer(float *temp, float *humedad, int16_t *error);
//};


/*
 *  Register    Contenido       Bytes
 *      1        Temperatura     2     uint16_t temp*10
 *      2        Humedad         2     idem
 *    0x101      Address         2
 *    0x102      Baud:           2
 *                0:9600
 *                1:14400
 *                2:19200
 *    0x103      Temp correc/10  2
 *                 -10..+10
 *    0x104      Humed correc/10  2
 *                 -10..+10
 *
 */
uint16_t CRC16(const uint8_t *nData, uint16_t wLength);

/*
 * XYMD02 5 medCocina tempCocina humCocina
 */
xymd02::xymd02(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=5)
    {
        nextion::enviaLog(tty,"#parametros incorrecto xymd02");
        *hayError = 1;
        return;
    }
    direccion = atoi(pars[1]);
    idNombreDisp = nombres::incorpora(pars[2]);
    direccion = atoi(pars[3]);
    medidaTemp = medida::findMedida(pars[4]);
    if (medidaTemp==NULL)
    {
        nextion::enviaLog(NULL,"No encuentro medTemp en xymd02");
        *hayError = 1;
    }
    medidaHum = medida::findMedida(pars[3]);
    if (medidaHum==NULL)
    {
        nextion::enviaLog(NULL,"No encuentro medHumedad en xymd02");
        *hayError = 1;
    }
    erroresSeguidos = 0;
    modbus::addDisp(this);
};

xymd02::~xymd02()
{
}

uint16_t xymd02::diIdNombre(void)
{
    return idNombreDisp;
}

const char *xymd02::diTipo(void)
{
    return "medTempHumedad";
}


int8_t xymd02::init(void)
{
    // no necesita inicializacion
    return 0;
}



void leeMBfunc04(uint8_t dirMB, uint16_t addressReg, uint8_t numRegs, uint8_t bufferRx[], uint16_t sizeofbufferRx, int16_t *error)
{
    uint8_t buffer[10];
    if (sizeofbufferRx <= (7 + 2*numRegs))
    {
        *error = -3;
        return;
    }
    buffer[0] = dirMB;
    buffer[1] = 0x04;
    buffer[2] = (addressReg&0xFF00)>>8;
    buffer[3] = addressReg&0xFF;
    buffer[4] = (numRegs & 0xFF00) >>8;
    buffer[5] = numRegs & 0xFF;

    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;
    bufferRx[0] = 0;
    bufferRx[1] = 0;
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(100));
    *error = modbus::chReadStrRs485(bufferRx, 7+2*numRegs, &bytesReceived, chTimeMS2I(100));
    if (*error!=0 || bytesReceived!=(7+2*numRegs))
    {
        *error = -1;
        return;
    }
    msgCRC = CRC16(bufferRx, bytesReceived-2);
    rxCRC = (bufferRx[bytesReceived-1]<<8) + bufferRx[bytesReceived-2];
    if (msgCRC!=rxCRC || direccion!= bufferRx[0] || bufferRx[1]!=0x04)
    {
        *error = -2;
        return;
    }
}

void xymd02::leer(uint16_t *float *valor, uint16_t addressReg, int16_t *error)
{

    return;
}

// retorna 1 si hay error
// mejor: 0: no error, 1: error, 2: no se ha usado
uint8_t xymd02::usaBus(void)
{
    uint16_t tempx10, humx10;
    uint8_t bufferRx[15];
    float valorTemp, valorHum;
    int16_t error;
    uint8_t heEnviado = 1;
    // definido
    if (med==NULL)
        continue;
    if (medidaTemp->necesitaUpdate())
    {
        leeMBfunc04(direccion, 1, 2, *bufferRx, sizeof(bufferRx), error);
        if (*error != 0)
        {
            erroresSeguidos++;
            if (erroresSeguidos>4)
            {
                valorTemp = 0.0f;
                valorHum = 0.0f;
                medidaTemp->set(&valorTemp, 1); // si hay error, vale 0.0f
                medidaTemp->setValidez(0);
                medidaHum->set(&valorHum, 1);   // si hay error, vale 0.0f
                medidaHum->setValidez(0);
            }
            return 2;
        }
        // temperatura en buff[3,4]
        tempx10 = bufferRx[3]<<8 + bufferRx[4];
        humx10 = bufferRx[5]<<8 + bufferRx[6];
        valorTemp = 10.0f*((float)tempx10);
        medidaTemp->set(&valorTemp, 0);
        valorHum = 10.0f*((float)humx10);
        medidaHum->set(&valorHum, 0);
    }
    return 0;
}




