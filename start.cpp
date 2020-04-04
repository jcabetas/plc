#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


/*
class start: public bloque {
    protected:
        programador *program;
        uint16_t numInput;
        uint8_t AoB;
        uint8_t DOW;
        uint8_t hora;
        uint8_t min;
    public:
        start(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms);
};
 */

extern estados est;
extern programador *programadores[NUMPROGRAMADORES];


/*
 * Start nombProgramador nombStart AoB DOW hora min
 */
start::start(uint8_t numPar, char *pars[])
{
    if (numPar!=7)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numNombre = nombres::incorpora(pars[2]);
    if (numNombre==0)
        return;
    arrancaA = atoi(pars[3]);
    DOW = atoi(pars[4]);
    hora = atoi(pars[5]);
    min = atoi(pars[6]);
    for (uint8_t numProg=0;numProg<programador::numProgramadores;numProg++)
    {
        if (!strcmp(programadores[numProg]->diNombre(),pars[1]))
        {
            program = programadores[numProg];
            program->asignaStart(this);
        }
    }
};

const char *start::diTipo(void)
{
	return "start";
}

const char *start::diNombre(void)
{
	return nombres::nomConId(numNombre);
}

int8_t start::init(void)
{
    return 0;
}

int8_t start::calcula(void) // devuelve 1 si ha cambiado
{
    return 0;
}

int8_t start::addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    return 0;
}

void start::print(void)
{
    printf("START (%s, %s, ArrancaA:%d, %d:%d)\n",program->diNombre(), nombres::nomConId(numNombre), arrancaA, hora, min);
}