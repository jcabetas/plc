#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"


/*
class programador: public bloque {
    protected:
        zona *zonas[16];
        start *starts[3];
        uint8_t estado; // 0 o zonaActiva
        uint16_t segundosQueFaltan;
        uint16_t numNombre;
        uint8_t numZonas;
        uint8_t numStarts;
        static uint8_t numProgramadores;
    public:
        programador(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        int8_t calcula(void);
        void   print(void);
        int8_t addTime(uint16_t ms);
};
 */


programador *programadores[NUMPROGRAMADORES];

uint8_t programador::numProgramadores=0;

/*
 * PROGRAMADOR riego
 */
programador::programador(uint8_t numPar, char *pars[])
{
    if (numPar!=2)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numNombre = nombres::incorpora(pars[1]);
    if (numNombre==0)
        return;
    if (numProgramadores>=NUMPROGRAMADORES)
    {
        printf("Error: Demasiados programadores\n");
        return;
    }
    programadores[numProgramadores++] = this;
    numZonas = 0;
    numStarts = 0;
};

const char *programador::diTipo(void)
{
	return "programador";
}

const char *programador::diNombre(void)
{
	return nombres::nomConId(numNombre);
}

int8_t programador::init(void)
{
    estado = 0;
    segundosQueFaltan = 0;
    return 0;
}

void programador::asignaZona(zona *zon)
{
    if (numZonas>=MAXZONAS)
        return;
    zonas[numZonas] = zon;
    numZonas++;
}

void programador::asignaStart(start *strt)
{
    if (numStarts>=MAXSTARTS)
        return;
    starts[numStarts] = strt;
    numStarts++;
}

int8_t programador::calcula(void) // devuelve 1 si ha cambiado
{
    return 0;
}

int8_t programador::addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    return 0;
}

void programador::print(void)
{
    printf("PROGRAMADOR %s\n",nombres::nomConId(numNombre));
}
