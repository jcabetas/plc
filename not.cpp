#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"

/*
 * class NOT: public bloque {
    protected:
        uint16_t numOut;
        uint16_t numInput;
    public:
        NOT(uint8_t numPar, char *pars[]);  // lee desde string
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        void calcula(void);
        void print(void);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
};
 */

extern estados est;

/*
 * NOT  output input
 */
NOT::NOT(uint8_t numPar, char *pars[])
{
    if (numPar<3)
    {
        printf("#parametros incorrecto\n");
        return;
    }
    numOut = estados::addEstado(pars[1],1);
    if (numOut==0)
        return;
    numInput = estados::addEstado(pars[2],0);
};

const char *NOT::diTipo(void)
{
	return "NOT";
}

const char *NOT::diNombre(void)
{
	return nombres::nomConId(numOut);
}

int8_t NOT::init(void)
{
    estados::ponEstado(numOut, 0);
    return 0;
}

void NOT::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    estados::ponEstado(numOut, !estados::diEstado(numInput));
}

void NOT::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
}

void NOT::print(void)
{
    printf("[%s-%d] = NOT [%s-%d]\n",estados::nombre(numOut),numOut,estados::nombre(numInput),numInput);
}
