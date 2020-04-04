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
    est[numOut] = 0;
    cuentaDs = 0;
    return 0;
}

int8_t timer::calcula(void) // devuelve 1 si ha cambiado
{
    uint8_t estadoOld = est[numOut];
    if (!est[numOut]) // no esta activo?
    {
        if (est[numInput])
        {
            est[numOut] = 1;
            return 1;
        }
    }
    return 0;
}

int8_t timer::addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (est[numOut])
    {
        if (est[numInput])
        	cuentaDs = 0;
        else
        {
			cuentaDs += ms/100;
			if (cuentaDs>=tiempoDs)
			{
				est[numOut] = 0;
				return 1;
			}
        }
    }
    return 0;
}

void timer::print(void)
{
    printf("[%s-%d] = TIMER [%s-%d] T:%d ds\n",estados::nombre(numOut),numOut,
           estados::nombre(numInput), numInput, tiempoDs);
}
