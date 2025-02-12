/*
 * pinInput.cpp
 */


#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "bloques.h"
#include "parametros.h"
#include "nextion.h"
#include "dispositivos.h"


/*
 *
class med2can: public bloque
{
//  cuando hay una medida se transmite all busCAN
//  esto solo sirve de buffer, para iniciarse cuando esten todos los bloque definidos
protected:
    static uint16_t  *nomMed2can[MAXMED2CAN];
    static uint16_t idMed2Can[MAXMED2CAN];
    static uint8_t  numMed2Can;
public:
    static void addMed2Can(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    static void initMed2Can(void);
};
 *
 */

uint16_t  med2can::nomMed2Can[MAXMED2CAN] = {0};
uint16_t med2can::idMed2Can[MAXMED2CAN] = {0};
uint8_t  med2can::numMed2Can = 0;

med2can::~med2can()
{
    numMed2Can = 0;
}

//  MED2CAN  placasEntrada boyaAlta 2
uint8_t med2can::addMed2Can(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    /*
     * Cuando llegue una nueva medida, llamar a can::transmit
     * MED2CAN NivelGasoil 22
     */
    if (numPar!=3)
    {
        nextion::enviaLog(tty,"#parametros MED2CAN");
        *hayError = 1;
        return 1; // error
    }
    if (numMed2Can>=MAXMED2CAN)
    {
        nextion::enviaLog(tty,"#demasiados MED2CAN");
        *hayError = 2;
        return 2; // error
    }
    nomMed2Can[numMed2Can] = nombres::incorpora(pars[1]);
    idMed2Can[numMed2Can] = atoi(pars[2]);
    numMed2Can++;
    return 0;
}


uint8_t med2can::initMed2Can(void)
{
    char buffer[50];
    for (uint8_t m2c=1; m2c<=numMed2Can;m2c++)
    {
        uint16_t idNom = nomMed2Can[m2c-1];
        medida *med = medida::findMedida(nombres::nomConId(idNom));
        if (med==NULL)
        {
            chsnprintf(buffer, sizeof(buffer), "No encuentro medida %s",nombres::nomConId(idNom));
            nextion::enviaLog(NULL,buffer);
            return 1;
        }
        med->attachCan(idMed2Can[m2c-1]);
    }
    return 0;
}

void med2can::deleteAll(void)
{
    numMed2Can = 0;
}
