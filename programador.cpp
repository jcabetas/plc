#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

/*
 * PROGRAMADOR riego
 */
programador::programador(uint8_t numPar, char *pars[])
{
    if (numPar!=2)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numNombre = nombres::incorpora(pars[1]);
    if (numNombre==0)
        return;
};

const char *programador::diTipo(void)
{
	return "programador";
}

const char *programador::diNombre(void)
{
	return nombres::nomConId(numNombre);
}

int8_t programador::init(void)
{
    estado = 0;
    segundosQueFaltan = 0;
    return 0;
}

int8_t programador::calcula(void) // devuelve 1 si ha cambiado
{
    return 0;
}

int8_t programador::addTime(uint16_t ms)
{
    return 0;
}

void programador::print(void)
{
    printf("[%s-%d] = PROGRAMADOR\n",nombres::nomConId(numNombre),numNombre);
}
