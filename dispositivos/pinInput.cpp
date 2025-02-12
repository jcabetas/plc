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
    uint16_t numEstado;
    dispositivo *disp;
    uint16_t idNombDisp;
 *
 */
//  pinInput placasEntrada boyaAlta 2
pinInput::pinInput(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=4)
    {
        nextion::enviaLog(tty,"#parametros PININPUT");
        *hayError = 1;
        return; // error
    }
    idNombDisp = nombres::incorpora(pars[1]);
    numEstado = estados::addEstado(tty, pars[2], 1, hayError);
    disp =  dispositivo::findDispositivo(idNombDisp);
    if (disp==NULL)
        nextion::enviaLog(tty, "No existe disp. en PININPUT");
    else
        disp->attachInputPlacaMadre(this, numPar-3, &pars[3], hayError);
};

pinInput::~pinInput()
{
}

void pinInput::setValor(uint8_t valor)
{
    estados::ponEstado(numEstado, valor);
}

const char *pinInput::diTipo(void)
{
    return "pinInput";
}

const char *pinInput::diNombre(void)
{
    return estados::nombre(numEstado);
}

int8_t pinInput::init(void)
{
    return 0;
}

uint8_t pinInput::calcula(uint8_t , uint8_t , uint8_t , uint8_t )
{
    return 0;
}


void pinInput::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t )
{
}


void pinInput::print(BaseSequentialStream *tty)
{
    char buffer[60];
    chsnprintf(buffer,sizeof(buffer),"[%s-%d] = PININPUT %s",estados::nombre(numEstado),numEstado,nombres::nomConId(idNombDisp));
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}


void pinInput::trataOrdenNextion(char **, uint16_t )
{

}

