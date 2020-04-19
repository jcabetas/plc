#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"


extern estados est;

/*
 * AND  incoherenciaTB  BoyaTope  noBoyaBaja
 */
add::add(uint8_t numPar, char *pars[])
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

const char *add::diTipo(void)
{
	return "and";
}

const char *add::diNombre(void)
{
	return nombres::nomConId(numOut);
}

int8_t add::init(void)
{
    estados::ponEstado(numOut, 0);
    return 0;
}

void add::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    uint8_t estat = 1;
    for (uint16_t i=0;i<4;i++)
        if (numInputs[i]>0)
            estat &= estados::diEstado(numInputs[i]);//est[numInputs[i]];
    estados::ponEstado(numOut, estat);
}

void add::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
}

void add::print(void)
{
    printf("[%s-%d] = AND ",estados::nombre(numOut),numOut);
    for (uint8_t i=0;i<4;i++)
        if (numInputs[i]>0)
            printf(" [%s-%d]",estados::nombre(numInputs[i]),numInputs[i]);
    printf("\n");
}
