#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "parametros.h"
#include "bloques.h"

uint16_t parametro::numParams = 0;
parametro *parametro::params[MAXPARAMETROS];

uint16_t parametroFlash::numParamsFlash = 0;
parametroFlash *parametroFlash::paramsFlash[MAXPARAMETROS];

parametro::parametro()
{
    params[numParams] = this;
    numParams++;
}

parametro::~parametro()
{
    printf("Borrando parametro\n");
}

void parametro::deleteAll(void)
{
    for (int16_t nparam=0;nparam<parametro::numParams;nparam++)
        delete params[nparam];
    parametro::numParams = 0;
}

void parametro::printAll(void)
{
    for (int16_t nparam=0;nparam<parametro::numParams;nparam++)
        params[nparam]->print();
}



parametroFlash::parametroFlash()
{
    paramsFlash[numParamsFlash] = this;
    numParamsFlash++;
}

parametroU16Valor::parametroU16Valor(uint16_t valor)
{
    valU16 = valor;
}

parametroU16Valor::~parametroU16Valor()
{
    printf("Borrando parametro U16Valor: %d\n",valU16);
}


uint16_t parametroU16Valor::valor(void)
{
    return valU16;
}

void parametroU16Valor::print()
{
    printf("Valor de U16Valor:%d\n",valU16);
}



parametroU16Flash::parametroU16Flash(char *nombre, uint16_t valorIni, uint16_t valorMin, uint16_t valorMax)
{
    // parametrosU16[numParamsU16] = this;
    // numParamsU16++;
    valU16 = valorIni;
    valIni = valorIni;
    valMin = valorMin;
    valMax = valorMax;
    idNombre = nombres::incorpora(nombre);
}

parametroU16Flash::~parametroU16Flash()
{
    printf("Borrando parametro U16Flash %s: %d\n",nombres::nomConId(idNombre),valU16);
}

void parametroU16Flash::leeDeFlash()
{
    // to do: leer de flash
}

uint16_t parametroU16Flash::valor(void)
{
    return valU16;
}

void parametroU16Flash::print(void)
{
    printf("Valor de U16Flash %s: %d\n",nombres::nomConId(idNombre),valU16);
}

parametroU16 *parametro::addParametroU16(char *parStr, uint8_t *error)
{
    uint8_t numPar;
    char *par[10];

    // si no empieza por '[' es un numero
    if (parStr[0]!='[')
        return new parametroU16Valor(atoi(parStr)); // podria verificar que son numeros
    // es un bloque del tipo
    // [riego,0,0,60]
    divideBloque(parStr, &numPar, par);
    if (numPar != 4)
    {
        *error = 1;
        return new parametroU16Valor(0); // valor cuando hay error
    }
    return new parametroU16Flash(par[0], atoi(par[1]),atoi(par[2]),atoi(par[3]));
}