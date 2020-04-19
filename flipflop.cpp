/*
 * Flip-Flop.cpp
 *
 *  Created on: 5 abr. 2020
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
 * class flipflop: public bloque {
    protected:
        int16_t numOut;
        int16_t numInputSet;
        int16_t numInputReset;
    public:
        flipflop(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(void);
        void   print(void);
        void addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};
 */


/*
 * FLIPFLOP llenarPozo  nivelBajo(S)  nivelAlto(R)
 */
flipflop::flipflop(uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=4)
    {
        printf("#parametros incorrecto\n");
        *hayError = 1;
        return;
    }
    numOut = estados::addEstado(pars[1],1, hayError);
    if (numOut==0)
        return;
    numInputSet = estados::addEstado(pars[2],0, hayError);
    numInputReset = estados::addEstado(pars[3],0, hayError);
};

const char *flipflop::diTipo(void)
{
    return "FLIPFLOP";
}

const char *flipflop::diNombre(void)
{
    return nombres::nomConId(numOut);
}

int8_t flipflop::init(void)
{
    estados::ponEstado(numOut, 0);
    return 0;
}

void flipflop::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (estados::diEstado(numInputSet))
    {
        estados::ponEstado(numOut, 1);
        return;
    }
    if (estados::diEstado(numInputReset))
    {
        estados::ponEstado(numOut, 0);
        return;
    }
}

void flipflop::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
}

void flipflop::print(void)
{
    printf("[%s-%d] = FLIPFLOP (S:[%s-%d] R:[%s-%d]\n",estados::nombre(numOut),numOut,estados::nombre(numInputSet),numInputSet,estados::nombre(numInputReset),numInputReset );
}


