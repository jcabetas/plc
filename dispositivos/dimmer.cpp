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
#include "math.h"



/*
    class dimmer : public dispositivo
    {
        // dimmer termo 2 2000 WTermo
    protected:
        uint16_t idNombreDisp;
        uint16_t direccion;
        medida   medidaSP;
        float    wMax;
        struct fechaHora dateTimeLectAnterior;
    public:
        dimmer(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
        ~dimmer();
        uint8_t usaBus(void);
        uint16_t diIdNombre(void);
        const char *diTipo(void);
        int8_t init(void);
    };
 */

/*
 * Lee el valor de la medida de consigna, sabe la potencia maxima de la carga y ajusta factor de carga del dimmer
 * limita el refresco a cuando haya cambios de setpoint o mas de 15s sin enviar datos
 */

#define     HR_PSETPOINT        6
uint16_t CRC16(const uint8_t *nData, uint16_t wLength);

/*
 * DIMMER Nombre idMB Pmax MedidaSP
 * DIMMER Termo    15 2000 WTermo
 */
dimmer::dimmer(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=5)
    {
        nextion::enviaLog(tty,"#parametros incorrecto DIMMER");
        *hayError = 1;
        return;
    }
    idNombreDisp = nombres::incorpora(pars[1]);
    direccion = atoi(pars[2]);
    wMax =atof(pars[3]);
    medidaSP = medida::findMedida(pars[4]);
    oldSP = 0.0f;
    calendar::getFechaHora(&dateTimeLectAnterior);
    modbus::addDisp(this);
};

dimmer::~dimmer()
{
}

uint16_t dimmer::diIdNombre(void)
{
    return idNombreDisp;
}

const char *dimmer::diTipo(void)
{
    return "dimmer";
}


int8_t dimmer::init(void)
{
    // no necesita inicializacion
    return 0;
}


void dimmer::escribeHR(uint16_t valor, uint16_t addressReg, int16_t *error)
{
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t buffer[20];
    uint8_t bufferRx[20];

// Write Single Holding Register
//  Slave Address   11
//  Function        06
//  Reg. Address Hi 00
//  Reg. Address Lo 01
//  Write Data Hi   00
//  Write Data Lo   03
//  Error Check Lo  9A
//  Error Check Hi  9B

    buffer[0] = direccion;
    buffer[1] = 0x06;
    buffer[2] = (addressReg&0xFF00)>>8;
    buffer[3] = addressReg&0xFF;
    buffer[4] = (valor & 0xFF00)>>8;
    buffer[5] = valor & 0xFF;

    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;
    bufferRx[0] = 0;
    bufferRx[1] = 0;
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(100));

/*
    Devuelve
    buffTx[0] = holdingRegisters[HR_IDMODBUS];
    buffTx[1] = 6;
    buffTx[2] = buffer[2];
    buffTx[3] = buffer[3];
    buffTx[4] = buffer[4];
    buffTx[5] = buffer[5];
    uint16_t msgCRC = CRC16(buffTx, 6);
    buffTx[6] = (msgCRC & 0xFF);
    buffTx[7] = (msgCRC & 0xFF00) >> 8;
*/

    *error = modbus::chReadStrRs485(bufferRx, 8, &bytesReceived, chTimeMS2I(100));
    if (*error!=0 || bytesReceived!=8)
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
    return;
}

// retorna 1 si hay error
// mejor: 0: no error, 1: error, 2: no se ha usado
uint8_t dimmer::usaBus(void)
{
    int16_t error;
    uint16_t dsDif = calendar::dsDiff(&dateTimeLectAnterior);
    float newSP = medidaSP->diValor();
    if (fabs(newSP - oldSP)>10.0f || dsDif>150)
    {
        uint16_t factorCarga = 100.0f*newSP/wMax;
        escribeHR(factorCarga, HR_PSETPOINT, &error);
        if (error!=0)
        {
            // vuelve a intentarlo
            escribeHR(factorCarga, HR_PSETPOINT, &error);
            if (error!=0)
                return 2;
        }
        oldSP = newSP;
        calendar::getFechaHora(&dateTimeLectAnterior);
    }
    return 0;
}

