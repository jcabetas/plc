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

#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"
#include "tty.h"
#include "colas.h"

/*
    // MODBUS 1 9600
protected:
    uint16_t numModBus;
    uint16_t baudios;
};

 Como funciona:
 - Crear IO modbus, con MODBUS (Numero de modbus, baudios)
    MODBUS 1 9600
 - Crear dispositivo que use el MODBUS, p.e. SDM120CT con direccion=2
    SDM120CT MedidorFlexo 1 2 (nombreDispo, InterfaceModbus#1, direccion #2)
 - Crear medidas, actualizable cada 3s y 20s
    MEDIDA WFlexo MedidorFlexo W 3 (nombreMedida, nombreDispo, nombreDatos, periodicidad)
                                    + fechaUltimaMedida, esValida, errorMedida
    MEDIDA Tension MedidorFlexo V 20s
 - Utilizar la medida
    ASIGNAVAR intensidad WFlexo Tension /
    ASIGNAESTADO encendido.base intensidad 2 >

 Opciones:
 - Thread a nivel de Modbus. Ese thread conoce modelo de dispositivo y datos de las medidas (quizas por funciones)
     Para cada medida, guarda #dispositivo, #medida y fecha en la que se debe leer
     Hay que fijar la estrategia de lectura. Si en principio podemos usar la fecha propuesta por la medida
     habra que resolver conflictos; quizas dando un peso que aumente con el retraso respecto a lo deseado
 */

uint16_t modbus::baudios = 9600;
uint8_t modbus::definido = 0;
modbus *modbus::modbusPtr = NULL;
thread_t *modbus::procesoModbus = NULL;
dispositivo *modbus::listDispositivosMB[MAXDISPOSITIVOSMB] = {0};
uint8_t modbus::errorEnDispMB[MAXDISPOSITIVOSMB] = {0};
uint32_t  modbus::numErrores[MAXDISPOSITIVOSMB] = {0};
uint32_t  modbus::numMs[MAXDISPOSITIVOSMB] = {0};
uint32_t  modbus::numAccesos[MAXDISPOSITIVOSMB] = {0};
float modbus::tMedio[MAXDISPOSITIVOSMB] = {0};
uint16_t modbus::numDispositivosMB = 0;

uint16_t testMB::idPage;
uint16_t testMB::idNombre;
uint32_t testMB::baudios;
uint8_t testMB::direccion;
uint8_t testMB::funcion;
uint8_t testMB::datosTx[30];
uint8_t testMB::datosRx[30];
uint8_t testMB::longTx;
uint16_t testMB::longRx;

uint16_t CRC16(const uint8_t *nData, uint16_t wLength);
event_source_t testMB_source;

/*
 * Thread modbus
 */
static THD_WORKING_AREA(wamodbus, 3000);
static THD_FUNCTION(modbusThrd, arg) {
    (void)arg;
    event_listener_t el0;
    chRegSetThreadName("modbus");
    uint16_t iter = 0;
    chEvtRegister(&testMB_source, &el0, 0);
    while (true) {
        if (++iter == 100)
        {
            modbus::leeTodos(1);
            iter = 0;
        }
        modbus::leeTodos(0);
        eventmask_t evento = chEvtWaitAnyTimeout(ALL_EVENTS, TIME_MS2I(20));
        if (evento!=0)
        {
            // he recibido evento de sacar por modbus
            // @testMB,9600,1,1,[88 22 00 BB]\x00
            testMB::leeMB();
        }
        //chThdSleepMilliseconds(20);
        if (chThdShouldTerminateX())
        {
            chEvtUnregister(&testMB_source, &el0);
            chThdExit((msg_t) 1);
        }
    }
}





// MODBUS 9600
modbus::modbus(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=2)
    {
        if (tty!=NULL)
            chprintf(tty,"#parametros incorrecto MODBUS\n\r");
        else
            nextion::enviaLog(tty,"#parametros MODBUS");
        *hayError = 1;
        return; // error
    }
    if (definido)
    {
        nextion::enviaLog(tty,"#MODBUS repetido");
        *hayError = 1;
    }
    baudios = atoi(pars[1]);
    for (uint8_t d=1;d<MAXDISPOSITIVOSMB;d++)
    {
        listDispositivosMB[d-1] = NULL;
        errorEnDispMB[d-1] = 0;
    }
    numDispositivosMB = 0;
    procesoModbus = NULL;
    modbusPtr = this;
    definido = 1;
};

// MODBUS 9600
modbus::modbus(uint16_t baudiosPar)
{
    if (definido)
    {
        chprintf((BaseSequentialStream *)&SD1,"Error: solo debe haber un objeto MODBUS\n");
        return;
    }
    baudios = baudiosPar;
    for (uint8_t d=1;d<MAXDISPOSITIVOS;d++)
    {
        listDispositivosMB[d-1] = NULL;
        errorEnDispMB[d-1] = 0;
    }
    numDispositivosMB = 0;
    procesoModbus = NULL;
    modbusPtr = this;
    definido = 1;
};

uint32_t modbus::diBaudios(void)
{
    return baudios;
}


void modbus::deleteMB(void)
{
    for (uint8_t d=0;d<MAXDISPOSITIVOSMB;d++)
        listDispositivosMB[d] = NULL;
    numDispositivosMB = 0;
    definido = 0;
}

void modbus::addDisp(dispositivo *disp)
{
    if (numDispositivosMB>=MAXDISPOSITIVOSMB)
    {
        nextion::enviaLog(NULL,"#demasiados dispositivos MB");
        return;
    }
    listDispositivosMB[numDispositivosMB] = disp;
    errorEnDispMB[numDispositivosMB] = 0;
    numDispositivosMB++;
}

void modbus::leeTodos(uint8_t incluyeErroneos)
{
    uint8_t error;
    char buffer[80];
    /*
     *      static uint32_t  numMs[MAXDISPOSITIVOSMB];
            static uint32_t  numAccesos[MAXDISPOSITIVOSMB];
            static float tMedio[MAXDISPOSITIVOSMB];
     */
    systime_t tIni, tFin;
    for (uint8_t d=1;d<=numDispositivosMB;d++)
    {
        if (incluyeErroneos || errorEnDispMB[d-1]==0)
        {
            dispositivo *disp = listDispositivosMB[d-1];
            if (disp!=NULL)
            {
                tIni = chVTGetSystemTimeX();
                error = disp->usaBus(); // 0: no error, 1: error, 2: no se ha usado
                if (error==1 && errorEnDispMB[d-1]!=1)
                {
                    numErrores[d-1]++;
                    error = disp->usaBus(); // si es un error suelto, reintenta
                }
                if (error==0)
                {
                    tFin = chVTGetSystemTimeX();
                    numMs[d-1] += chTimeI2MS(chTimeDiffX(tIni, tFin));
                    numAccesos[d-1]++;
                    tMedio[d-1] = (float) (numMs[d-1]/numAccesos[d-1]);
                }
                if (error!=2 && error != errorEnDispMB[d-1])
                {
                    if (error)
                        chsnprintf(buffer,sizeof(buffer),"Error en %s",nombres::nomConId(disp->diIdNombre()));
                    else
                        chsnprintf(buffer,sizeof(buffer),"Error recuperado en %s",nombres::nomConId(disp->diIdNombre()));
                    nextion::enviaLog(NULL,buffer);
                    registraEnLog(buffer);
                }
                if (error!=2)
                    errorEnDispMB[d-1] = error;
            }
        }
    }
}

int16_t modbus::chReadStrRs485(uint8_t *buffer, uint16_t numBytesExpected, uint16_t *bytesReceived,  sysinterval_t timeout)
{
    size_t nb;
    palClearLine(LINE_TXRX);
    nb = sdReadTimeout(&MBSD,buffer,numBytesExpected,timeout);
    *bytesReceived = nb;
    if (*bytesReceived != numBytesExpected) return 1;
    return 0;
}

int16_t modbus::chprintStrRs485(uint8_t *str, uint16_t lenStr, sysinterval_t timeout)
{
    eventmask_t evt;
    event_listener_t endEot_event;
    chEvtRegisterMaskWithFlags (chnGetEventSource(&MBSD),&endEot_event, EVENT_MASK (0),CHN_TRANSMISSION_END);
    palSetLine(LINE_TXRX);
    chThdSleepMilliseconds(2);
    sdWrite(&MBSD, str,lenStr);
    while (true)
    {
        evt = chEvtWaitAnyTimeout(ALL_EVENTS, timeout);
        if (evt==0) // Timeout
            break;
        if (evt & EVENT_MASK(0)) // Evento fin de transmision, limpio RX/TX
            break;
    }
    palClearLine(LINE_TXRX);
    chEvtUnregister(chnGetEventSource(&MBSD),&endEot_event);
    if (evt==0)
        return 1;
    return 0;
}

int8_t modbus::init(void)
{
    SerialConfig configSD2;
    palClearLine(LINE_TXRX);
    palSetLineMode(LINE_TXRX, PAL_MODE_OUTPUT_PUSHPULL);
    configSD2.speed = baudios;
    configSD2.cr1 = 0;
    configSD2.cr2 = USART_CR2_STOP1_BITS;// | USART_CR2_LINEN;;
    configSD2.cr3 = 0;
    sdStart(&MBSD, &configSD2);
    if (definido && modbusPtr!=NULL && procesoModbus==NULL)
        procesoModbus = chThdCreateStatic(wamodbus, sizeof(wamodbus), NORMALPRIO, modbusThrd, NULL);
    else
        return 1;
    return 0;
}

void modbus::stop(void)
{
    if (procesoModbus!=NULL)
    {
        chThdTerminate(procesoModbus);
        chThdWait(procesoModbus);
        procesoModbus = NULL;
    }
    sdStop(&MBSD);
}

void modbus::leeMBfunc04(uint8_t dirMB, uint16_t addressReg, uint8_t numRegs, uint8_t bufferRx[], uint16_t sizeofbufferRx, uint16_t msDelayMax, uint16_t *msDelay, int16_t *error)
{
    uint8_t buffer[10];
    uint16_t msgCRC, rxCRC, bytesReceived;
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
    systime_t start = chVTGetSystemTime();
    *error = modbus::chReadStrRs485(bufferRx, 1, &bytesReceived, chTimeMS2I(msDelayMax));
    if (*error!=0 || bytesReceived!=1)
    {
        *error = -1;
        return;
    }
    sysinterval_t duracion = chVTTimeElapsedSinceX(start);
    *error = modbus::chReadStrRs485(&bufferRx[1], 4+2*numRegs, &bytesReceived, chTimeMS2I(10));
    if (*error!=0 || bytesReceived!=(4+2*numRegs))
    {
        *error = -2;
        return;
    }
    msgCRC = CRC16(bufferRx, bytesReceived-1);
    rxCRC = (bufferRx[bytesReceived]<<8) + bufferRx[bytesReceived-1];
    if (msgCRC!=rxCRC || dirMB!= bufferRx[0] || bufferRx[1]!=0x04)
    {
        *error = -3;
        return;
    }
    *msDelay = chTimeI2MS(duracion);
    return;
}

modbus::~modbus()
{
    numDispositivosMB = 0;
    procesoModbus = NULL;
    modbusPtr = NULL;
    definido = 0;
}

const char *modbus::diTipo(void)
{
    return "modbus";
}

const char *modbus::diNombre(void)
{
    return "MODBUS";
}

void modbus::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  )
{
}

void modbus::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"MODBUS %d baud, %d disp.",baudios, numDispositivosMB);
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}


//
//class testMB
//{
//    static uint32_t baudios;
//    static uint8_t direccion;
//    static uint8_t funcion;
//    static uint8_t datosTx[30];
//    static uint8_t datosRx[30];
//    static uint8_t longTx;
//    static uint8_t longRx;
//static uint16_t idPage;
//static uint16_t idNombre;
//public:
//    static void leeDatosNextion(uint8_t numPar, char *pars[]);
//};

uint8_t testMB::leeDatosNextion(uint8_t numPar, char *pars[])
{
    //@testMB,modbus,respuesta,9600,1,1,88 B 22\x00
    uint32_t numHex;
    uint16_t msgCRC;
    idPage = nombres::incorpora(pars[1]);
    idNombre = nombres::incorpora(pars[2]);
    baudios = atoi(pars[3]);
    direccion = atoi(pars[4]);
    int8_t error = HexStr2Int((uint8_t *) pars[5],&numHex);
    if (error!=0)
        return 1;
    funcion = (uint16_t) numHex;
    // lo paso a datosTx
    datosTx[0] = direccion;
    datosTx[1] = funcion;
    for (uint8_t i=6;i<numPar;i++)
    {
        int8_t error = HexStr2Int((uint8_t *) pars[i],&numHex);
        if (error!=0)
            return 1;
        datosTx[i-4] = (uint8_t) numHex;
    }
    longTx = numPar - 4;
    msgCRC = CRC16(datosTx, longTx);
    datosTx[longTx++] = msgCRC & 0xFF;
    datosTx[longTx++] = (msgCRC & 0xFF00) >>8;
    return 0;
}

void testMB::leeMB(void)
{
    SerialConfig configSD;
    int16_t error;
    uint16_t msgCRC, rxCRC;
    char buff[60];
    // ajusto la velocidad del bus
    sdStop(&MBSD);
    configSD.speed = baudios;
    configSD.cr1 = 0;
    configSD.cr2 = USART_CR2_STOP1_BITS;// | USART_CR2_LINEN;;
    configSD.cr3 = 0;
    sdStart(&MBSD, &configSD);
    // envio datos
    modbus::chprintStrRs485(datosTx, longTx, chTimeMS2I(50));
    modbus::chReadStrRs485(datosRx, sizeof(datosRx), &longRx, chTimeMS2I(500));
    msgCRC = modbus::CRC16(datosRx, longRx-2);
    rxCRC = (datosRx[longRx-1]<<8) + datosRx[longRx-2];
    if (msgCRC==rxCRC)
        error=0;
    else
        error=1;
    buff[0] = 0;
    for (uint8_t i=1; i<=longRx; i++)
    {
        chsnprintf(buff,sizeof(buff),"%s %X",buff,datosRx[i-1]);
    }
    if (error)
        chsnprintf(buff,sizeof(buff),"%s Error CRC (%X vs %X)",buff,msgCRC,rxCRC);
    enviaTxt(idPage, idNombre, buff);
    // restablece bus
    sdStop(&MBSD);
    configSD.speed = modbus::diBaudios();
    configSD.cr1 = 0;
    configSD.cr2 = USART_CR2_STOP1_BITS;// | USART_CR2_LINEN;;
    configSD.cr3 = 0;
    sdStart(&MBSD, &configSD);
}


extern "C"
{
    void testMB(void);
}

void testMB(void)
{
    sdm120ct *flexo;
    uint8_t error;

    modbus *modbusObj = new modbus(9600);

    flexo = new sdm120ct("flexo",2);
    medida *medV = new medida("flexoV",20,2,"flexo",&error);
    flexo->attachMedidas(medV, "V", &error);
    modbusObj->addDisp(flexo);

//    inversorRiba = new vacon("inversor", 1);
//    inversorRiba->attachMedida(&frecuencia, "Hz", 20, "Frecuencia");
//    inversorRiba->attachMedida(&frecuencia, "AI2%", 10, "Presion");
//    modbusObj->addDisp(inversorRiba);

    modbusObj->init();
}
