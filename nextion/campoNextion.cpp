/*
 * led.cpp
 *
 *  Created on: 28 jun. 2020
 *      Author: jcabe
 */


#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "bloques.h"
#include "nextion.h"


campoNextion *campoNextion::camposNextion[NUMCAMPOSNEXTION] = {};
uint16_t campoNextion::numCamposNxt = 0;


campoNextion::campoNextion(uint16_t idNombre, uint16_t idPage, uint8_t tipoNextion, uint8_t picBaseNextion)
{
    camposNextion[numCamposNxt] = this;
    if (numCamposNxt < NUMCAMPOSNEXTION-1)
        numCamposNxt++;
    idNombreNxt = idNombre;
    idPageNxt = idPage;
    tipoNxt = tipoNextion;
    picBaseNxt = picBaseNextion;
}


campoNextion *campoNextion::addCampoNextion(char *nombre, uint16_t tipoNextion, uint8_t *hayError)
{
    uint8_t tipoNxtProv, picBaseNxtProv;
    uint16_t idNombreNxtProv, idPageNxtProv, idNombreWWWProv;
    char buffer[60];
    if (numCamposNxt>=NUMCAMPOSNEXTION-1)
    {
        nextion::enviaLog(NULL,"Demasiados campos nextion");
        *hayError = 1;
    }
    uint8_t error = hallaNombreyDatosNextion(nombre, tipoNextion, &idNombreNxtProv, &idNombreWWWProv, &idPageNxtProv, &tipoNxtProv, &picBaseNxtProv);
    if (error)
        *hayError = 1;
    if (error || (tipoNextion!=TIPONEXTIONALL && tipoNxtProv!=tipoNextion))
    {
        chsnprintf(buffer,sizeof(buffer),"Error campo nextion %s",nombre);
        nextion::enviaLog(NULL,buffer);
        *hayError = 1;
    }
    // compruebo que no exista ese campo
    for (uint16_t numCN=0;numCN<numCamposNxt;numCN++)
    {
        campoNextion *CN = camposNextion[numCN];
        if (CN->diNombreNxt()==idNombreNxtProv)
        {
            // existia ya, actualizo resto de campos y termino
            uint8_t errorCampos = 0;
            // actualizo idPage
            if (CN->idPageNxt!=0)
            {
                if (idPageNxtProv>0 && CN->idPageNxt!=idPageNxtProv)
                    errorCampos = 1;
            }
            else
                CN->idPageNxt = idPageNxtProv;
            // idem tipoNxt
            if (CN->tipoNxt!=0 && CN->tipoNxt!=TIPONEXTIONALL)
            {
                if (tipoNxtProv>0 && (tipoNxtProv!=TIPONEXTIONALL) && CN->tipoNxt!=tipoNxtProv)
                    errorCampos = 1;
            }
            else
                CN->tipoNxt = tipoNxtProv;
            // idem picBase
            if (CN->picBaseNxt!=0)
            {
                if (picBaseNxtProv>0 && CN->picBaseNxt!=picBaseNxtProv)
                    errorCampos = 1;
            }
            else
                CN->picBaseNxt= picBaseNxtProv;
            if (errorCampos==1)
            {
                chsnprintf(buffer,sizeof(buffer),"Campo nextion %s no coherentes",nombre);
                nextion::enviaLog(NULL,buffer);
                *hayError = 1;
            }
            return camposNextion[numCN];
        }
    }
    // no existe, lo creo
    return new campoNextion(idNombreNxtProv, idPageNxtProv, tipoNxtProv, picBaseNxtProv);
}


void campoNextion::deleteAll(void)
{
    for (uint16_t numCN=0;numCN<numCamposNxt;numCN++)
    {
        if (camposNextion[numCN]!=NULL)
        {
            delete camposNextion[numCN];
            camposNextion[numCN] = NULL;
        }
    }
    numCamposNxt = 0;
}

void campoNextion::enviaValPicCN(uint16_t valor)
{
    enviaValPic(idPageNxt, idNombreNxt, tipoNxt, picBaseNxt, valor);
}

void campoNextion::enviaTxtCN(const char *valor)
{
    enviaTxt(idPageNxt, idNombreNxt, valor);
}

uint16_t campoNextion::diPageNxt(void)
{
    return idPageNxt;
}

uint16_t campoNextion::diNombreNxt(void)
{
    return idNombreNxt;
}


uint8_t campoNextion::diTipoNxt(void)
{
    return tipoNxt;
}
