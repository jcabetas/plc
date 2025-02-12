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
- Crear dispositivo que use el MODBUS, p.e. t80 con direccion=2
   T80 litrosDeposito 0.1 2 (nombreDispo, factorMultiplicador, direccion)
*/

uint16_t CRC16(const uint8_t *nData, uint16_t wLength);

/*
class t80 : public dispositivo
{
    // t80 MedidorFlexo 8
protected:
    uint16_t idNombreDisp;
    uint16_t direccion;
    medida   *volumen;
    float factorMultiplicador;
    uint8_t  erroresSeguidos;
public:
    t80(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~t80();
    void changeID(uint8_t oldId, uint8_t newId, int16_t *error);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(float *valor, int16_t *error);
};
*/

t80::t80(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    char buffer[50];
    // T80 MedidorDeposito 0.1 2 (nombreDispo, medida, factor, direccion)
    if (numPar!=4)
    {
        nextion::enviaLog(tty,"#parametros incorrecto t80");
        *hayError = 1;
        return;
    }
    idNombreDisp = nombres::incorpora(pars[1]);
    factorMultiplicador = atof(pars[2]);
    direccion = atoi(pars[3]);
    erroresSeguidos = 0;
    volumen = medida::findMedida(pars[1]);
    if (volumen==NULL)
    {
        chsnprintf(buffer,sizeof(buffer),"en T80, no encuentro medida %s",pars[1]);
        nextion::enviaLog(tty,buffer);
        *hayError = 3;
        return; // error
    }
    modbus::addDisp(this);
};

t80::~t80()
{
}

uint16_t t80::diIdNombre(void)
{
    return idNombreDisp;
}

const char *t80::diTipo(void)
{
    return "t80";
}


int8_t t80::init(void)
{
    // no necesita inicializacion
    return 0;
}


void t80::leer(float *valor, int16_t *error)
{
    float valor2;
    uint16_t bytesReceived;
    uint16_t msgCRC, rxCRC;
    uint8_t buffer[20];
    uint8_t bufferRx[20];

    /*
     * Example
    retraso 3.5 carateres (<>5ms)
    Address 01
    Function code 02
    AddrH 0
    AddrL 0
    numRegsH 0
    numRegsL 2
    CRC C40B
    retraso 3.5
     */
    // leer PUH
    buffer[0] = direccion;
    buffer[1] = 0x03;
    buffer[2] = 0; // registro 0000
    buffer[3] = 0;
    buffer[4] = 0; // leer 2 bytes
    buffer[5] = 2;

    msgCRC = CRC16(buffer, 6);
    buffer[6] = msgCRC & 0xFF;
    buffer[7] = (msgCRC & 0xFF00) >>8;
    bufferRx[0] = 0;
    bufferRx[1] = 0;
    chThdSleepMilliseconds(5);
    modbus::chprintStrRs485(buffer, 8, chTimeMS2I(100));
    *error = modbus::chReadStrRs485(bufferRx, 9, &bytesReceived, chTimeMS2I(100));
    chThdSleepMilliseconds(5);
    if (*error!=0 || bytesReceived!=9)
    {
        *error = -1;
        *valor = 0.0f;
        return;
    }
    msgCRC = CRC16(bufferRx, bytesReceived-2);
    rxCRC = (bufferRx[bytesReceived-1]<<8) + bufferRx[bytesReceived-2];
    if (msgCRC!=rxCRC || direccion!= bufferRx[0] || bufferRx[1]!=0x03)
    {
        *error = -2;
        *valor = 0.0f;
        return;
    }
    int16_t valorInt = (bufferRx[3]<<8) + bufferRx[4];
    valor2 = 1.0f*valorInt;
    if (bufferRx[6]==1)
        valor2 /= 10.0f;
    if (bufferRx[6]==2)
        valor2 /= 100.0f;
    *error = 0;
    *valor = valor2;
    return;
}

// retorna 1 si hay error
// mejor: 0: no error, 1: error, 2: no se ha usado
uint8_t t80::usaBus(void)
{
    float valor;
    int16_t error;
    uint8_t heEnviado = 1;
    if (volumen==NULL)
        return 0;
    if (volumen->necesitaUpdate())
    {
        leer(&valor, &error);
        heEnviado = 1;
        if (error!=0)
        {
            erroresSeguidos++;
            // vuelve a intentarlo
            leer(&valor, &error);
            if (error!=0)
            {
                erroresSeguidos++;
                if (erroresSeguidos>4)
                {
                    volumen->set(&valor, 1); // si hay error, vale 0.0f
                    volumen->setValidez(0);
                }
                return 2;
            }
        }
        else
            erroresSeguidos = 0;
        valor *= factorMultiplicador;
        volumen->set(&valor, 0);
    }
    if (heEnviado)
        return 0;
    else
        return 2;
}

