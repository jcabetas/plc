#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"


extern estados est;
uint8_t tipDuracion(char *descTipDuracion);
const char *descTipDuracion(uint8_t tipDura);

/*
class timerNoRedisp: public bloque {
    protected:
        int16_t numOut;
        int16_t numInput;
        uint16_t cuenta;
        uint16_t tiempo;
        uint8_t tipoCuenta; // 1:horas, 2:minutos, 3:segundos, 4:dsegundos
        uint8_t horaIni, minIni, secIni, dsIni; // 

}

    TIMERNOREDISP  output input tiempo [tipoCuenta (ds default)]

    => Arranca cuando la entrada es 1
       Cuando ha arrancado esta a nivel alto hasta que transcurre el tiempo y baja la entrada
       Es Timer NO redisparable

       Input   ______-------------------______
       Output  ______----------------------___
                     <-------  T  -------->

 */



timerNoRedisp::timerNoRedisp(uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar<4 || numPar>5)
    {
        printf("#parametros incorrecto\n");
        *hayError = 1;
        return; // error
    }
    numOut = estados::addEstado(pars[1],1,hayError);
    if (numOut==0)
    {
        *hayError = 1;
        return;
    }
    numInput = estados::addEstado(pars[2],0, hayError);
    tiempo = atoi(pars[3]);
    if (numPar==5)
        tipoCuenta = tipDuracion(pars[4]);
    else
        tipoCuenta = 4;     
}

const char *timerNoRedisp::diTipo(void)
{
	return "timerNoRedisp";
}

const char *timerNoRedisp::diNombre(void)
{
	return nombres::nomConId(numOut);
}

int8_t timerNoRedisp::init(void)
{
    estados::ponEstado(numOut, 0);
    cuenta = 0;
    return 0;
}

void timerNoRedisp::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    // si no estaba arrancado. Hay que arrancar ahora??
    if (!estados::diEstado(numOut) && estados::diEstado(numInput))
    {
        estados::ponEstado(numOut, 1);
        minIni = min;
        secIni = seg;
        dsIni = ds;
        cuenta = 0;
    }
}

void timerNoRedisp::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (estados::diEstado(numOut))
    {
        // estabamos activos, miro si tengo que avanzar contador
        if  (tipoCuenta==4 ||  // ds
            (tipoCuenta==3 && (dsIni==ds)) ||   // seg
            (tipoCuenta==2 && (dsIni==ds) && (secIni==seg)) ||  // min
            (tipoCuenta==1 && (dsIni==ds) && (secIni==seg) && minIni==min)) // hora
        {
            if (cuenta<tiempo)
                cuenta += dsInc;
        }
        // si hemos llegado al maximo, pero la entrada esta baja, desactiva
        if (cuenta==tiempo && !estados::diEstado(numInput))
        {
            estados::ponEstado(numOut, 0);
        }
    }
}

void timerNoRedisp::print(void)
{
    printf("[%s-%d] = TIMERNOREDISP [%s-%d] T:%d %s\n",estados::nombre(numOut),numOut,
           estados::nombre(numInput), numInput, tiempo, descTipDuracion(tipoCuenta));
}
