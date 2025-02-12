/*
 * pinOutput.cpp
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
    uint16_t numEstado;
    dispositivo *disp;
    uint8_t  valor, valorOld;
    uint8_t numPin;
 *
 */
//  pinOut relesSalidas calefOut [4]
pinOut::pinOut(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar<3)
    {
        nextion::enviaLog(tty,"#parametros pinOut");
        *hayError = 1;
        return; // error
    }
    idNombDisp = nombres::incorpora(pars[1]);
    numEstado = estados::addEstado(tty, pars[2], 0, hayError);
    disp =  dispositivo::findDispositivo(idNombDisp);
    if (disp==NULL)
        nextion::enviaLog(tty, "No existe disp. en PINOUT");
    else
        disp->attachOutputPlacaMadre(this, numPar-3, &pars[3], hayError);
};

pinOut::~pinOut()
{
}

uint8_t pinOut::getValor(void)
{
    return estados::diEstado(numEstado);
}

const char *pinOut::diTipo(void)
{
    return "pinOut";
}

const char *pinOut::diNombre(void)
{
    return "pinOut";//estados::nombre(numEstado);
}

int8_t pinOut::init(void)
{
    return 0;
}

void pinOut::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t )
{
}


void pinOut::print(BaseSequentialStream *tty)
{
    char buffer[60];
    chsnprintf(buffer,sizeof(buffer),"[%s-%d] = PINOUT %s",estados::nombre(numEstado),numEstado,nombres::nomConId(idNombDisp));
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}


void pinOut::trataOrdenNextion(char **, uint16_t )
{

}

