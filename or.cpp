/*
 * OR.cpp
 *
 *  Created on: 4 abr. 2020
 *      Author: joaquin
 */
#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"

extern estados est;

/*
 * OR  incoherenciaTB  BoyaTope  noBoyaBaja
 */
OR::OR(uint8_t numPar, char *pars[])
{
    if (numPar<4 || numPar>5)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numOut = estados::addEstado(pars[1],1);
    if (numOut==0)
        return;
    for (uint8_t i=2;i<numPar;i++)
        numInputs[i-2] = estados::addEstado(pars[i],0);
};

const char *OR::diTipo(void)
{
    return "OR";
}

const char *OR::diNombre(void)
{
    return nombres::nomConId(numOut);
}

int8_t OR::init(void)
{
    estados::ponEstado(numOut, 0);
    return 0;
}

int8_t OR::calcula(void) // devuelve 1 si ha cambiado
{
    uint8_t estadoOld = est[numOut];
    uint8_t estat = 1;
    for (uint16_t i=0;i<4;i++)
        if (numInputs[i]>0)
            estat |= est[numInputs[i]];
    est[numOut] = estat;
    return (est[numOut]!=estadoOld);
}

int8_t OR::addTime(uint16_t ms)
{
    (void) ms;
    return 0;
}

void OR::print(void)
{
    printf("[%s-%d] = OR ",nombres::nomConId(numOut),numOut);
    for (uint8_t i=0;i<4;i++)
        if (numInputs[i]>0)
            printf(" [%s-%d]",nombres::nomConId(numInputs[i]),numInputs[i]);
    printf("\n");
}


