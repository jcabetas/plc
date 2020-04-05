#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"


extern estados est;

/*
 * TIMER  output input dsegundo
 */
timer::timer(uint8_t numPar, char *pars[])
{
    if (numPar!=4)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numOut = estados::addEstado(pars[1],1);
    if (numOut==0)
        return;
    numInput = estados::addEstado(pars[2],0);
    tiempoDs = atoi(pars[3]);
};

const char *timer::diTipo(void)
{
	return "timer";
}

const char *timer::diNombre(void)
{
	return nombres::nomConId(numOut);
}

int8_t timer::init(void)
{
    estados::ponEstado(numOut, 0);
    cuentaDs = 0;
    return 0;
}

void timer::calcula(void)
{
    if (!estados::diEstado(numOut)) // no esta activo?
    {
        if (estados::diEstado(numInput))
            estados::ponEstado(numOut, 1);
        else
            estados::ponEstado(numOut, 0);
    }
}

void timer::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (estados::diEstado(numOut))
    {
        if (estados::diEstado(numInput))
        	cuentaDs = 0;
        else
        {
			cuentaDs += dsInc;
			if (cuentaDs>=tiempoDs)
	            estados::ponEstado(numOut, 0);
        }
    }
}

void timer::print(void)
{
    printf("[%s-%d] = TIMER [%s-%d] T:%d ds\n",estados::nombre(numOut),numOut,
           estados::nombre(numInput), numInput, tiempoDs);
}
