/*
 * modbus.cpp
 *
 *  Created on: 17 abr. 2021
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"
#include "tty.h"

dispositivo *dispositivo::listDispositivos[MAXDISPOSITIVOS] = {0};
uint8_t dispositivo::numDispositivos = 0;




//// OOOOJJOOOOO
//extern "C" void __cxa_pure_virtual() {
//    while (1);
//}

dispositivo::dispositivo()
{
    /*
     *     static dispositivo *listDispositivos[MAXDISPOSITIVOS];
    static uint8_t numDispositivos;
     */
    if (numDispositivos>=MAXDISPOSITIVOS)
        return;
    listDispositivos[numDispositivos++] = this;
}

dispositivo::~dispositivo()
{
}

void dispositivo::deleteAll(void)
{
    for (int16_t d=0;d<dispositivo::numDispositivos;d++)
        delete listDispositivos[d];
    dispositivo::numDispositivos = 0;
}

uint8_t dispositivo::attachMedidas(medida *, uint8_t , char **, uint8_t *)
{
    return 1; // error por defecto
}


uint8_t dispositivo::attachOutputPlacaMadre(pinOut *, uint8_t , char **, uint8_t *)
{
    return 1;
}

uint8_t dispositivo::attachInputPlacaMadre(pinInput *, uint8_t , char **, uint8_t *)
{
    return 1;
}

dispositivo *dispositivo::findDispositivo(uint16_t idNombre)
{
    dispositivo *disp;
    for (uint8_t dis=1;dis<=numDispositivos;dis++)
    {
        disp = dispositivo::listDispositivos[dis-1];
        if (disp->diIdNombre()==idNombre)
            return (dispositivo *) disp;
    }
    return NULL;
}

