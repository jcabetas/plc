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
        uint8_t horaIni,minIni,segIni,dsIni;
        uint8_t horaFin,minFin,segFin,dsFin;
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
 *      inputTest niv1 8 0 0 0  8 0 1 0
 */


inputTest::inputTest(uint8_t numPar, char *pars[])
{
    if (numPar != 10)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numOut = estados::addEstado(pars[1],1);
    if (numOut==0)
        return;
    horaIni = atoi(pars[2]);
    minIni = atoi(pars[3]);
    segIni = atoi(pars[4]);
    dsIni  = atoi(pars[5]);
    horaFin = atoi(pars[6]);
    minFin = atoi(pars[7]);
    segFin = atoi(pars[8]);
    dsFin  = atoi(pars[9]);
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
    return 0;
}

int8_t inputTest::addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (hora>=horaIni && hora<=horaFin && min>=minIni && min<=minFin && seg>=segIni && seg<=segFin && ds>=dsIni && ds<=dsFin)
        estados::ponEstado(numOut, 1);
    else
        estados::ponEstado(numOut, 0);
    return 0;
}

void inputTest::print(void)
{
    printf("[%s-%d] = inputTest (%d:%d %d.%d-%d:%d %d.%d)\n",estados::nombre(numOut),numOut,horaIni, minIni, segIni, dsIni, horaFin, minFin, segFin, dsFin);
}
