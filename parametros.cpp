#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "parametros.h"
#include "bloques.h"


uint16_t parametroU16::numParamsU16 = 0;
parametroU16 *parametroU16::parametrosU16[MAXPARAMETROS];

/*
class parametroU16
{
public:
    static parametroU16 *parametrosU16[MAXPARAMETROS];
    static uint16_t numParamsU16;
    parametroU16();
    virtual uint16_t valor(void) = 0;
};

class parametroU16Valor : parametroU16
{
private:
    uint16_t valU16;
public:
    parametroU16Valor(uint16_t valor);
    uint16_t valor(void);
};

class parametroU16Flash : parametroU16
{
private:
    uint16_t valU16;
public:
    parametroU16Flash(char *nombre, uint16_t valorIni, uint16_t valorMin, uint16_t valorMax);
    uint16_t valor(void);
};
*/

parametroU16Valor::parametroU16Valor(uint16_t valor)
{
    parametrosU16[numParamsU16] = this;
    numParamsU16++;
    valU16 = valor;
}

uint16_t parametroU16Valor::valor(void)
{
    return valU16;
}



parametroU16Flash::parametroU16Flash(char *nombre, uint16_t valorIni, uint16_t valorMin, uint16_t valorMax)
{
    parametrosU16[numParamsU16] = this;
    numParamsU16++;
    valU16 = valorIni;
}

uint16_t parametroU16Flash::valor(void)
{
    return valU16;
}

parametroU16 *parametroU16::addParametro(char *parStr, uint8_t *error)
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