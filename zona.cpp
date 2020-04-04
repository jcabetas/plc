#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


/*
 * class zona: public bloque {
    protected:
        programador *program;
        int16_t numOut;
        uint8_t minutosA;
        uint8_t minutosB;
    public:
        zona(uint8_t numPar, char *pars[]);  // lee desde string
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms);
};
 */

extern estados est;

/*
 * Zona nombProgramador nombOut minutosA minutosB
 */
zona::zona(uint8_t numPar, char *pars[])
{
    if (numPar!=5)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numOut = estados::addEstado(pars[2],1);
    minutosA = atoi(pars[3]);
    minutosB = atoi(pars[4]);
    if (numOut==0)
        return;
};

const char *zona::diTipo(void)
{
	return "zona";
}

const char *zona::diNombre(void)
{
	return nombres::nomConId(numOut);
}

int8_t zona::init(void)
{
    return 0;
}

int8_t zona::calcula(void) // devuelve 1 si ha cambiado
{
    return 0;
}

int8_t zona::addTime(uint16_t ms)
{
    return 0;
}

void zona::print(void)
{
    printf("[%s-%d] = ZONA\n",nombres::nomConId(numOut),numOut);
}
