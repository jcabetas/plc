#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"


extern estados est;

/*
 * class delayon: public bloque {
    protected:
        int16_t numOut;
        int16_t numInput;
        uint16_t cuentaDs;
        uint16_t tiempoDs;
    public:
        delayon(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(void);
        void print(void);
        void addTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};
 */

/*
 * DELAYON  output input dsegundo
 */
delayon::delayon(uint8_t numPar, char *pars[])
{
    if (numPar!=4)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numOut = estados::addEstado(pars[1],1);
    if (numOut==0)
        return;
    numInput = estados::addEstado(pars[2],0);
    tiempoDs = atoi(pars[3]);
};

const char *delayon::diTipo(void)
{
	return "delayon";
}

const char *delayon::diNombre(void)
{
	return nombres::nomConId(numOut);
}

int8_t delayon::init(void)
{
    estados::ponEstado(numOut, 0);
    cuentaDs = 0;
    return 0;
}

// mientras la entrada este activa, va aumentando cuenta. Cuando llega al valor, activa la salida
// si baja en cualquier momento se reinicia
void delayon::calcula(void)
{
    // esta On si esta activo y la entrada esta activa
    if (estados::diEstado(numOut) && estados::diEstado(numInput))
        estados::ponEstado(numOut, 1);
    else
        estados::ponEstado(numOut, 0);
}

void delayon::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    // esta activo con entrada activa?  => ok, dejalo asi
    if (estados::diEstado(numOut) && estados::diEstado(numInput))
        return;
    // si la entrada esta desactivada, resetea
    if (!estados::diEstado(numInput))
    {
        estados::ponEstado(numOut, 0);
        cuentaDs = 0;
        return;
    }
    else
    {
        if (cuentaDs>=tiempoDs)
        {
            estados::ponEstado(numOut, 1);
        }
        cuentaDs += dsInc;
    }
}

void delayon::print(void)
{
    printf("[%s-%d] = DELAYON [%s-%d] T:%d ds\n",estados::nombre(numOut),numOut,
           estados::nombre(numInput), numInput, tiempoDs);
}
