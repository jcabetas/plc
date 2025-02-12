/*
 * medidas.h
 *
 *  Created on: 19 abr. 2021
 *      Author: joaquin
 */

#ifndef DISPOSITIVOS_DISPOSITIVOS_H_
#define DISPOSITIVOS_DISPOSITIVOS_H_

#include "bloques.h"
#include "calendar.h"
#include "ArduinoJson.h"

/*
- clase basica de interfaz
  cada interface conoce los dispositivos conectados
  cada dispositivo conoce las medidas o estados que puede leer/escribir
  pero seria interesante que cada interfaz conociera todas las medidas

  Interface (modbus1) -> dispositivo A (sdm120ct) -> medida (kW)
  El thread debería:
  - Mirar cual es la medida/estados/otros mas urgentes
  - Usar el dispositivo correspondiente para leerlo

  Eso lleva a:
  Interface (modbus1) -> medida (kW) -> dispositivo A (sdm120ct)

*/

#define MAXDISPOSITIVOS     40
#define MAXDISPOSITIVOSMB   15
#define MAXDISPOSITIVOSI2C   6
#define MAXDISPOSITIVOSMQTT  15

#define MAXMEDENDISPOSI  10
#define MAXPLACASRELESMB  4
#define MAXMEDIDAS       35
#define REGSINMEDMAX      5

#define MAXMED2CAN        10
#define MAXCAN2MED        10
#define MAXCAN2STAT       5
#define MAXSTAT2CAN       5

#define MBSD   SD4

uint16_t CRC16(const uint8_t *nData, uint16_t wLength);

class dispositivo;
class medida;
class medidaMax;
class pinOut;
class pinInput;

/*
- clase basica de interfaz
  cada interface conoce los dispositivos conectados
  cada dispositivo conoce las medidas o estados que puede leer/escribir
  pero seria interesante que cada interfaz conociera todas las medidas

  Interface (modbus1) -> dispositivo A (sdm120ct) -> medida (kW)
  El thread debería:
  - Mirar cual es la medida/estados/otros mas urgentes
  - Usar el dispositivo correspondiente para leerlo

  Eso lleva a:
  Interface (modbus1) -> medida (kW) -> dispositivo A (sdm120ct)

*/

class modbus : bloque
{
    // MODBUS 9600
protected:
    static uint16_t baudios;
    static uint8_t definido;
    static modbus *modbusPtr;
    static thread_t *procesoModbus;
    static dispositivo *listDispositivosMB[MAXDISPOSITIVOSMB];
    static uint8_t  errorEnDispMB[MAXDISPOSITIVOSMB];
    static uint32_t  numErrores[MAXDISPOSITIVOSMB];
    static uint32_t  numMs[MAXDISPOSITIVOSMB];
    static uint32_t  numAccesos[MAXDISPOSITIVOSMB];
    static float tMedio[MAXDISPOSITIVOSMB];
    static uint16_t numDispositivosMB;
public:
    modbus(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    modbus(uint16_t baudiosPar);
    ~modbus();
    static void deleteMB(void);
    static uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachInputPlacaMadre(pinInput *inpPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t usaBus(void);
    static uint16_t diIdNombre(void);
    static uint32_t diBaudios(void);

    static void addDisp(dispositivo *disp);
    static void leeMBfunc04(uint8_t dirMB, uint16_t addressReg, uint8_t numRegs, uint8_t bufferRx[], uint16_t sizeofbufferRx, uint16_t msDelayMax, uint16_t *msDelay, int16_t *error);
    static void leeTodos(uint8_t incluyeErroneos);
    static dispositivo *findDispositivo(uint16_t idNombre);
    static int16_t chReadStrRs485(uint8_t *buffer, uint16_t numBytesExpected, uint16_t *bytesReceived,  sysinterval_t timeout);
    static int16_t chprintStrRs485(uint8_t *str, uint16_t lenStr, sysinterval_t timeout);
    static uint16_t CRC16(const uint8_t *nData, uint16_t wLength);
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    void stop(void);
    void addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  );
    void print(BaseSequentialStream *tty);
};





class i2cthread : bloque
{
    // I2CBUS
protected:
    static uint8_t definido;
    static i2cthread *i2cPtr;
    static thread_t *procesoI2C;
    static dispositivo *listDispositivosI2C[MAXDISPOSITIVOSI2C];
    static uint8_t  errorEnDispI2C[MAXDISPOSITIVOSI2C];
    static uint16_t numDispositivosI2C;
    static mutex_t MtxI2C;
public:
    i2cthread(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    ~i2cthread();
    static uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachInputPlacaMadre(pinInput *inpPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t usaBus(void);
    static uint16_t diIdNombre(void);
    static void resetI2C(void);
    static void initIODIR(uint16_t dirTarjeta, uint8_t dirIO, uint8_t *error);
    static uint32_t leeIODIR(uint16_t dirTarjeta, uint8_t *error);
    static uint32_t leePullUp(uint16_t dirTarjeta, uint8_t *error);
    static void  initPullUp(uint16_t dirTarjeta, uint8_t *error);
    static void writeAndRead2Bytes(uint16_t dirTarjeta, uint16_t *valor, uint8_t *error);
    static void writeOutput(uint16_t dirTarjeta, uint16_t valorAEscribir, uint8_t *error);
    static void lockI2C(void);
    static void unlockI2C(void);

    static void addDisp(dispositivo *disp);
    static void leeTodos(uint8_t incluyeErroneos);
    static dispositivo *findDispositivo(uint16_t idNombre);
    static int16_t chReadStrRs485(uint8_t *buffer, uint16_t numBytesExpected, uint16_t *bytesReceived,  sysinterval_t timeout);
    static int16_t chprintStrRs485(uint8_t *str, uint16_t lenStr, sysinterval_t timeout);
    static uint16_t CRC16(const uint8_t *nData, uint16_t wLength);
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    void stop(void);
    void addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  );
    void print(BaseSequentialStream *tty);
};

enum tipoMsgCan_t {cambiaID=1, medidaCan, state,confCargador,setSP, estadoCargador, potenciaVE, medidaCasa, IVE, pContratadas, setFechaHora};
class can : bloque
{
protected:
    static uint8_t definido;
    static uint8_t idCan;
    static can *canPtr;
    static thread_t *procesoCANRX;
    static thread_t *procesoCANTX;
    static medida  *can2med[MAXCAN2MED];
    static uint16_t idCan2Med[MAXCAN2MED];
    static uint8_t  numCan2Med;
    static uint16_t can2state[MAXCAN2STAT];
    static uint16_t idCan2state[MAXCAN2STAT];
    static uint16_t numCan2state;
    static uint16_t state2can[MAXSTAT2CAN];
    static uint16_t idState2can[MAXSTAT2CAN];
    static uint16_t numState2can;
public:
    can(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    ~can();
    static uint8_t attachMed2Can(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachCan2Med(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachCan2State(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachState2Can(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint16_t diIdNombre(void);
    static uint8_t leeMsg(CANRxFrame rxmsg);
    static uint8_t sendMsg(void);
    static uint8_t med2msg(uint16_t idCan, float *valor);
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    void stop(void);
    void addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  );
    void print(BaseSequentialStream *tty);
};


class pinOut: public bloque
{
//  pinOut relesSalidas calefOut 4
protected:
    uint16_t numEstado;
    dispositivo *disp;
    uint16_t idNombDisp;
public:
    pinOut(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~pinOut();
    uint8_t getValor(void);
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    uint16_t diTiempo(uint8_t esB);
    void ponSalida(uint8_t estado);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t msInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void trataOrdenNextion(char **Vars, uint16_t numVars);
    void printStatus(char *buffer, uint8_t longBuffer);
};

class pinInput: public bloque
{
//  debe hacer un attachInput.
//  La placa lee el estado del periferico y el pin lo actualiza en el estado
//  pinInput placasEntrada boyaAlta 2
protected:
    uint16_t numEstado;
    dispositivo *disp;
    uint16_t idNombDisp;
public:
    pinInput(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~pinInput();
    uint8_t attachInputPlacaMadre(uint8_t numPin, uint8_t *hayError);
    void setValor(uint8_t valor);
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    uint8_t calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    uint16_t diTiempo(uint8_t esB);
    void ponSalida(uint8_t estado);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t msInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void trataOrdenNextion(char **Vars, uint16_t numVars);
    void printStatus(char *buffer, uint8_t longBuffer);
};


class med2can: public bloque
{
//  cuando hay una medida se transmite all busCAN
//  esto solo sirve de buffer, para iniciarse cuando esten todos los bloque definidos
protected:
    static uint16_t  nomMed2Can[MAXMED2CAN];
    static uint16_t idMed2Can[MAXMED2CAN];
    static uint8_t  numMed2Can;
public:
    ~med2can();
    static uint8_t addMed2Can(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t initMed2Can(void);
    static void deleteAll(void);
};

class dispositivo
{
private:
    static dispositivo *listDispositivos[MAXDISPOSITIVOS];
    static uint8_t numDispositivos;
public:
    dispositivo();
    virtual ~dispositivo() = 0;
    static dispositivo *findDispositivo(uint16_t idNombre);
    static void deleteAll(void);
    virtual uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    virtual uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    virtual uint8_t attachInputPlacaMadre(pinInput *inpPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    virtual uint8_t usaBus(void) = 0;
    virtual uint16_t diIdNombre(void) = 0;
    virtual const char *diTipo(void) = 0;
    virtual int8_t init(void) = 0;
};



//struct conexMqtt_t {
//    uint16_t idHost;
//    uint16_t port;
//    uint16_t idUsuario;
//    uint16_t idPassword;
//};

class dispMqtt
{
protected:
    int8_t numDispMqtt;
public:
    virtual void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds) = 0;
    virtual void trataRx(const char *valor, const char *info) = 0;
    virtual void suscribir(uint8_t *todoSuscrito) = 0;
    virtual void actualizaEstSusc(uint8_t estadoNuevo) = 0;
    virtual void initMqtt(void) = 0;
};

class mqttRaspi : bloque
{
    // MQTT 192.168.8.30 1883 joaquin JeW31ZT9Rdx
protected:
    static uint16_t idHost;
    static uint16_t port;
    static uint16_t idUsuario;
    static uint16_t idPassword;
    static uint8_t definido;
    static uint8_t estado;
    static mqttRaspi *mqttRaspiPtr;
    static dispMqtt *listDispositivosMqtt[MAXDISPOSITIVOSMQTT];
    static uint8_t numDispositivosMqtt;
    static uint16_t dsEnEstado;
    static void conectarMqtt(void);
public:
    mqttRaspi(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    ~mqttRaspi();
    static int8_t addDisp(dispMqtt *disp);
    static dispMqtt *findDispositivo(uint16_t idNombre);
    static uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t attachInputPlacaMadre(pinInput *inpPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    static uint8_t usaBus(void);
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    void addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  );
    void print(BaseSequentialStream *tty);
    void initWWW(BaseSequentialStream *SDPort, uint8_t *heVolcado);
    static void initMqtt(void);
    static void enviaSolicitudConex(void);
    static void actualizaEstadoConex(uint8_t estado);
    static void mqttDispSuscrito(uint16_t numDisp);
    static void mqttMsgRx(uint16_t numDisp, const char *valor, const char *info);
};

class sonoff : public dispositivo, public dispMqtt
{
    // SONOFFPOW 3E5176 calCocina Pcocina   EnerCocina
    //           nombre pinOut    medidaP   medidaEner
protected:
    uint16_t idNombre;
    pinOut   *pinSalida;
    medida   *medidaP;
    medida   *medidaEner;
    uint8_t  estadoSolicitado;
    uint8_t  estadoReportado;
    uint16_t dsOutDiferente;
    uint8_t  suscrito;
    uint8_t  erroresSeguidos;
    struct fechaHora dateTimeEnvioAnterior;
public:
    sonoff(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~sonoff();
    uint16_t diIdNombre(void);
    //char *diNombre(void);
    const char *diTipo(void);
    uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char **, uint8_t *hayError);
    int8_t init(void);
    uint8_t usaBus(void);
    void initMqtt(void);
    void enviaSolicitudSusc(void);
    void suscribir(uint8_t *todoSuscrito);
    void actualizaEstSusc(uint8_t estadoNuevo);
    void trataRx(const char *valor, const char *info);
    void ponSalida(uint8_t valorDeseado);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void leer(float *valor, uint16_t addressReg, int16_t *error);
    void print(BaseSequentialStream *tty);
};


class mqtt2status : public dispositivo, public dispMqtt
{
    // MQTT2STATUS /villagolf/presenciaGaraje hayGente
protected:
    uint16_t idNombre;
    uint8_t  numEstado;
    uint8_t  suscrito;
    uint8_t  erroresSeguidos;
    uint16_t dsSinRx;
    struct fechaHora dateTimeEnvioAnterior;
public:
    mqtt2status(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~mqtt2status();
    uint16_t diIdNombre(void);
    //char *diNombre(void);
    const char *diTipo(void);
//    uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char **, uint8_t *hayError);
    int8_t init(void);
    uint8_t usaBus(void);
    void initMqtt(void);
    void enviaSolicitudSusc(void);
    void suscribir(uint8_t *todoSuscrito);
    void actualizaEstSusc(uint8_t estadoNuevo);
    void trataRx(const char *valor, const char *info);
    void ponSalida(uint8_t valorDeseado);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void leer(float *valor, uint16_t addressReg, int16_t *error);
    void print(BaseSequentialStream *tty);
};

class medida : public bloque
{
   //    MEDIDA WFlexo 3 1 ... (nombreMedida, nombreCompleto, periodicidad, numDecimales ... [nombreDispo, definicionesDispositivos])
   // + fechaUltimaMedida, esValida, errorMedida
protected:
    uint16_t idNombre;
    uint16_t idNombreWWW;
    uint16_t idPageNxt;
    uint8_t tipoNxt;
    uint8_t picBaseNxt;
    uint8_t decimales;
    struct fechaHora dateTimeEnvioNextionAnterior;
    float valor;
    uint16_t idNombDispositivo;
    uint16_t ds;
    uint16_t dsObj;
    uint8_t medidaValida;
    uint8_t inicializada;;
    uint8_t enviadaANextionyWWW;
    mutex_t MtxMedida;
    medidaMax *medidaDeMaximos;
    uint16_t idCan;
    uint16_t idNombTopic;
    char valorStrEnviado[10];
    static medida *medidas[MAXMEDIDAS];
    static uint16_t numMedidas;
public:
    medida(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    medida(const char *nombMedida, uint16_t ds, uint8_t numDecimales, const char *nombDisp, uint8_t *hayError);
//    medida(BaseSequentialStream *tty, char *nombreMed, uint8_t numDecimales, uint16_t dsObjetivo, uint8_t *hayError);
    ~medida();
    void attachCan(uint16_t idCAN);
    static void attachMqtt(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError);
    static void deleteAll(void);
    uint8_t necesitaUpdate(void);
    const char *diTipo(void);
    const char *diNombre(void);
    float diValor(void);
    static medida *findMedida(const char *nombre);
    void set(float *valor, uint8_t fuerzaAct);
    uint8_t diDecimales(void);
    uint8_t esVieja(void);
    void attachMedidaMax(medidaMax *medMax);
    uint8_t esValida(void);
    void setValidez(uint8_t esBuena);
    int8_t init(void);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void actualizaNextionyWWW(uint8_t fuerza);
    void initWWW(BaseSequentialStream *SDPort, uint8_t *hayDatosVolcados);
    void enviaWWW(char *valorStr);
};

class medidaMax : public bloque
{
    //    MEDIDAMAX WFlexoMax WFlexo 20 100 (nombre, nombreMedida, numRegistros dsEntreRegs)
protected:
    uint16_t idNombre;
    uint16_t idPageNxt;
    uint16_t dsEntreRegs;
    medida *medidaOrigen;
    medida *medidaMaxima;
    uint8_t ptrRegActual;
    struct fechaHora dateTimeRegAnterior;
    float valorMaxRegActual;
    float valoresReg[REGSINMEDMAX];
public:
    medidaMax(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~medidaMax();
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    void nuevaMedida(float *med);
    void setValidez(uint8_t esBuena);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void actualizaNextionyWWW(void);
};

class sdm120ct : public dispositivo
{
    // SDM120CT MedidorFlexo 2
protected:
    uint16_t idNombreDisp;
    uint16_t direccion;
    medida   *listMedidas[MAXMEDENDISPOSI];
    uint8_t  tipo[MAXMEDENDISPOSI];
    uint8_t  numMedidas;
    uint8_t  erroresSeguidos;
public:
    sdm120ct(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    sdm120ct(const char *nombrePar, uint16_t dirPar);
    ~sdm120ct();
    void changeID(uint8_t oldId, uint8_t newId, int16_t *error);
    uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t attachMedidas(medida *med, const char *tipMedida, uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(float *valor, uint16_t addressReg, int16_t *error);
};


//xymd02
class xymd02 : public dispositivo
{
    // xymd02 tempSalon 2
protected:
    uint16_t idNombreDisp;
    uint16_t direccion;
    medida   *medidaTemp;
    medida   *medidaHum;
    uint8_t  numMedidas;
    uint8_t  erroresSeguidos;
public:
    xymd02(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~xymd02();
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(float *temp, float *humedad, int16_t *error);
};




class sdm630ct : public dispositivo
{
    // SDM630CT MedidorFlexo 2
protected:
    uint16_t idNombreDisp;
    uint16_t direccion;
    medida   *listMedidas[MAXMEDENDISPOSI];
    uint8_t  tipo[MAXMEDENDISPOSI];
    uint8_t  numMedidas;
    uint8_t  erroresSeguidos;
public:
    sdm630ct(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~sdm630ct();
    void changeID(uint8_t oldId, uint8_t newId, int16_t *error);
    uint8_t attachMedidas(medida *med, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(float *valor, uint16_t addressReg, int16_t *error);
};

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

class adm4240 : public dispositivo
{
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
};



class relesMB: public dispositivo
{
protected:
    uint16_t idNombre;
    uint8_t address;
    uint16_t numEstadoPlaca; // indica si esta ok (cuando esta a 1)
    pinOut *pinesMB[16];
    uint8_t numRele[16];
    uint8_t estadoEnviado[16];
    uint8_t numPines;
    struct fechaHora dateTimeLectAnterior;
public:
    relesMB(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~relesMB();
    uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void escribir(uint8_t numPin, uint8_t valorDeseado, int16_t *error);
};


class reles8Input8MB: public dispositivo
{
protected:
    uint16_t idNombre;
    uint8_t address;
    uint16_t numEstadoPlaca; // indica si esta ok (cuando esta a 1)
    // salidas
    pinOut *pinesRelesMB[8];
    uint8_t numRele[8];
    uint8_t estadoEnviado[8];
    uint8_t numPinesReles;
    struct fechaHora dateTimeEscrituraAnterior;
    // entradas
    pinInput *pinesInputMB[8];
    uint8_t numInput[8];
    uint8_t estado[8];
    uint8_t numPinesInput;
    uint16_t minDs;
    struct fechaHora dateTimeLectAnterior;
public:
    reles8Input8MB(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~reles8Input8MB();
    uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t attachInputPlacaMadre(pinInput *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void escribir(uint8_t numPin, uint8_t valorDeseado, int16_t *error);
    void leer(uint16_t *resultado, int16_t *error);
};

class N4DIG08: public dispositivo
{
protected:
    uint16_t idNombre;
    uint8_t address;
    uint16_t numEstadoPlaca; // indica si esta ok (cuando esta a 1)
    // entradas
    pinInput *pinesInputMB[8];
    uint8_t numInput[8];
    uint8_t estado[8];
    uint8_t numPinesInput;
    uint16_t minDs;
    struct fechaHora dateTimeLectAnterior;
public:
    N4DIG08(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~N4DIG08();
    uint8_t attachInputPlacaMadre(pinInput *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(uint16_t *resultado, int16_t *error);
};



class mb16di: public dispositivo
{
protected:
    uint16_t idNombre;
    uint8_t address;
    uint16_t numEstadoPlaca; // indica si esta ok (cuando esta a 1)
    pinInput *pinesMB[16];
    uint8_t numInput[16];
    uint8_t estado[16];
    uint8_t numPines;
    struct fechaHora dateTimeLectAnterior;
    uint16_t minDs;
public:
    mb16di(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~mb16di();
    uint8_t attachInputPlacaMadre(pinInput *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(uint16_t *resultado, int16_t *error);
};



class placaI2COut: public dispositivo
{
protected:
    uint16_t numEstado;
    uint8_t dirI2C;
    uint16_t numEstadoPlaca; // indica si esta ok (cuando esta a 1)
    pinOut *pinesI2C[16];
    uint8_t numRele[16];
    uint8_t estadoEnviado[16];
    uint8_t numPines;
    uint16_t checkIOIters;
public:
    placaI2COut(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~placaI2COut();
    uint8_t attachOutputPlacaMadre(pinOut *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  );
};


class placaI2CInput: public dispositivo
{
protected:
    uint16_t numEstado;
    uint8_t dirI2C;
    uint16_t numEstadoPlaca; // indica si esta ok (cuando esta a 1)
    pinInput *pinesI2C[16];
    uint8_t numInput[16];
    uint8_t numPines;
    uint16_t checkIOIters;
public:
    placaI2CInput(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~placaI2CInput();
    uint8_t attachInputPlacaMadre(pinInput *outPin, uint8_t numPar, char *pars[], uint8_t *hayError);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
    void leer(uint16_t *resultado, int16_t *error);
};

class t80 : public dispositivo
{
    // T80 MedidorFlexo 8
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


class testMB
{
    static uint16_t idPage;
    static uint16_t idNombre;
    static uint32_t baudios;
    static uint8_t direccion;
    static uint8_t funcion;
    static uint8_t datosTx[30];
    static uint8_t datosRx[30];
    static uint8_t longTx;
    static uint16_t longRx;
public:
    static uint8_t leeDatosNextion(uint8_t numPar, char *pars[]);
    static void leeMB(void);
};

typedef enum { RDESCONOCIDO=0, RDESCONECTADO, RCONECTADO, RPIDECARGA, RPIDEVENTILACION} estadoRes_t;
typedef enum { DESCONOCIDO=0, DESCONECTADO, RECIENCONECTADO, CARGANDO_MAX, CARGANDO_POCO,
               CARGADO, ESPERANDOCAPACIDAD, DIODOMAL } estadoCarga_t;

typedef enum {CPFIJO=0, CPMAXIMO, CPNOEXPORTA, CPDESCONOCIDO} tipoControlP_t;
typedef enum {TMFIJO=0, TMESCLAVO, TMMAESTRO, TMDESCONOCIDO} tipoMaestro_t;

#define MAXCARGADORES 3
#define MAXAJUSTES 2
#define NUMREGSHIST 8

class cargador : public bloque
{
// CARGADOR cargTesla sensRCar PTesla ITesla PCasa [ajustFijoAuto 0] [IFija 10] [Pcontr 9000] 3 16 MAESTRO setpointMaestro
// CARG-AJUSTES ajustes [ajustFijoEscMaestro.ajustes.picc.4 0] [Ifija 10] [mAmin 4300] Ptot Pmax [Ppunta 10000] [Pvalle 15000] PermP PermL PermV coches
protected:
    static cargador *cargadorPtr;
    medida *medPTotal;
    uint16_t medPTotalOld;
    uint16_t pContratada;
    uint16_t idPageAjustes;
    parametroU16Flash *pContratadaPunta;
    parametroU16Flash *pContratadaValle;
    uint8_t osciladorOculto;
    uint8_t haCambiadoR;
    uint8_t ondaNegOk;
    uint8_t antDsOndaNegOk;
    estadoRes_t nuevaR;
    uint16_t datADCHist[NUMREGSHIST];     // para grabar cambios
    estadoRes_t  estadosResHist[NUMREGSHIST];
    uint8_t numDatHist;
    uint16_t idPageCuando[MAXAJUSTES];
    parametroU16Flash *statusPermP[MAXAJUSTES];
    parametroU16Flash *statusPermL[MAXAJUSTES];
    parametroU16Flash *statusPermV[MAXAJUSTES];
    parametroU16Flash *horaIni[MAXAJUSTES];
    parametroU16Flash *horaFin[MAXAJUSTES];
    parametroU16Flash *soloSol[MAXAJUSTES];
    parametroU16Flash *tipoControlPCuando[MAXAJUSTES];
    int8_t ajusteAplicable;
    uint8_t permisoDeCarga; // segun "donde"
    parametroU16Flash *mAmin;

    static uint8_t numAjustes;
    campoNextion *nombre;
    uint16_t idPagecoches;
    medida *medkWhCoche;
    float  kWhIniCoche;
    float kWhIniHora;           // NUEVO
    float pAdicional;
    bool haykWhIniCoche;
    medida *medkWhParcCoche;
    estadoCarga_t statusCarga;
    estadoRes_t statusResisOld;
    medida *medICoche[3];
    uint16_t medICocheOld;
    medida *medPCoche;
    uint16_t medPCocheOld;

    // variables parecids a los cargadores remotos, pero locales
    parametroU16Flash *IMaxSelf[MAXAJUSTES];
    parametroU16Flash *idChargerSelf;
    tipoControlP_t tipoControlPSelf; // CPFIJO=0, CPMAXIMO, CPNOEXPORTA
    tipoMaestro_t tipoMaestroSelf;      // TMFIJO=0, TMESCLAVO, TMMAESTRO
    medida *medPSetPoint;
    float oldSP;
    float factorP; // relacion potencia deseada/real

    // datos de cargadores remotos
    uint8_t numCoches;
    uint8_t dsSinComunica[MAXCARGADORES];
    float   iMax[MAXCARGADORES];
    float   iVE[MAXCARGADORES];
    uint8_t numFasesMax[MAXCARGADORES];
    uint8_t idCharger[MAXCARGADORES];
    float   setPointP[MAXCARGADORES];
    uint16_t setPointPOld[MAXCARGADORES];
    float   minP[MAXCARGADORES];
    float   maxP[MAXCARGADORES];
    estadoRes_t statusResis[MAXCARGADORES];
    tipoControlP_t tipoControlP[MAXCARGADORES];
    tipoMaestro_t tipoMaestro[MAXCARGADORES];
    uint8_t numFases[MAXCARGADORES];
    float   PCoche[MAXCARGADORES];
public:
    cargador(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    cargador(void);
    ~cargador();
    void attachCargador(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    static void defAjustes(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    void calculaNumFases(void);
    uint8_t estimaFases(void);
    float minPotencia(void);
    float potencia2i(float pSetPoint);
    float i2potencia(float amp);
    void controlPropio(uint8_t seg, uint8_t ds);
    void controlConjunto(uint8_t ds);
    estadoRes_t diEstadoRes(void);
    static void setEstadoRes(uint16_t valorADC, estadoRes_t estR);
    static estadoRes_t getEstadoRes(void);
    static void ocultaOscilador(void);
    static void sacaOscilador(void);
    uint8_t esPuntaLLanoValle(void);
    const char *diTipo(void);
    const char *diNombre(void);
    static uint8_t diId(void);
    static uint8_t buscoCargador(uint8_t id);
    static float getPVEtotal(void);
    static void canCambiaID(uint8_t id, uint8_t nuevaID);
    static void canSetConfig(uint8_t id, uint8_t iMax, uint8_t numFasesMax);
    static void canSetSP(uint8_t idMaestro, uint8_t idDestino, uint16_t potSP);
    static void canSetEstadoEnNextion(uint8_t id, uint8_t estR, uint8_t estCont, uint8_t estMaestro, uint8_t numFasesReal, uint16_t minPot, uint16_t maxPot);
    static void canSetPVE(uint8_t id, uint16_t potVE);
    static void canSetIVE(uint8_t id, float intenVE);
    static void canSetPCasa(uint16_t potCasa);
    static void canSetPContratadas(uint16_t pPunta, uint16_t pValle);
    static void enviaEstadoCargador2can(void);
    static void enviaPVE(void);
    static void enviaIVE(void);
    static void enviaPContratadas(void);
    static void recibidoPorCan(uint8_t idCarg);
    static void limpiaCoche(uint8_t coche);
    static void limpiaCochesScreen(void);
    static uint8_t cochesConectados(void);
    static void estimaValoresADC(void);
    static void enviaHistADC(void);
    static void iniciaADC(void);
    void aplicaTipoControl(void);
    int8_t init(void);
    void initCargador(void);
    uint8_t calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void fijaAmperios(float amperios);
    void initADC(void);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void trataOrdenNextion(char *vars[], uint16_t numPars);
    void actualizaNextion(void);
};


class cocheSim : public bloque
{
protected:
    medida  *pSP, *pCoche, *kWhCar, *ICoche;
    float iMax;
    uint8_t numFases;
public:
    cocheSim(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~cocheSim();
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    uint8_t calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void actualizaNextion(void);
};

class casaSim : public bloque
{
protected:
    medida  *pCasa;
    float pInterna;
public:
    casaSim(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~casaSim();
    const char *diTipo(void);
    const char *diNombre(void);
    int8_t init(void);
    uint8_t calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void actualizaNextion(void);
};

class dimmer : public dispositivo
{
    // dimmer termo 2 2000 WTermo
protected:
    uint16_t idNombreDisp;
    uint16_t direccion;
    medida   *medidaSP;
    float    oldSP;
    float    wMax;
    struct fechaHora dateTimeLectAnterior;
public:
    dimmer(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~dimmer();
    void escribeHR(uint16_t valor, uint16_t addressReg, int16_t *error);
    uint8_t usaBus(void);
    uint16_t diIdNombre(void);
    const char *diTipo(void);
    int8_t init(void);
};

#endif /* DISPOSITIVOS_DISPOSITIVOS_H_ */
