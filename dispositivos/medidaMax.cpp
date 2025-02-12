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

/*
 *     uint16_t idNombre;
    uint16_t idPageNxt;
    uint8_t numRegs;
    uint16_t dsEntreRegs;
    medida *medidaOrigen;
    medida *medidaMaxima;
    struct fechaHora dateTimeRegAnterior;
    float valorMaxRegActual;
    float valoresReg[];
 */

//    MEDIDAMAX WFlexo WFlexoMax 100 (nombre, nombreMedida, dsEntreRegs)
medidaMax::medidaMax(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    char buffer[50];
    if (numPar!=4)
    {
        nextion::enviaLog(tty,"#parametros MedidaMax");
        *hayError = 1;
        return; // error
    }
    // encuentro medida base
    medidaOrigen = medida::findMedida(pars[1]);
    ptrRegActual = 0;
    if (medidaOrigen==0)
    {
        nextion::enviaLog(tty,"#medida desconocida en MEDIDAMAX");
        *hayError = 1;
        return; // error
    }
    // creo nueva medida (la de maximos)
    dsEntreRegs = atoi(pars[3]);
    //    medida(BaseSequentialStream *tty, char *nombreMed, uint8_t numDecimales, uint16_t dsObjetivo, uint8_t *hayError);
//    medidaMaxima = new medida(tty, pars[2], medidaOrigen->diDecimales(),dsEntreRegs,hayError);
    medidaMaxima = medida::findMedida(pars[2]);//new medida(tty, pars[2], atoi(pars[4]), atoi(pars[3]), hayError);
    if (medidaMaxima==NULL)
    {
        chsnprintf(buffer,sizeof(buffer),"No encuentro medida %s",pars[2]);
        nextion::enviaLog(tty,buffer);
        *hayError = 2;
        return; // error
    }
};

medidaMax::~medidaMax()
{

}

const char *medidaMax::diTipo(void)
{
    return "medidaMax";
}

const char *medidaMax::diNombre(void)
{
    return medidaMaxima->diNombre();
}

void medidaMax::nuevaMedida(float *med)
{
    if (*med>valorMaxRegActual+0.0001f)
    {
        valorMaxRegActual = *med;
        // halla el maximo de los registros
        float maxVal = valorMaxRegActual;
        for (uint8_t m=0;m<REGSINMEDMAX;m++)
            if (valoresReg[m]>maxVal)
                maxVal = valoresReg[m];
        if (medidaMaxima!=NULL)
            medidaMaxima->set(&maxVal, 0);
    }
    medidaMaxima->setValidez(1);
}

void medidaMax::setValidez(uint8_t esBuena)
{
    medidaMaxima->setValidez(esBuena);
}

int8_t medidaMax::init(void)
{
    calendar::getFechaHora(&dateTimeRegAnterior);
    for (uint8_t m=0;m<REGSINMEDMAX;m++)
        valoresReg[m] = 0.0f;
    valorMaxRegActual = 0.0;
    medidaOrigen->attachMedidaMax(this);
    return 0;
}



//uint8_t medidaMax::calcula(uint8_t , uint8_t , uint8_t , uint8_t )
//{
//
//}

void medidaMax::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t  )
{
    uint16_t dsDif = calendar::dsDiff(&dateTimeRegAnterior);
    if (dsDif>dsEntreRegs)
    {
        // corre los registros, pon el maximo en puntero actual
        valoresReg[ptrRegActual] = valorMaxRegActual;
        if (++ptrRegActual>=REGSINMEDMAX)
            ptrRegActual = 0;
        // pon a cero el aximo del actual
        valorMaxRegActual = 0.0f;
        // halla el maximo de los registros
        float maxVal = 0.0f;
        for (uint8_t m=0;m<REGSINMEDMAX;m++)
            if (valoresReg[m]>maxVal)
                maxVal = valoresReg[m];
        if (medidaMaxima!=NULL)
            medidaMaxima->set(&maxVal, 0);
        calendar::getFechaHora(&dateTimeRegAnterior);
    }
}

void medidaMax::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"MEDIDAMAX Max(%s) en %s ds:%d",medidaOrigen->diNombre(),medidaMaxima->diNombre(),dsEntreRegs);
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}

void medidaMax::actualizaNextionyWWW(void)
{
    medidaMaxima->actualizaNextionyWWW(0);
//    char buffer[20];
//
//    if (idPageNxt>0)
//    {
//        uint16_t dsDif = calendar::dsDiff(&dateTimeEnvioNextionAnterior);
//        if (dsDif<10)
//            return;
//        if (!medidaValida)
//            chsnprintf(buffer,sizeof(buffer),"?");
//        else
//        {
//            switch (decimales)
//            {
//            case 0:
//                chsnprintf(buffer,sizeof(buffer),"%d",(int) valor);
//                break;
//            case 1:
//                chsnprintf(buffer,sizeof(buffer),"%.1f",valor);
//                break;
//            case 2:
//                chsnprintf(buffer,sizeof(buffer),"%.2f",valor);
//                break;
//            case 3:
//                chsnprintf(buffer,sizeof(buffer),"%.3f",valor);
//                break;
//            default:
//                chsnprintf(buffer,sizeof(buffer),"%d",(int) valor);
//                break;
//            }
//        }
//        enviaTxt(idPageNxt,idNombre, buffer);
//        calendar::getFechaHora(&dateTimeEnvioNextionAnterior);
//    }
}
