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
extern programador *programadores[NUMPROGRAMADORES];


/*
 * Zona nombProgramador descripcionZona minutosA minutosB
 */
zona::zona(uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=5)
    {
        printf("#parametros incorrecto\n");
        *hayError = 1;
        return; // error
    }
    nombOut = parametro::addParametroString(pars[2],hayError);    
    printf("%s\n",nombOut->id());
    char id[20];
    strncpy(id, nombOut->id(),sizeof(id));
    numOut = estados::addEstado(id, 1, hayError);
    if (nombOut==NULL)
    {
        *hayError = 1;
        return;
    }
    minutosA = atoi(pars[3]);
    minutosB = atoi(pars[4]);
    for (uint8_t numProg=0;numProg<programador::numProgramadores;numProg++)
    {
        if (!strcmp(programadores[numProg]->diNombre(),pars[1]))
        {
            program = programadores[numProg];
            program->asignaZona(this);
        }
    }
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
    estados::ponEstado(numOut, 0);
    return 0;
}

void zona::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{

}

uint8_t zona::diTiempo(uint8_t esB)
{
    if (!esB)
        return minutosA;
    else
        return minutosB;
}

void zona::ponSalida(uint8_t estado)
{
    estados::ponEstado(numOut, estado);
}

void zona::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{

}

void zona::print(void)
{
    printf("[%s-%d] = ZONA (%s, %d min A, %d min B)\n",estados::nombre(numOut),numOut,program->diNombre(),minutosA, minutosB);
}
