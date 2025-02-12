/*
 * casaSim.cpp
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "bloques.h"
#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"


int32_t randomNum(int32_t numMin, int32_t numMax);

/*
 * Simula potencia total de una casa
 * valores de entrada:
 * - Lee potencias de coches desde los datos de cargador
*  - ...
 * valores de salida:
*  - *pCasa
 */
// CASASIM pCasa
casaSim::casaSim(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=2)
    {
        nextion::enviaLog(tty,"#parametros incorrecto CASASIM");
        *hayError = 1;
        return;
    }
    pCasa = medida::findMedida(pars[1]);
    pInterna = 0.0f;
    return;
}


casaSim::~casaSim()
{
}

const char *casaSim::diNombre(void)
{;
    return "CASASIM";
}

const char *casaSim::diTipo(void)
{
    return "CASASIM";
}

int8_t casaSim::init(void)
{
    // pCasa, pCoche1, pCoche2
    pInterna = (float) randomNum(-9000, -7000);
    pCasa->set(&pInterna, 1);
    return 0;
}

uint8_t casaSim::calcula(uint8_t , uint8_t , uint8_t , uint8_t )
{
    return 0;
}

void casaSim::addTime(uint16_t , uint8_t , uint8_t , uint8_t seg, uint8_t ds)
{
    if (ds == 0)
    {
        if (seg>30)
            pInterna = -9000.0f+(seg-30)*100.0f;
        else
            pInterna = -3000.0f+seg*100.0f;
    }
    pInterna = -9000.0f;
    float pCasaActual = pInterna + cargador::getPVEtotal();
    pCasa->set(&pCasaActual, 1);
}

void casaSim::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"casaSim");
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}

