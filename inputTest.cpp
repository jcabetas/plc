/*
 * inputTest.cpp
 *
 *  Created on: 4 abr. 2020
 *      Author: joaquin
 */

#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include <stdlib.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"

/*
 * class inputTest: public bloque {
    protected:
        uint16_t numOut;
        uint16_t cuentaDs;
        uint16_t tiempoIni;
        uint16_t tiempoFin;
    public:
        inputTest(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms);
};
 */

/*
 *      inputTest niv1 5 10
 */


inputTest::inputTest(uint8_t numPar, char *pars[])
{
    if (numPar != 4)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numOut = estados::addEstado(pars[1],1);
    if (numOut==0)
        return;
    tiempoIni = atoi(pars[2]);
    tiempoFin = atoi(pars[3]);
};

const char *inputTest::diTipo(void)
{
    return "inputTest";
}

const char *inputTest::diNombre(void)
{
    return nombres::nomConId(numOut);
}

int8_t inputTest::init(void)
{
    estados::ponEstado(numOut, 0);
    cuentaDs = 0;
    return 0;
}

int8_t inputTest::calcula(void)
{
    if (cuentaDs>=tiempoIni && cuentaDs<=tiempoFin)
        estados::ponEstado(numOut, 1);
    else
        estados::ponEstado(numOut, 0);
    return 0;
}

int8_t inputTest::addTime(uint16_t ms)
{
    cuentaDs += ms/100;
    return 0;
}

void inputTest::print(void)
{
    printf("[%s-%d] = inputTest Start:%d Fin:%d\n",nombres::nomConId(numOut),numOut,tiempoIni,tiempoFin);
}
