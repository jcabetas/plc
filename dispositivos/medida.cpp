/*
 * medida.cpp
 *
 *  Created on: 1 may. 2021
 *      Author: joaquin
 */



#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"
#include "string.h"
#include "stdlib.h"
#include "bloques.h"
#include "nextion.h"
#include "dispositivos.h"
#include "colas.h"

extern struct queu_t colaMsgTxWWW;

medida *medida::medidas[MAXMEDIDAS] = {0};
uint16_t medida::numMedidas = 0;

/*
 * MEDIDA WFlexo"Pot flexo" 3 1 ... (nombreMedida, nombreCompleto, periodicidad, numDecimales ... [nombreDispo, definicionesDispositivos])
 * MEDIDA altAgua.base.txt"Altura agua" 20 0 conv4_20 1 0.0 300.0
 *
 * Si esta definida pagina nextion y campo texto, se actualiza con cada medida, pero como maximo cada segundo
 * Si es invalida pondra "?" y en color rojo
 */
medida::medida(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar<4)
    {
        nextion::enviaLog(tty,"#parametros Medida");
        *hayError = 1;
        return; // error
    }
    uint8_t error = hallaNombreyDatosNextion(pars[1], TIPONEXTIONALL, &idNombre, &idNombreWWW, &idPageNxt, &tipoNxt, &picBaseNxt);
    if (error)
        *hayError = 1;
    dsObj = atoi(pars[2]);
    decimales = atoi(pars[3]);
    ds = 0xFFFF; // medida invalidada
    medidaValida = 0;
    inicializada = 0;
    enviadaANextionyWWW = 0;
    calendar::getFechaHora(&dateTimeEnvioNextionAnterior);
    valorStrEnviado[0] = 0;
    idNombTopic = 0;
    if (numMedidas>=MAXMEDIDAS)
    {
        nextion::enviaLog(tty,"demasiadas medidas");
        *hayError = 1;
    }
    else
    {
        medidas[numMedidas++] = this;
    }
    medidaDeMaximos = NULL;
    idCan = 0;
    chMtxObjectInit(&MtxMedida);

    if (numPar==4)
    {
        idNombDispositivo = 0;
        return;
    }
    // si hay mas parametros, vinculo el dispositivo
    idNombDispositivo = nombres::incorpora(pars[4]);
    dispositivo *disp = dispositivo::findDispositivo(idNombDispositivo);
    if (disp!=NULL)
    {
        disp->attachMedidas(this, numPar-5, &pars[5], hayError);
    }
    else
    {
        nextion::enviaLog(tty,"#no existe dispositivo");
        *hayError = 1;
    }
};

medida::medida(const char *nombMedida, uint16_t ds, uint8_t numDecimales, const char *nombDisp, uint8_t *hayError)
{
    uint8_t error = hallaNombreyDatosNextion(nombMedida, TIPONEXTIONALL, &idNombre, &idNombreWWW, &idPageNxt, &tipoNxt, &picBaseNxt);
    if (error)
        *hayError = 1;
    dsObj = ds;
    decimales = numDecimales;
    ds = 0xFFFF; // medida invalidada
    medidaValida = 0;
    inicializada = 0;
    enviadaANextionyWWW = 0;
    calendar::getFechaHora(&dateTimeEnvioNextionAnterior);
    valorStrEnviado[0] = 0;
    idNombTopic = 0;
    if (numMedidas>=MAXMEDIDAS)
    {
        //nextion::enviaLog(tty,"demasiadas medidas");
        *hayError = 1;
    }
    else
    {
        medidas[numMedidas++] = this;
    }
    medidaDeMaximos = NULL;
    idCan = 0;
    chMtxObjectInit(&MtxMedida);

    if (nombDisp[0]==0)
    {
        idNombDispositivo = 0;
        return;
    }
    // no hago attach, complicado de hacerlo generico
};

//medida::medida(BaseSequentialStream *tty, char *nombreMed, uint8_t numDecimales, uint16_t dsObjetivo, uint8_t *hayError)
//{
//    uint8_t error = hallaNombreyDatosNextion(nombreMed, TIPONEXTIONALL, &idNombre, &idNombreWWW, &idPageNxt, &tipoNxt, &picBaseNxt);
//    if (error)
//        *hayError = 1;
//    idNombDispositivo = 0;
//    dsObj = dsObjetivo;
//    decimales = numDecimales;
//    ds = 0xFFFF; // medida invalidada
//    medidaValida = 0;
//    inicializada = 0;
//    enviadaANextionyWWW = 0;
//    idCan = 0;
//    calendar::getFechaHora(&dateTimeEnvioNextionAnterior);
//    if (numMedidas>=MAXMEDIDAS)
//    {
//        nextion::enviaLog(tty,"demasiadas medidas");
//        *hayError = 1;
//    }
//    else
//    {
//        medidas[numMedidas++] = this;
//    }
//    medidaDeMaximos = NULL;
//    chMtxObjectInit(&MtxMedida);
//};


medida::~medida()
{

}

void medida::deleteAll(void)
{
    for (uint16_t med=1;med<=numMedidas;med++)
    {
        if (medidas[med-1] == NULL)
        {
            delete medidas[med-1];
            medidas[med-1] = NULL;
        }
    }
    numMedidas = 0;
}



void medida::set(float *valorPar, uint8_t fuerzaAct)
{
    chMtxLock(&MtxMedida);
    valor = *valorPar;
    chMtxUnlock(&MtxMedida);
    if (medidaDeMaximos!=NULL) // tiene medidas maximas asociadas
    {
        medidaDeMaximos->nuevaMedida(valorPar);
        medidaDeMaximos->actualizaNextionyWWW();
        medidaDeMaximos->setValidez(1);
    }
    if (idCan!=0)
        can::med2msg(idCan, valorPar);
    setValidez(1);
    actualizaNextionyWWW(fuerzaAct);
    ds = 0;
}

float medida::diValor(void)
{
    return valor;
}

uint8_t medida::diDecimales(void)
{
    return decimales;
}


const char *medida::diTipo(void)
{
    return "medida";
}

const char *medida::diNombre(void)
{
    return nombres::nomConId(idNombre);
}


medida *medida::findMedida(const char *nombre)
{
    for (uint16_t m=1;m<=numMedidas;m++)
    {
        if (!strcmp(nombre,medidas[m-1]->diNombre()))
            return medidas[m-1];
    }
    return NULL;
}

int8_t medida::init(void)
{
    if (!inicializada)
    {
        chMtxLock(&MtxMedida);
        valor = 0.0f;
        chMtxUnlock(&MtxMedida);
        inicializada = 1;
    }
    ds = 0xFFFF;
    return 0;
}

uint8_t medida::necesitaUpdate(void)
{
    if (ds>dsObj)
        return 1;
    else
        return 0;
}

uint8_t medida::esVieja(void)
{
    if (ds<4*dsObj)
        return 0;
    else
        return 1;
}

uint8_t medida::esValida(void)
{
    return medidaValida;
}

void medida::setValidez(uint8_t nuevaValidez)
{
    uint8_t validezOld = medidaValida;
    medidaValida = nuevaValidez;
    if (medidaDeMaximos!=NULL)
        medidaDeMaximos->setValidez(nuevaValidez);
    if (nuevaValidez==1)
        ds = 0;
    if (nuevaValidez!=validezOld)
    {
        actualizaNextionyWWW(0);
        if (nuevaValidez==0)
        {
            chMtxLock(&MtxMedida);
            valor = 0.0f;
            chMtxUnlock(&MtxMedida);
        }
    }
}

void medida::attachMedidaMax(medidaMax *medMax)
{
    medidaDeMaximos = medMax;
}

void medida::attachCan(uint16_t idCAN)
{
    idCan = idCAN;
}

// MED2MQTT kWhImp villagolf/kWhImp
void medida::attachMqtt(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros MED2MQTT");
        *hayError = 1;
        return; // error
    }
    medida *medMqtt = findMedida(pars[1]);
    if (medMqtt==NULL)
    {
        nextion::enviaLog(tty,"medida no encontrada en MED2MQTT");
        *hayError = 1;
        return; // error
    }
    medMqtt->idNombTopic = nombres::incorpora(pars[2]);
}


void medida::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  )
{
    if (ds<0xFFFF)
       ds++;
    if (esVieja())
        setValidez(0);
}

void medida::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"MEDIDA %s en %s",nombres::nomConId(idNombre),nombres::nomConId(idNombDispositivo));
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}



// Llamada en Init: vuelca a JSON
void medida::initWWW(BaseSequentialStream *SDPort, uint8_t *hayDatosVolcados)
{
    /*
    { tipo: 'MEDIDA', id: 'Ptot', nombre: 'Pot casa (W):'}
     */
    if (idNombreWWW!=0)
    {
        if (*hayDatosVolcados)
            chprintf(SDPort,",");
        chprintf(SDPort, "{\"tipo\":\"MEDIDA\",\"id\":\"%s\",\"nombre\":\"%s\",\"valor\":\"-\"",
                   nombres::nomConId(idNombre),nombres::nomConId(idNombreWWW));
        if (idNombTopic>0)
            chprintf(SDPort, ",\"topic\":\"%s\"",nombres::nomConId(idNombTopic));
        chprintf(SDPort, "}");
        *hayDatosVolcados = 1;
    }
}

void medida::enviaWWW(char *valorStr)
{
    struct msgTxWWW_t message;
    message.idNombVariable = idNombre;
    message.accion = TXWWWMEDIDAVAL;
    chsnprintf(message.valor, sizeof(message.valor), "\"%s\"",valorStr);
    putQueu(&colaMsgTxWWW, &message);
}

void medida::actualizaNextionyWWW(uint8_t forzar)
{
    char buffer[20];
    uint16_t dsDif;
    if (idPageNxt>0 || idNombreWWW>0)
    {
        dsDif = calendar::dsDiff(&dateTimeEnvioNextionAnterior);
        if (dsDif<10 && enviadaANextionyWWW && !forzar)
            return;
        if (!medidaValida)
            chsnprintf(buffer,sizeof(buffer),"?");
        else
        {
            chMtxLock(&MtxMedida);
            float val = valor;
            chMtxUnlock(&MtxMedida);
            switch (decimales)
            {
            case 0:
                chsnprintf(buffer,sizeof(buffer),"%d",(int) val);
                break;
            case 1:
                chsnprintf(buffer,sizeof(buffer),"%.1f",val);
                break;
            case 2:
                chsnprintf(buffer,sizeof(buffer),"%.2f",val);
                break;
            case 3:
                chsnprintf(buffer,sizeof(buffer),"%.3f",val);
                break;
            default:
                chsnprintf(buffer,sizeof(buffer),"%d",(int) val);
                break;
            }
        }
        // solo actualizo si ha pasado 30s o hay cambios respecto envio anterior
        if (!forzar && dsDif<300 && !strncmp(buffer, valorStrEnviado, sizeof(valorStrEnviado)))
            return;
        enviadaANextionyWWW = 1;
        if (idPageNxt>0)
            enviaTxt(idPageNxt,idNombre, buffer);
        if (idNombreWWW>0)
            enviaWWW(buffer);
        strncpy(valorStrEnviado, buffer, sizeof(valorStrEnviado));
        calendar::getFechaHora(&dateTimeEnvioNextionAnterior);
    }
}
