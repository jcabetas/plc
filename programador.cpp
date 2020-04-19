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
        uint16_t dsQueFaltan;
        uint16_t numNombre;
        uint8_t numZonas;
        uint8_t numStarts;
        uint8_t cicloArrancado;
        uint16_t numSuspendeConteo;
        uint16_t numOutBomba;
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
 * PROGRAMADOR riego outBomba [numOutBomba
 */
programador::programador(uint8_t numPar, char *pars[])
{
    if (numPar!=3 && numPar!=4)
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
    numOutBomba = estados::addEstado(pars[2],1);
    if (numPar==4)
        numSuspendeConteo = estados::addEstado(pars[3],0);
    else
        numSuspendeConteo = 0;
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
    dsQueFaltan = 0;
    return 0;
}


void programador::arranca(uint8_t esB)
{
    if (numZonas==0)
        return;
    estado = 1;
    cicloArrancado = esB;
    dsQueFaltan = 600*zonas[0]->diTiempo(esB);
    zonas[0]->ponSalida(1);
    estados::ponEstado(numOutBomba, 1);
    return;
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

void programador::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds) // devuelve 1 si ha cambiado
{
}

void programador::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (estado==0) // toca actualizar temporizador?
        return;
    if (numSuspendeConteo>0 && estados::diEstado(numSuspendeConteo)==1) // no esta paralizado el conteo?
        return;
    if (dsInc <= dsQueFaltan)
        dsQueFaltan -= dsInc;
    else
        dsQueFaltan = 0;
    if (dsQueFaltan==0) // hemos alcanzado el contador, pasa a siguiente zona
    {
        zonas[estado-1]->ponSalida(0); // desactiva zona en marcha
        if (estado>=numZonas) // hemos cubierto todas las zonas
        {
            estado = 0;
            estados::ponEstado(numOutBomba, 0);
            return;
        }
        while (++estado<=numZonas) // busca zonas con tiempo>0
        {
            if (zonas[estado-1]->diTiempo(cicloArrancado)>0)
            {
                dsQueFaltan = 600*zonas[estado-1]->diTiempo(cicloArrancado);
                zonas[estado-1]->ponSalida(1);
                return;
            }
        }
        // hemos cubierto todas las zonas
        estado = 0;
        estados::ponEstado(numOutBomba, 0);
    }
}

void programador::print(void)
{
    if (numSuspendeConteo==0)
        printf("PROGRAMADOR %s bomba:%s-%d\n",nombres::nomConId(numNombre),estados::nombre(numOutBomba),numOutBomba);
    else
        printf("PROGRAMADOR %s bomba:%s-%d suspendeConteo:%s-%d\n",nombres::nomConId(numNombre),estados::nombre(numOutBomba),numOutBomba,estados::nombre(numSuspendeConteo),numSuspendeConteo);
}
