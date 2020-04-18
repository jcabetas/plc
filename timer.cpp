#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"


extern estados est;

/*
class timer: public bloque {
    protected:
        int16_t numOut;
        int16_t numInput;
        uint16_t cuenta;
        uint16_t tiempo;
        uint8_t tipoCuenta; // 1:horas, 2:minutos, 3:segundos, 4:dsegundos
        uint8_t horaIni, minIni, secIni, dsIni; // 

}

    TIMER  output input tiempo [tipoCuenta (ds default)]

    => Arranca cuando la entrada es 1
       Cuando ha arrancado esta a nivel alto hasta que trasncurre el tiempo 
       después de la bajada de la entrada
       Es Timer redisparable

 */


// decodifica tipo de duracion: // 1:horas, 2:minutos, 3:segundos, 4:dsegundos
uint8_t tipDuracion(char *descTipDuracion)
{
    uint8_t longDesc = strlen(descTipDuracion);
    if (!strncmp("horas", descTipDuracion,longDesc)) // vale "h", "hor", "horas"
        return 1;
    if (!strncmp("minutos", descTipDuracion,longDesc))
        return 2;
    if (!strncmp("segundos", descTipDuracion,longDesc))
        return 3;
    if (!strncmp("dsegundos", descTipDuracion,longDesc))
        return 4;
    printf("Tipo de duracion (%s) incorrecto\n",descTipDuracion);
    return 4;
}

const char *descTipDuracion(uint8_t tipDura)
{
    switch(tipDura)
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


timer::timer(uint8_t numPar, char *pars[])
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

void timer::calcula(void)
{
}

void timer::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (estados::diEstado(numOut))
    {
        // estaba arrancado, revisemos si sigue la entrada
        if (estados::diEstado(numInput))
        	cuenta = 0; // si, reseteamos contador
        else
        {
            // la entrada ha dejado de estar incativa, vemos si tengo que avanzar contador
            // si arranco a las 3:55 15,3 s
            // si cuenta minutos tiene que avanzar cuando coincidan los seg y dseg
            if  (tipoCuenta==4 ||  // ds
                (tipoCuenta==3 && (dsIni==ds)) ||   // seg
                (tipoCuenta==2 && (dsIni==ds) && (secIni==seg)) ||  // min
                (tipoCuenta==1 && (dsIni==ds) && (secIni==seg) && minIni==min)) // hora
            {
                cuenta += dsInc;
                if (cuenta>=tiempo)
                    estados::ponEstado(numOut, 0);
            }
        }
    }
    else
    {
        // no estaba arrancado. Hay que arrancar ahora??
        if (estados::diEstado(numInput))
        {
            estados::ponEstado(numOut, 1);
            minIni = min;
            secIni = seg;
            dsIni = ds;
            cuenta = 0;
        }
    }
    
}

void timer::print(void)
{
    printf("[%s-%d] = TIMER [%s-%d] T:%d %s\n",estados::nombre(numOut),numOut,
           estados::nombre(numInput), numInput, tiempo, descTipDuracion(tipoCuenta));
}
