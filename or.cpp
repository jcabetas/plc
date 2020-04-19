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
OR::OR(uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar<4 || numPar>5)
    {
        printf("#parametros incorrecto\n");
        *hayError = 1;
        return; // error
    }
    numOut = estados::addEstado(pars[1], 1, hayError);
    if (numOut==0)
        return;
    for (uint8_t i=2;i<numPar;i++)
        numInputs[i-2] = estados::addEstado(pars[i], 0, hayError);
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

void OR::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    uint8_t estat = 0;
    for (uint16_t i=0;i<4;i++)
        if (numInputs[i]>0)
            estat |= estados::diEstado(numInputs[i]);
    estados::ponEstado(numOut, estat);
}

void OR::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
}

void OR::print(void)
{
    printf("[%s-%d] = OR ",estados::nombre(numOut),numOut);
    for (uint8_t i=0;i<4;i++)
        if (numInputs[i]>0)
            printf(" [%s-%d]",estados::nombre(numInputs[i]),numInputs[i]);
    printf("\n");
}


