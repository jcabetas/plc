#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "string.h"
#include "colas.h"
#include "nextion.h"
#include "bloques.h"
#include "radio.h"
#include "calendar.h"

void arrancaPozo(void);
uint8_t esValle(void);
void int2str(uint8_t valor, char *string);

extern mutex_t MtxMedidas;
extern struct queu_t colaCambiosPozo;
extern event_source_t registraMsgPozo_source;
struct msgRx_t ultMsg;

/*
 * RADIO LLAMADOR pideAgua idLlamador dsMinEntreMsgsLlamador dsMaxEntreMsgsLlamador
 * RADIO MONITOR
 * RADIO POZO outInversor dsMinEntreMsgsPozo dsMaxEntreMsgsPozo
 *
 */

radio *radio::radioPtr = NULL;;
uint8_t radio::estadoLlamaciones = 0;
uint8_t radio::estadoActivos = 0;
uint8_t radio::estadoAbusones = 0;
time_t radio::timeUltConexion[] = {0};
time_t radio::timeInicioPeticion[] = {0};
float radio::totEner = 0.0f;


void radio::reseteaVariables(void)
{
    time_t ahora;
    ahora = calendar::getSecUnix();
    estadoLlamaciones = 0;
    estadoActivos = 0;
    estadoAbusones = 0;
//    struct datosPozoGuardados *datos = (struct datosPozoGuardados *) BKPSRAM_BASE;
    for (uint8_t disp=0;disp<8;disp++)
    {
        timeInicioPeticion[disp] =  ahora;
        timeUltConexion[disp] =  ahora;
    }
    // comprueba coherencia de datos historicos
//    if (datos->seedInicial != SEEDHISTORICO) // cuando esta iniciado
//    {
//        nextion::enviaLog(NULL,"Reseteo historico!");
//        reseteaHistoriaPozo();
//    }
    if (radioPtr!=NULL)
        radioPtr->reseteaVariablesEspecificas();
}


void radio::trataRxRf95Radio(eventmask_t evt)
{
    if (radio::radioPtr!=NULL)
        radioPtr->trataRxRf95(evt);
}

//void radio::apuntaEnergia(float incEner)
//{
//    totEner += incEner;
//}

uint8_t radio::radioDefinida(void)
{
    return (radioPtr!=NULL);
}

/*
 * Busco error en tabla de errores
 * - Devuelve 1 si lo ha encontrado, en slot
 * - Devuelve 2 si no lo ha encontrado, pero hay hueco
 * - Devuelve 3 si no lo ha encontrado ni hay hueco
 */
uint8_t radio::buscoSlot(uint8_t numError, uint8_t numEstacion, uint8_t *slot)
{
    // Miro si estaba en un slot existente
    for (*slot=0;*slot<MAXERRORESAVISO;(*slot)++)
    {
        if (numErrorAviso[*slot]==numError && idEstacionAviso[*slot]==numEstacion)
            return 1;
    }
    // busco hueco
    for (*slot=0;*slot<MAXERRORESAVISO;(*slot)++)
    {
        if (numErrorAviso[*slot]==0 && idEstacionAviso[*slot]==0)
            return 2;
    }
    return 3;
}

/*
 * Actualizo mensaje de error
 */
void radio::actualizoErrorDesdeLlamador(uint8_t numEstacion, uint8_t numError, uint8_t *msgError)
{
    uint8_t encontrado, slot;
    if (numEstacion<1 || numEstacion>8)
        return;
    /*
    * - Devuelve 1 si lo ha encontrado, en slot
    * - Devuelve 2 si no lo ha encontrado, pero hay hueco
    * - Devuelve 3 si no lo ha encontrado ni hay hueco
    * */
    encontrado = buscoSlot(numError, numEstacion, &slot);
    if (encontrado==2) // hueco
    {
        numErrorAviso[slot] = 1;
        idEstacionAviso[slot] = numEstacion;
    }
    if (encontrado==1 || encontrado==2) // actualizo time y mensaje
    {
        strncpy((char *)mensajeAviso[slot],(char *)msgError,sizeof(mensajeAviso[slot]));
        timeInicioAvisoError[slot] = calendar::getSecUnix();
    }
}

/*
 * Limpia mensaje de error
 */
uint8_t radio::limpiaError(uint8_t numEstacion, uint8_t numError)
{
    // localiza slot de error
    for (uint8_t numSlot=0;numSlot<MAXERRORESAVISO;numSlot++)
    {
        if (idEstacionAviso[numSlot]==numEstacion && numErrorAviso[numSlot]==numError)
        {
            idEstacionAviso[numSlot] = 0;
            numErrorAviso[numSlot] = 0;
            return 1;
        }
    }
    return 0;
}

void radio::apuntaEnergia(float )
{
//    uint8_t esValleVal;
//    struct datosPozoGuardados *datos = (struct datosPozoGuardados *) BKPSRAM_BASE;
//    esValleVal = esValle();
//    // Energia total
//    chMtxLock(&MtxMedidas);
//    datos->kWhTotal += incEner;
//    if (esValleVal)
//        datos->kWhValle += incEner;
//    else
//        datos->kWhPunta += incEner;
//    chMtxUnlock(&MtxMedidas);
//    // ahora a las estaciones
//    if (estadoLlamaciones==0)
//        return;
//    for (uint8_t est=0;est<8;est++)
//    {
//        uint8_t estLlama = (estadoLlamaciones>>est) & 1;
//        if (estLlama==1)
//        {
//            chMtxLock(&MtxMedidas);
//            if (esValleVal)
//                datos->datosId[est].kWhValle += incEner;
//            else
//                datos->datosId[est].kWhPunta += incEner;
//            chMtxUnlock(&MtxMedidas);
//        }
//    }
}

void radio::reseteaHistoriaPozo(void)
{
//    struct datosPozoGuardados *datos = (struct datosPozoGuardados *) BKPSRAM_BASE;
//    datos->seedInicial = SEEDHISTORICO; // cuando esta iniciado
//    datos->fechaInicio = calendar::getSecUnix();
//    datos->m3Total = 0.0f;
//    datos->kWhTotal = 0.0f;
//    datos->kWhPunta = 0.0f;
//    datos->kWhValle = 0.0f;
//    for (uint8_t i=0;i<8;i++)
//    {
//        datos->datosId[i].numPeticiones = 0;
//        datos->datosId[i].segundosPeticion = 0;
//        datos->datosId[i].m3Total = 0.0f;
//        datos->datosId[i].kWhTotal = 0.0f;
//        datos->datosId[i].kWhPunta = 0.0f;
//        datos->datosId[i].kWhValle = 0.0f;
//    }
}



void radio::ponEnColaRegistrador(void)
{
    struct cambiosPozo_t estadoPozo;
//    if (montarSD->valor()==0)
//    if (montarSDValor()==0)
//        return;
    // pon en cola de registrador
    estadoPozo.timet = calendar::getSecUnix();
    estadoPozo.estActi = estadoActivos;
    estadoPozo.estLlam = estadoLlamaciones;
    estadoPozo.estAbuso = estadoAbusones;
//    struct datosPozoGuardados *datos = (struct datosPozoGuardados *) BKPSRAM_BASE;
//    chMtxLock(&MtxMedidas);
//    estadoPozo.m3Total = datos->m3Total;
//    estadoPozo.kWhPunta = datos->kWhPunta;
//    estadoPozo.kWhValle = datos->kWhValle;
//    chMtxUnlock(&MtxMedidas);
    putQueu(&colaCambiosPozo, &estadoPozo);
    chEvtBroadcast(&registraMsgPozo_source);
}


void radio::actualizaNextion(uint8_t estLlamOld, uint8_t estActOld, uint8_t estAbuOld, uint8_t petBombOld)
{
    char buff[10];
    //uint16_t bOn;
    if (estadoLlamaciones!=estLlamOld)
    {
        int2str(estadoLlamaciones, buff);
        enviaTxt(idPageLlamadores, idNombreLlamadores, buff);
    }
    if (estadoActivos!=estActOld)
    {
        int2str(estadoActivos, buff);
        enviaTxt(idPageActivos, idNombreActivos, buff);
    }
    if (estadoAbusones!=estAbuOld)
    {
        int2str(estadoAbusones, buff);
        enviaTxt(idPageAbusones, idNombreAbusones, buff);
    }
    if (bombaPozoOn!=petBombOld)
    {
        //bOn = bombaPozoOn;
        estados::ponEstado(numEstadoBomba, bombaPozoOn);
        estados::actualizaNextion(numEstadoBomba); // antes bombaPozoOn);
        //enviaValPic(idPageEstBomba, idNombreEstBomba, tipEstBomba, picEstBomba, bOn);
    }
}

/*
 *  varNUMERO idLlamador(18,8,1,8,"Id Llamador");
    varNUMERO dsMinEntreMsgsLlamador(19,5,2,60,"Tmn Msg Ll ds");
    varNUMERO dsMaxEntreMsgsLlamador(20,100,50,600,"Tmx Msg Ll ds");
 */
//uint16_t dsMaxEntreMsgsLlamador = 100;
//uint16_t dsMinEntreMsgsLlamador = 2;

//radio::radio(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
//{
//    if (numPar<2)
//    {
//        enviaLog(tty,"#parametros incorrectos RADIO",100);
//        *hayError = 1;
//        return;
//    }
//    if (strcasecmp(pars[1],"REGISTRADOR"))
//        modoRadio = 1;
//    else if (strcasecmp(pars[1],"LLAMADOR"))
//    {
//        if (numPar!=5)
//        {
//            enviaLog(tty,"#parametros incorrectos RADIO LLAMADOR",100);
//            *hayError = 1;
//            return;
//        }
//        // RADIO LLAMADOR pideAgua idLlamador dsMinEntreMsgsLlamador dsMaxEntreMsgsLlamador
//        modoRadio = 2;
//        numInput = estados::addEstado(tty, pars[2], 0, hayError);
//        idLlamador = parametro::addParametroU16FlashMinMax(tty, pars[3],1,8,hayError);
//        dsMinEntreMsgsLlamador = parametro::addParametroU16FlashMinMax(tty, pars[4],2,60,hayError);
//        dsMaxEntreMsgsLlamador = parametro::addParametroU16FlashMinMax(tty, pars[5],50,3600,hayError);
//    }
//    else if (strcasecmp(pars[1],"POZO"))
//    {
//        /*
//         *     varNOSI bloqueoAbusones(13,0,"Bloqueo abusos", onCambioParametrosPozo);
//    varNUMERO tiempoAbuso(14,360,1,600,"T abuso m");
//    varNUMERO sOlvido(15,60,10,600,"T olvido s");
//         *
//         */
//        if (numPar!=8)
//        {
//            enviaLog(tty,"#parametros incorrectos RADIO POZO",100);
//            *hayError = 1;
//            return;
//        }
//        // RADIO POZO outInversor dsMinEntreMsgsPozo dsMaxEntreMsgsPozo bloqueoAbusones tiempoAbuso sOlvido
//        modoRadio = 3;
//        numOutput = estados::addEstado(tty, pars[2], 1, hayError);
//        dsMinEntreMsgsPozo = parametro::addParametroU16FlashMinMax(tty, pars[4],2,60,hayError);
//        dsMaxEntreMsgsPozo = parametro::addParametroU16FlashMinMax(tty, pars[5],50,3600,hayError);
//        tiempoAbuso = parametro::addParametroU16FlashMinMax(tty, pars[5],5,600,hayError); // minutos
//        sOlvido = parametro::addParametroU16FlashMinMax(tty, pars[5],10,600,hayError);    // segundos
//    }
//    else
//    {
//        modoRadio = 0;
//        enviaLog(tty,"ERROR en MODO RADIO",1000);
//    }
//};

//radio::~radio()
//{
//}
//
//const char *radio::diTipo(void)
//{
//	return "RADIO";
//}
//
//const char *radio::diNombre(void)
//{
//	return "RADIO";
//}
//
//int8_t radio::init(void)
//{
//    arrancaPozo();
//    return 0;
//}
//
//void radio::calcula(uint8_t , uint8_t , uint8_t , uint8_t )
//{
//
//}
//
//void radio::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t )
//{
//}
//
//void radio::print(BaseSequentialStream *tty)
//{
//    char buffer[60];
//    if (modoRadio==0)
//        chsnprintf(buffer,sizeof(buffer),"RADIO REGISTRADOR");
//    else if (modoRadio==1)
//        chsnprintf(buffer,sizeof(buffer),"RADIO LLAMADOR input:%s-%d",estados::nombre(numInput),numInput);
//    else if (modoRadio ==2)
//        chsnprintf(buffer,sizeof(buffer),"RADIO POZO output:%s-%d",estados::nombre(numOutput),numOutput);
//    if (tty!=NULL)
//        enviaLog(tty, buffer,0);
//}
