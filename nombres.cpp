/*
 * nombres.cpp
 *
 *  Created on: 4 abr. 2020
 *      Author: joaquin
 */


#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "bloques.h"

/*
 * class nombres {
    public:
      static uint16_t incorpora(const char *nombre_p);
      static uint16_t busca(const char *nombre_p);
      static const char *nomConId(uint16_t id);
};

    #define STORESIZE   1000
    #define MAXSTATES     50
    #define MAXNOMBRES    50

 */

static char nombStore[STORESIZE];
static uint16_t nombStart[MAXNOMBRES];
static uint16_t numNombres = 0;

uint16_t nombres::incorpora(const char *nombre_p)
{
    uint16_t posIniGrabacion;
    // compruebo si existe
    uint16_t posNomb = nombres::busca(nombre_p);
    if (posNomb>0)
        return posNomb;
    // hay que crearlo... cabe?
    if (numNombres>0)
    {
        uint16_t sitioUsado = nombStart[nombStart[numNombres]] + strlen(&nombStore[nombStart[numNombres]]) + 1;
        if (sitioUsado+strlen(nombre_p)+1>=STORESIZE)
            return 0;
        posIniGrabacion = sitioUsado;
    }
    else
    {
        if (strlen(nombre_p)+1>=STORESIZE)
            return 0;
        posIniGrabacion = 0;
    }
    // a grabar!
    nombStart[++numNombres] = posIniGrabacion;
    strcpy(&nombStore[nombStart[numNombres]],nombre_p);
    return numNombres;
};


uint16_t nombres::busca(const char *nombre_p)
{
    for (uint16_t posNomb=0;posNomb<numNombres;posNomb++)
        if (!strcmp(&nombStore[nombStart[posNomb]],nombre_p))
            return posNomb;
    return 0;
};

const char *nombres::nomConId(uint16_t id)
{
    if (id>numNombres)
        return "Error: id demasiado elevado";
    return &nombStore[nombStart[id]];
};
