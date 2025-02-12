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

/*
- Crear dispositivo que use el MODBUS, p.e. TAC11XX con direccion=2
   TAX11XX MedidorTermo MB1 2 (nombreDispo, InterfaceModbus, direccion)
*/
/*
class tac11xx : public dispositivo
{
    // TAC11X MedidorTermo 2
protected:
    uint16_t idNombreDisp;
    uint16_t direccion;
    medida   *listMedidas[MAXMEDENDISPOSI];
    uint8_t  tipo[MAXMEDENDISPOSI];
    uint8_t  numMedidas;
    uint8_t  erroresSeguidos;
public:
    tac11xx(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~tac11xx();
    void changeID(uint8_t oldId, uint8_t newId, int16_t *error);
    uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(float *valor, uint16_t addressReg, int16_t *error);
};

 Como funciona:
 - Crear IO modbus, con MODBUS (Numero de modbus, interfaceModbus, baudios)
    MODBUS 1 MB1 9600
 - Crear dispositivo que use el MODBUS, p.e. TAC11XX con direccion=2
    TAC11XX Medidortermo MB1 2 (nombreDispo, InterfaceModbus, direccion)
 - Crear medidas, actualizable cada 3s y 20s
    MEDIDA WTermo Medidortermo W 3 (nombreMedida, nombreDispo, nombreDatos, periodicidad)
                                    + fechaUltimaMedida, esValida, errorMedida
    MEDIDA Tension Medidortermo V 20s
 - Utilizar la medida
    ASIGNAVAR intensidad WTermo Tension /
    ASIGNAESTADO encendido.base intensidad 2 >

 Metodo:
 - Sólo hacemos esto a nivel de Modbus
 - Thread a nivel de Modbus.
   Ese thread conoce los dispositivos conectados.
   Los dispositivos deben implementar:
   * usoBus(): debe mirar si necesita actualizar datos, y si procede usar el bus.
               devuelve el error. Si un dispositivo da error, sólo se reintentará cada 10s
   El tac11x debe tener punteros a las medidas
   Las medidas deben guardar antiguedad de la lectura
 */


uint16_t CRC16(const uint8_t *nData, uint16_t wLength);

static const char *descMedidaTAC11XX[] = {"V","I","P","Px2","Px3","Q","kWh","kVArh"};
static const uint16_t addressTAC11XX[] = {0, 0x6, 0x0C, 0x0C, 0x0C, 0x18, 0x504, 0x50C};


/*
class tac11xx : public dispositivo
{
    // TAC11X MedidorTermo 2
protected:
    uint16_t idNombreDisp;
    uint16_t direccion;
    medida   *listMedidas[MAXMEDENDISPOSI];
    uint8_t  tipo[MAXMEDENDISPOSI];
    uint8_t  numMedidas;
    uint8_t  erroresSeguidos;
public:
    tac11xx(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~tac11xx();
    void changeID(uint8_t oldId, uint8_t newId, int16_t *error);
    uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(float *valor, uint16_t addressReg, int16_t *error);
};
 */
tac11xx::tac11xx(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros incorrecto TAC11XX");
        *hayError = 1;
        return;
    }
    idNombreDisp = nombres::incorpora(pars[1]);
    direccion = atoi(pars[2]);
    numMedidas = 0;
    erroresSeguidos = 0;
    for (uint8_t m=0;m<MAXMEDENDISPOSI;m++)
    {
        listMedidas[m] = NULL;
        tipo[m] = 0;
    }
    modbus::addDisp(this);
};

tac11xx::~tac11xx()
{
}

uint16_t tac11xx::diIdNombre(void)
{
    return idNombreDisp;
}

const char *tac11xx::diTipo(void)
{
    return "dispositivoMedidas";
}


int8_t tac11xx::init(void)
{
    // no necesita inicializacion
    return 0;
}


uint8_t tac11xx::attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    // pars: P
    if (numMedidas>=MAXMEDENDISPOSI || numPar!=1)
    {
        nextion::enviaLog(NULL,"# params en medida");
        *hayError = 1;
        return 1;
    }
    for (uint8_t tip=0;tip<8;tip++)
    {
        if (!strcasecmp(descMedidaTAC11XX[tip],pars[0]))
        {
            tipo[numMedidas] = tip;
            listMedidas[numMedidas] = med;
            numMedidas++;
            return 0;
        }
    }
    nextion::enviaLog(NULL,"#tipo medida no existe");
    *hayError = 1; // no esta definido ese tipo de medida
    return 0;
}

void tac11xx::leer(float *valor, uint16_t addressReg, int16_t *error)
{
    float valor2;
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t  *ptrChr;
    uint8_t buffer[20];
    uint8_t bufferRx[20];

    /*
     * Example

    The following query will request Volts from an instrument with node address 1:

    Field Name Example(Hex)
    Slave Address 01
    Function 04
    Starting Address High 00
    Starting Address Low 00
    Number of Points High 00
    Number of Points Low 02
    Error Check Low 71
    Error Check High CB
     */
    buffer[0] = direccion;
    buffer[1] = 0x04;
    buffer[2] = (addressReg&0xFF00)>>8;
    buffer[3] = addressReg&0xFF;
    buffer[4] = 0;
    buffer[5] = 2;

    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;
    bufferRx[0] = 0;
    bufferRx[1] = 0;
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(100));
    *error = modbus::chReadStrRs485(bufferRx, 9, &bytesReceived, chTimeMS2I(100));
    if (*error!=0 || bytesReceived!=9)
    {
        *error = -1;
        *valor = 0.0f;
        return;
    }
    msgCRC = CRC16(bufferRx, bytesReceived-2);
    rxCRC = (bufferRx[bytesReceived-1]<<8) + bufferRx[bytesReceived-2];
    if (msgCRC!=rxCRC || direccion!= bufferRx[0] || bufferRx[1]!=0x04)
    {
        *error = -2;
        *valor = 0.0f;
        return;
    }
    ptrChr = (uint8_t *)&valor2;
    *ptrChr++ = bufferRx[6];
    *ptrChr++ = bufferRx[5];
    *ptrChr++ = bufferRx[4];
    *ptrChr = bufferRx[3];
    *error = 0;
    *valor = valor2;
    return;
}

// retorna 1 si hay error
// mejor: 0: no error, 1: error, 2: no se ha usado
uint8_t tac11xx::usaBus(void)
{
    float valor;
    int16_t error;
    uint8_t heEnviado = 1;
    for (uint8_t m=1;m<=numMedidas;m++)
    {
        medida *med = listMedidas[m-1];
        if (med==NULL)
            continue;
        if (med->necesitaUpdate())
        {
            uint8_t tip = tipo[m-1];
            leer(&valor, addressTAC11XX[tip], &error);
            heEnviado = 1;
            if (error!=0)
            {
                erroresSeguidos++;
                // vuelve a intentarlo
                leer(&valor, addressTAC11XX[tip], &error);
                if (error!=0)
                {
                    erroresSeguidos++;
                    if (erroresSeguidos>4)
                    {
                        med->set(&valor, 1); // si hay error, vale 0.0f
                        med->setValidez(0);
                    }
                    return 2;
                }
            }
            else
                erroresSeguidos = 0;
            // quizas hay que multiplicar valor (p.e. Px2)
            if (tip==3)
                valor *= 2.0f;
            if (tip==4)
                valor *= 3.0f;
//            med->setValidez(1); ya se valida al hacer el set
            med->set(&valor, 0);
        }
    }
    if (heEnviado)
        return 0;
    else
        return 2;
}

void tac11xx::changeID(uint8_t oldId, uint8_t newId, int16_t *error)
{
    // creo que no va afuncionar, se hace directamente en el sensor
    float newIdFloat;
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t  *ptrChr;
    uint8_t buffer[20];
    uint8_t bufferRx[20];

    /*
     * Example

    The following query will request Volts from an instrument with node address 1:

    Field Name Example(Hex)
    Slave Address 01
    Function 04
    Starting Address High 00
    Starting Address Low 00
    Number of Points High 00
    Number of Points Low 02
    Error Check Low 71
    Error Check High CB
     */
    newIdFloat = (float) newId;
    buffer[0] = oldId;//direccion;
    buffer[1] = 0x10; // write holding registers
    buffer[2] = 0;//(addressReg&0xFF00)>>8;
    buffer[3] = 0x14;//addressReg&0xFF;
    buffer[4] = 0;
    buffer[5] = 2; // number of registers low
    buffer[6] = 4; // count bytes

    ptrChr = (uint8_t *)&newIdFloat;
    buffer[10] = *ptrChr++;
    buffer[9] = *ptrChr++;
    buffer[8] = *ptrChr++;
    buffer[7] = *ptrChr++;
    msgCRC = CRC16(buffer, 11);
    buffer[11] = msgCRC & 0xFF;
    buffer[12] = (msgCRC & 0xFF00) >>8;
    osalThreadSleepMilliseconds(5);
    modbus::chprintStrRs485(buffer, 13, chTimeMS2I(200));
    *error = modbus::chReadStrRs485(bufferRx, 8, &bytesReceived, chTimeMS2I(300));
    if (*error!=0 || bytesReceived!=8)
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






