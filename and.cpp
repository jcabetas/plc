#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"

//int16_t registraNombre(const char *nombre, uint8_t esOut);
//void divideString(char *buff,uint8_t *numPar, char *par[]);
//const char *nombState(uint16_t st);

//extern int8_t state[MAXSTATES];

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

int8_t add::calcula(void) // devuelve 1 si ha cambiado
{
    uint8_t estadoOld = est[numOut];
    uint8_t estat = 1;
    for (uint16_t i=0;i<4;i++)
        if (numInputs[i]>0)
            estat &= est[numInputs[i]];
    est[numOut] = estat;
    return (est[numOut]!=estadoOld);
}

int8_t add::addTime(uint16_t ms)
{
    (void) ms;
    return 0;
}

void add::print(void)
{
    printf("[%s-%d] = AND ",nombres::nomConId(numOut),numOut);
    for (uint8_t i=0;i<4;i++)
        if (numInputs[i]>0)
            printf(" [%s-%d]",nombres::nomConId(numInputs[i]),numInputs[i]);
    printf("\n");
}
