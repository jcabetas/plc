#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"


extern estados est;
const char *descTipDuracion(uint8_t tipDura);
uint8_t tipDuracion(char *descTipDuracion);

/*
 * class delayon: public bloque {
    protected:
        int16_t numOut;
        int16_t numInput;
        uint16_t cuenta;
        uint16_t tiempo;
        uint8_t tipoCuenta;
        uint8_t horaIni, minIni, secIni, dsIni;
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
 * DELAYON  output input tiempo [unidades, dsegundo default]
 */
delayon::delayon(uint8_t numPar, char *pars[])
{
    if (numPar<4 || numPar>5)
    {
        printf("#parametros incorrecto\n");
        return; // error
    }
    numOut = estados::addEstado(pars[1],1);
    if (numOut==0)
        return;
    numInput = estados::addEstado(pars[2],0);
    tiempo = atoi(pars[3]);
    if (numPar==5)
        tipoCuenta = tipDuracion(pars[4]);
    else
        tipoCuenta = 4; 
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
    contando = 0;
    cuenta = 0;
    return 0;
}

// mientras la entrada este activa, va aumentando cuenta. Cuando llega al valor, activa la salida
// si baja en cualquier momento se reinicia
void delayon::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
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
        contando = 0;
        cuenta = 0;
        return;
    }
    // entrada activada, pero no la salida => cuento
    // estabamos contando??
    if (!contando)
    {
        contando = 1;  // no estaba contando. Empiezo
        cuenta = 0;
        minIni = min;
        secIni = seg;
        dsIni = ds;
        return;
    }
    // Vemos si tengo que avanzar contador
    // si arranco a las 3:55 15,3 s
    // si cuenta minutos tiene que avanzar cuando coincidan los seg y dseg
    if  (tipoCuenta==4 ||  // ds
        (tipoCuenta==3 && (dsIni==ds)) ||   // seg
        (tipoCuenta==2 && (dsIni==ds) && (secIni==seg)) ||  // min
        (tipoCuenta==1 && (dsIni==ds) && (secIni==seg) && minIni==min)) // hora
    {
        cuenta += dsInc;
        if (cuenta>=tiempo)
            estados::ponEstado(numOut, 1);
    }
}

void delayon::print(void)
{
    printf("[%s-%d] = DELAYON [%s-%d] T:%d %s\n",estados::nombre(numOut),numOut,
           estados::nombre(numInput), numInput, tiempo,descTipDuracion(tipoCuenta));
}
