#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "parametros.h"

extern estados est;

/*
    TIMER  output input tiempo [tipoCuenta (ds default)]

    => Arranca cuando la entrada es 1
       Cuando ha arrancado esta a nivel alto hasta que transcurre el tiempo 
       después de la bajada de la entrada
       Es Timer redisparable

        Input   ______-------------------_________________________
       Output  ______------------------------------------------___
                                         <-------  T  -------->  

 */

// decodifica tipo de duracion: // 1:horas, 2:minutos, 3:segundos, 4:dsegundos
uint8_t tipDuracion(char *descTipDuracion)
{
    uint8_t longDesc = strlen(descTipDuracion);
    if (!strncmp("horas", descTipDuracion, longDesc)) // vale "h", "hor", "horas"
        return 1;
    if (!strncmp("minutos", descTipDuracion, longDesc))
        return 2;
    if (!strncmp("segundos", descTipDuracion, longDesc))
        return 3;
    if (!strncmp("dsegundos", descTipDuracion, longDesc))
        return 4;
    printf("Tipo de duracion (%s) incorrecto\n", descTipDuracion);
    return 4;
}

const char *descTipDuracion(uint8_t tipDura)
{
    switch (tipDura)
    {
    case 1:
        return "horas";
        break;
    case 2:
        return "min";
        break;
    case 3:
        return "seg";
        break;
    case 4:
        return "dseg";
        break;
    default:
        return "??";
    }
}

timer::timer(uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar < 4 || numPar > 5)
    {
        printf("#parametros incorrecto\n");
        *hayError = 1;
        return; // error
    }
    numOut = estados::addEstado(pars[1], 1, hayError);
    if (numOut == 0)
        return;
    numInput = estados::addEstado(pars[2], 0, hayError);
    tiempo = parametro::addParametroU16(pars[3],hayError);
    if (numPar == 5)
        tipoCuenta = tipDuracion(pars[4]);
    else
        tipoCuenta = 4;
}

const char *timer::diTipo(void)
{
    return "timer";
}

const char *timer::diNombre(void)
{
    return nombres::nomConId(numOut);
}

int8_t timer::init(void)
{
    estados::ponEstado(numOut, 0);
    cuenta = 0;
    return 0;
}

void timer::calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    // hay que arrancarlo
    if (estados::diEstado(numInput) && !estados::diEstado(numOut))
    {
        cuenta = 0;                     // si, reseteamos contador
        estados::ponEstado(numOut, 1);  // y activamos salida
    }
}

void timer::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (estados::diEstado(numOut))
    {
        // estaba arrancado, revisemos si sigue la entrada
        if (estados::diEstado(numInput))
        {
            cuenta = 0; // si, reseteamos contador
            estados::ponEstado(numOut, 1);
            minIni = min;
            secIni = seg;
            dsIni = ds;
        }
        else
        {
            // la entrada ha dejado de estar inactiva, vemos si tengo que avanzar contador
            // si arranco a las 3:55 15,3 s
            // si cuenta minutos tiene que avanzar cuando coincidan los seg y dseg
            if (tipoCuenta == 4 ||                                                      // ds
                (tipoCuenta == 3 && (dsIni == ds)) ||                                   // seg
                (tipoCuenta == 2 && (dsIni == ds) && (secIni == seg)) ||                // min
                (tipoCuenta == 1 && (dsIni == ds) && (secIni == seg) && minIni == min)) // hora
            {
                cuenta += dsInc;
                if (cuenta >= tiempo->valor())
                    estados::ponEstado(numOut, 0);
            }
        }
    }
}

void timer::print(void)
{
    printf("[%s-%d] = TIMER [%s-%d] T:%d %s\n", estados::nombre(numOut), numOut,
           estados::nombre(numInput), numInput, tiempo->valor(), descTipDuracion(tipoCuenta));
}
