/*
 * nextionCom.cpp
 *
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;

#include "chprintf.h"
#include "tty.h"
#include "gets.h"
#include "string.h"
#include "manejoVars.h"
#include "../mensajesPozoOld/pozo.hkk"

void enviaLabel(const char *pageName, const char *varName, const char *valor);
void enviaVal(const char *pageName, const char *varName, uint16_t valor);
void enviaTxt(const char *pageName, const char *varName, uint16_t valor);
void enviaMsg(const char *pageName, const char *varName, char *valor);
void enviaEstado(const char *pageName, const char *varName, uint16_t valor);

extern uint8_t petBomba;
extern float frecuencia, presion;
extern float kW;
extern uint8_t estadoLlamaciones, estadoActivos, estadoAbusones;

extern varNOSI bloqueoAbusones;
extern varNUMERO tiempoAbuso;
extern varNUMERO sOlvido;
extern varNUMERO dsMaxEntreMsgsPozo;
extern varNUMERO parametroFrecuenciaMB;
extern varNUMERO parametroPresionMB;
extern varNUMERO parametroEstadoMB;

void actualizaPozoConfig(void)
{
    struct datosIdGuardados *datoId;
    char buff[50];
    uint32_t numPet,minutos,kWhP,kWhV;
    struct datosPozoGuardados *datos = (struct datosPozoGuardados *) BKPSRAM_BASE;
    if (datos->seedInicial != SEEDHISTORICO)
    {
        enviaLabel("pozoConfig", "pozoPet", "historico mal!");
        enviaLabel("pozoConfig", "pozoMin", "Reseteo historico");
        enviaLabel("pozoConfig", "pozokWhP", "");
        enviaLabel("pozoConfig", "pozokWhV", "");
        reseteaHistoriaPozo();
    }
    else
    {
        minutos =  0;
        numPet = 0;
        kWhP = 0;
        kWhV = 0;
        for (uint8_t id=0;id<8;id++)
        {
            datoId = &datos->datosId[id];
            if (datoId->numPeticiones==0)
                continue;
            uint32_t duracion = datoId->segundosPeticion;
            minutos += duracion/60;
            numPet += datoId->numPeticiones;
            kWhP += datoId->kWhPunta;
            kWhV += datoId->kWhValle;
        }
        chsnprintf(buff,sizeof(buff),"%d pet.",numPet);
        enviaMsg("pozoConfig","pozoPet",buff);
        chsnprintf(buff,sizeof(buff),"%d min.",minutos);
        enviaMsg("pozoConfig","pozoMin",buff);
        chsnprintf(buff,sizeof(buff),"%d kWhP",kWhP);
        enviaMsg("pozoConfig","pozokWhP",buff);
        chsnprintf(buff,sizeof(buff),"%d kWhV",kWhV);
        enviaMsg("pozoConfig","pozokWhV",buff);
    }
    enviaEstado("pozoConfig", "bombaOn", petBomba);

    chsnprintf(buff,sizeof(buff),"%3.1f Bar",presion);
    enviaLabel("pozoConfig","bar",buff);
    chsnprintf(buff,sizeof(buff),"%6.2f kW",kW);
    enviaLabel("pozoConfig","kW",buff);
    chsnprintf(buff,sizeof(buff),"%d Hz",(uint16_t) frecuencia);
    enviaLabel("pozoConfig","hz",buff);
    enviaVal("pozoConfig", "noAbusar", bloqueoAbusones.getValorNum());
    enviaTxt("pozoConfig", "tAbuso", tiempoAbuso.getValorNum());
    enviaTxt("pozoConfig", "IDBar", parametroPresionMB.getValorNum());
    enviaTxt("pozoConfig", "IDHz", parametroFrecuenciaMB.getValorNum());

}
