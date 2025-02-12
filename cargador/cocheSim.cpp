/*
 * cocheSim.cpp
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
 * Para probar cargador
 * valores de entrada:
 * - pSP1
 * - num. fases
 * - Imax
 * - Sensor de carga
 * valores de salida:
 * - pCoche
 * - kWh
 * - Icoche
 */
// COCHESIM pSP1, numFases, iMax, pCoche, kWhCar, Icoche
cocheSim::cocheSim(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=7)
    {
        nextion::enviaLog(tty,"#parametros incorrecto COCHESIM");
        *hayError = 1;
        return;
    }
    pSP = medida::findMedida(pars[1]);
    numFases = atoi(pars[2]);
    iMax = atof(pars[3]);
    pCoche = medida::findMedida(pars[4]);
    kWhCar = medida::findMedida(pars[5]);
    ICoche = medida::findMedida(pars[6]);
    return;
}


cocheSim::~cocheSim()
{
}

const char *cocheSim::diNombre(void)
{;
    return "cocheSim";
}

const char *cocheSim::diTipo(void)
{
    return "cocheSim";
}

int8_t cocheSim::init(void)
{
    // pSP1, numFases, iMax, pCoche, kWhCar, Icoche
    float pCero = 0.0f;
    float kWhIni1 = (float) randomNum(25, 700);
    pCoche->set(&pCero, 1);
    kWhCar->set(&kWhIni1, 1);
    ICoche->set(&pCero, 1);
    return 0;
}

uint8_t cocheSim::calcula(uint8_t , uint8_t , uint8_t , uint8_t )
{
    return 0;
}

void cocheSim::addTime(uint16_t dsInc, uint8_t , uint8_t , uint8_t , uint8_t )
{
    float iNueva;
    // asumo que los coches siguen las consignas, con un pelo de doferencia
    // para coche1 cojo el valor de I y pongo la P
    /*
     * valores de entrada:
     * - pSP1
     * - num. fases
     * - Imax
     * valores de salida:
     * - pCoche1
     * - kWh1
     * - Icoche1
     */
    float psp = pSP->diValor();
    float Isp = psp/(225.0f*numFases);
    // la intensidad solo puede crecer 2A por segundo
    float iActual = ICoche->diValor();
    estadoRes_t statusRes = cargador::getEstadoRes();// (estadoRes_t) estados::diEstado(idEstadoCarg);
    if (statusRes==RDESCONECTADO || statusRes==RDESCONOCIDO || statusRes==RCONECTADO)
    {
        iNueva = 0.0f;
    }
    else if (Isp>iActual)
    {
        iNueva = iActual + 0.05f;
        if (iNueva>Isp) iNueva = Isp;
    }
    else
    {
        iNueva = iActual - 0.2f;
        if (iNueva<Isp) iNueva = Isp;
    }
    if (iNueva > iMax) iNueva = iMax;
    if (Isp < 7.0f) iNueva = 0.0f;
    ICoche->set(&iNueva, 1);
    float pCar = iNueva*225.0f*numFases;
    pCoche->set(&pCar, 0);
    float kWh = kWhCar->diValor();
    kWh += pCar*dsInc/36000000.0f;
    kWhCar->set(&kWh, 0);
}

void cocheSim::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"COCHESIM");
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}
