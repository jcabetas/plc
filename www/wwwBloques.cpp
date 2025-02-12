/*
 * wwwBloques.cpp
 *
 *  Created on: 31 ago. 2021
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;

#include "bloques.h"
#include "nextion.h"
#include "chprintf.h"
#include "string.h"
#include "tty.h"


/*
 * ESTADOWWW estado [Nombre largo]
 *
 */
void estados::ponEstadoEnWWW(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    uint8_t numEstado;
    if (numPar!=3)
    {
        nextion::enviaLog(tty, "#parametros en ESTADOWWW");
        *hayError = 1;
        return; // error
    }
    numEstado = estados::addEstado(tty,pars[1], 0, hayError);

    if (pars[2][0]=='[')
        pars[2] = &pars[2][1];
    if (pars[2][strlen(pars[2])-1]==']')
        pars[2][strlen(pars[2])-1] = 0;
    idNombreWWW[numEstado-1] = nombres::incorpora(pars[2]);
}

