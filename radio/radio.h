/*
 * radio.h
 *
 *  Created on: 20 sept. 2020
 *      Author: jcabe
 */

#ifndef RADIO_RADIO_H_
#define RADIO_RADIO_H_


#include <stdint.h>
#include "parametros.h"
#include "bloques.h"
#include "calendar.h"

#define SEEDHISTORICO    5674

#define MODOREGISTRADOR 1
#define MODOLLAMADOR    2
#define MODOPOZO        3

#define NUMSATELITES                8
#define MSG_STATUSPOZO              1
#define MSG_STATUSLLAMACIONLOCAL    2
#define MSG_ERROR                   3
#define MSG_CLEARERROR              4
#define MAXERRORESAVISO             5

struct datosIdGuardados
{
    uint32_t numPeticiones;
    uint32_t segundosPeticion;
    float m3Total;
    float kWhTotal;
    float kWhPunta;
    float kWhValle;
};


struct datosPozoGuardados
{
    uint16_t seedInicial; // debe ser 9181 cuando esta iniciado
    time_t fechaInicio;
    float m3Total;
    float kWhTotal;
    float kWhPunta;
    float kWhValle;
    struct datosIdGuardados datosId[8];
};
//
//class radio : public bloque
//{
//protected:
//    uint16_t modoRadio;
//    uint16_t numInput;
//    uint16_t numOutput;
//    uint8_t estadoLlamaciones, estadoActivos, estadoAbusones;
//    uint8_t estadoLlamacionesOld, estadoActivosOld, estadoAbusonesOld;
//    int32_t msAleatorioMinEntreMsgsLllamador;
//    int32_t msAleatorioMaxEntreMsgsLllamador;
//    RTCDateTime dateTimeEnvioAnterior;
//    time_t timeInicioPeticion[NUMSATELITES];
//    time_t timeUltConexion[NUMSATELITES];
//    // tratamiento de errores de Jandro
//    uint8_t numErrorAviso[MAXERRORESAVISO];
//    uint8_t idEstacionAviso[MAXERRORESAVISO];
//    uint8_t mensajeAviso[MAXERRORESAVISO][21];
//    time_t timeInicioAvisoError[MAXERRORESAVISO];
//    parametroU16Flash *idLlamador;
//    parametroU16Flash *dsMaxEntreMsgsLlamador;
//    parametroU16Flash *dsMinEntreMsgsLlamador;
//    parametroU16Flash *dsMinEntreMsgsPozo;
//    parametroU16Flash *dsMaxEntreMsgsPozo;
//    parametroU16Flash *bloqueoAbusones;
//    parametroU16Flash *avisaAbuso;
//    parametroU16Flash *tiempoAbuso;         // minutos
//    parametroU16Flash *sOlvido;
//public:
//    static radio *radioPtr;
//    radio(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
//    ~radio();
//    // para implementar "bloque"
//    const char *diNombre(void);
//    int8_t init(void);
//    void calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
//    void print(BaseSequentialStream *tty);
//    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
//    void trataOrdenNextion(char **Vars, uint16_t numVars);
//    const char *diTipo(void);
//    // objeto "radio" propiemante dicho
//    // pozoComun:
//    void reseteaVariables(void);
//    uint8_t quitarAbuso(uint8_t numEstacion);
//    void onCambioParametrosPozo(void);
//    uint8_t buscoSlot(uint8_t numError, uint8_t numEstacion, uint8_t *slot);
//    void actualizoErrorDesdeLlamador(uint8_t numEstacion, uint8_t numError, uint8_t *msgError);
//    int8_t actualizoErrorDesdePozo(uint8_t numEstacion);
//    uint8_t limpiaError(uint8_t numEstacion, uint8_t numError);
//    void ponEnColaRegistrador(void);
//    void reseteaHistoriaPozo(void);
//    void apuntaEnergia(float incEner);
//    void registraCambiosPeticion(uint8_t estLlamaciones, uint8_t estLlamacionesOld);
//
//    // pozo:
//    void enviaStatusPozo(void);
//    void enviaErrorPozo(uint8_t slot);
//    void enviaClearErrorPozo(uint8_t numError, uint8_t numEstProblematica);
//    void reseteaEstadosOld(void);
//    uint8_t haCambiadoEstados(void);
//    uint8_t estadoPeticionBomba(void);
//    uint8_t gestionaPeticionPozo(uint8_t estacionMsg, uint8_t petBombaMsg);
//    void trataObsoletoPozo(void);
//    void trataRxPozo(struct msgRx_t *msgRx);
//    // llamador
//    void trataObsoletoLlamador(void);
//    void enviaStatusLlamacion(void);
//    uint8_t gestionaEstadoPozo(uint8_t petBombaMsg, uint8_t estadoLlamacionesMsg, uint8_t estadoActivosMsg);
//    void trataRxRegistradoryLlamador(struct msgRx_t *msgRx);
//
//    void trataRxRf95(eventmask_t evt);
//
//    void trataOrdenSMS(char **Vars, uint16_t numVars);
//};
//

class radio
{
protected:
    uint16_t modoRadio;
    uint16_t bombaPozoOn;                        // segun mensajes del pozo
    uint16_t bombaPozoSolicitada;                 // segun la peticion que le he hecho al pozo
    uint16_t numEstadoComOk, numEstadoBomba;
//    uint16_t idPageEstBomba, idNombreEstBomba;
//    uint8_t tipEstBomba, picEstBomba;
    uint16_t idPageLlamadores, idNombreLlamadores, idNombreLlamadoresWWW;
    uint16_t idPageLog, idNombreLog, idNombreLogWWW;
    uint16_t idPageActivos, idNombreActivos,idNombreActivosWWW;
    uint16_t idPageAbusones, idNombreAbusones, idNombreAbusonesWWW;
    static uint8_t estadoLlamaciones, estadoActivos, estadoAbusones;  // seg�n pozo
    static time_t timeInicioPeticion[NUMSATELITES];
    static time_t timeUltConexion[NUMSATELITES];
    uint8_t numErrorAviso[MAXERRORESAVISO];
    uint8_t idEstacionAviso[MAXERRORESAVISO];
    uint8_t mensajeAviso[MAXERRORESAVISO][21];
    time_t timeInicioAvisoError[MAXERRORESAVISO];
    uint8_t cnt;
    static float totEner;
    static radio *radioPtr;
public:
    static void apuntaEnergia(float incEner);
    static void trataRxRf95Radio(eventmask_t evt);
 //   virtual void reseteaVariables(void) = 0;
    virtual void trataRxRf95(eventmask_t evt) = 0;
    virtual void trataRx(struct msgRx_t *msgRx) = 0;
    static void reseteaHistoriaPozo(void);
    static void reseteaVariables(void);
    virtual void reseteaVariablesEspecificas(void) = 0;
    static void arrancaRadio(void);
    static void paraRadio(void);
    static uint8_t radioDefinida(void);
    // pozoComun:
    void registraCambiosPeticion(uint8_t estLlamaciones, uint8_t estLlamacionesOld);
    static void ponEnColaRegistrador(void);
    uint8_t buscoSlot(uint8_t numError, uint8_t numEstacion, uint8_t *slot);
    void actualizoErrorDesdeLlamador(uint8_t numEstacion, uint8_t numError, uint8_t *msgError);
    uint8_t limpiaError(uint8_t numEstacion, uint8_t numError);
    void actualizaNextion(uint8_t estLlamOld, uint8_t estActOld, uint8_t estAbuOld, uint8_t petBombOld);
//    virtual void ponStatus(sms *smsPtr);
//    void trataRxRegistradoryLlamador(struct msgRx_t *msgRx);
//    uint8_t quitarAbuso(uint8_t numEstacion);
//    void onCambioParametrosPozo(void);
//    uint8_t buscoSlot(uint8_t numError, uint8_t numEstacion, uint8_t *slot);
//    void actualizoErrorDesdeLlamador(uint8_t numEstacion, uint8_t numError, uint8_t *msgError);
//    uint8_t limpiaError(uint8_t numEstacion, uint8_t numError);
//    void ponEnColaRegistrador(void);
//    void apuntaEnergia(float incEner);
//    void registraCambiosPeticion(uint8_t estLlamaciones, uint8_t estLlamacionesOld);
//    void trataRxRegistradoryLlamador(struct msgRx_t *msgRx);
};


class registrador : public radio, bloque
{
protected:

    fechaHora dateTimeEnvioAnterior;
    parametroU16Flash *dsMaxEntreMsgsLlamador;
    parametroU16Flash *dsMinEntreMsgsLlamador;
    parametroU16Flash *sOlvido;
    struct fechaHora dateTimeRxPozoAnterior;
public:
    registrador(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~registrador();
    void reseteaVariablesEspecificas(void);
    // para implementar "bloque"
    const char *diNombre(void);
    int8_t init(void);
    void stop(void);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    const char *diTipo(void);
    uint8_t gestionaEstadoPozo(uint8_t petBombaMsg, uint8_t estadoLlamacionesMsg, uint8_t estadoActivosMsg);
    void trataRxRf95(eventmask_t evt);
    void trataRx(struct msgRx_t *msgRx);
//    void trataOrdenSMS(char **Vars, uint16_t numVars);
};


class llamador : public radio, bloque
{
protected:
    uint16_t numInput;
    uint16_t dsAleatorioMinEntreMsgsLlamador;
    uint16_t dsAleatorioMaxEntreMsgsLlamador;
    struct fechaHora dateTimeEnvioAnterior;
    struct fechaHora dateTimeRxPozoAnterior;
    //time_t timeUltConexion[NUMSATELITES];
    // tratamiento de errores de Jandro
//    uint8_t numErrorAviso[MAXERRORESAVISO];
//    uint8_t idEstacionAviso[MAXERRORESAVISO];
//    uint8_t mensajeAviso[MAXERRORESAVISO][21];
//    time_t timeInicioAvisoError[MAXERRORESAVISO];
//    uint16_t idPageComOk, idNombreComOk;
//    uint8_t tipComOk, picComOk;
    parametroU16Flash *idLlamador;
    parametroU16Flash *dsMaxEntreMsgsLlamador;
    parametroU16Flash *dsMinEntreMsgsLlamador;
    parametroU16Flash *sOlvido;
public:
    llamador(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~llamador();
    void reseteaVariablesEspecificas(void);
    // para implementar "bloque"
    const char *diNombre(void);
    int8_t init(void);
    void stop(void);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
//    void trataOrdenNextion(char **Vars, uint16_t numVars);
    const char *diTipo(void);
    // objeto "radio" propiamente dicho
    // pozoComun:
    uint8_t quitarAbuso(uint8_t numEstacion);
    void onCambioParametrosPozo(void);
    uint8_t limpiaError(uint8_t numEstacion, uint8_t numError);
    //void apuntaEnergia(float incEner);


    // llamador
    void trataObsoletoLlamador(void);
    void enviaStatusLlamacion(void);
    uint8_t gestionaEstadoPozo(uint8_t petBombaMsg, uint8_t estadoLlamacionesMsg, uint8_t estadoActivosMsg);


    void trataRxRf95(eventmask_t evt);
    void trataRx(struct msgRx_t *msgRx);
};



class pozo: public radio, bloque
{
protected:
    uint16_t numOutput;
    int32_t dsAleatorioMinEntreMsgsPozo;
    int32_t dsAleatorioMaxEntreMsgsPozo;
    struct fechaHora dateTimeEnvioAnterior;
    //time_t timeInicioPeticion[NUMSATELITES];
    //time_t timeUltConexion[NUMSATELITES];
    // tratamiento de errores de Jandro
//    uint8_t numErrorAviso[MAXERRORESAVISO];
//    uint8_t idEstacionAviso[MAXERRORESAVISO];
//    uint8_t mensajeAviso[MAXERRORESAVISO][21];
//    time_t timeInicioAvisoError[MAXERRORESAVISO];
    parametroU16Flash *dsMinEntreMsgsPozo;
    parametroU16Flash *dsMaxEntreMsgsPozo;
    parametroU16Flash *bloqueoAbusones;
    parametroU16Flash *avisaAbuso;
    parametroU16Flash *tiempoAbuso;         // minutos
    parametroU16Flash *sOlvido;
public:
    pozo(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~pozo();
    void reseteaVariablesEspecificas(void);
    // para implementar "bloque"
    const char *diNombre(void);
    int8_t init(void);
    void stop(void);
    uint8_t calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
//    void trataOrdenNextion(char **Vars, uint16_t numVars);
    const char *diTipo(void);
    // objeto "radio" propiemante dicho
    void reseteaVariables(void);
    uint8_t quitarAbuso(uint8_t numEstacion);
    void onCambioParametrosPozo(void);
    uint8_t buscoSlot(uint8_t numError, uint8_t numEstacion, uint8_t *slot);
    void actualizoErrorDesdeLlamador(uint8_t numEstacion, uint8_t numError, uint8_t *msgError);
    int8_t actualizoErrorDesdePozo(uint8_t numEstacion);


    //void reseteaHistoriaPozo(void);
    //void apuntaEnergia(float incEner);
    void registraCambiosPeticion(uint8_t estLlamaciones, uint8_t estLlamacionesOld);

    // pozo:
    void enviaStatusPozo(void);
    void enviaErrorPozo(uint8_t slot);
    void enviaClearErrorPozo(uint8_t numError, uint8_t numEstProblematica);
    void reseteaEstadosOld(void);
    uint8_t haCambiadoEstados(void);
    uint8_t estadoPeticionBomba(void);
    uint8_t gestionaPeticionPozo(uint8_t estacionMsg, uint8_t petBombaMsg);
    void trataObsoletoPozo(void);
    void trataRxPozo(struct msgRx_t *msgRx);
    // llamador
    void trataObsoletoLlamador(void);
    void enviaStatusLlamacion(void);
    uint8_t gestionaEstadoPozo(uint8_t petBombaMsg, uint8_t estadoLlamacionesMsg, uint8_t estadoActivosMsg);
    void trataRxRegistradoryLlamador(struct msgRx_t *msgRx);

    void trataRxRf95(eventmask_t evt);
    void trataRx(struct msgRx_t *msgRx);
};



#endif /* RADIO_RADIO_H_ */
