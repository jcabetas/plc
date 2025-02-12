/*
 * regExc.cpp
 *
 * // Regulador de excedentes
    class regExc
    {
    protected:
        int8_t numDispExcedentes;
        medida *medPTotal;
        medida *medkWhImp;
        medida *medkWhExp;
        float  kWhBalStart;
        bool hayBalkWh;
        medida *medReguladas[MAXDISPEXC];
        float maxPot[MAXDISPEXC];
    public:
        regExc(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
        ~regExc();
        const char *diTipo(void);
        const char *diNombre(void);
        int8_t init(void);
        uint8_t calcula(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void print(BaseSequentialStream *tty);
        void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
        void statusSMS(sms *smsPtr);
        void actualizaNextion(void);
    };
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "bloques.h"
#include "dispositivos.h"
#include "nextion.h"
#include "string.h"
#include "stdlib.h"


/*
 * Regulador de excedentes
 * - Lee balance de potencia y de energía
 * - Determina potencia total que se puede repartir (potencia + kWhBalance/tiempo_restante_hora)
 * - Asigna potencias progresivamente; antes de pasar de dispositivo espera tiempo, idem al bajar
 */
// REGEXC pTotal kWhImp kWhExp pSPDisp1 pMax1 pSPDisp2    pMax2
// REGEXC pTot   kWhImp kWhExp WTermo  2000  WCargador 11040
regExc::regExc(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar<6)
    {
        nextion::enviaLog(tty,"# params incorrecto REGEXC");
        *hayError = 1;
        return;
    }
    numDispExcedentes = ((numPar-4)>>1);
    if ((4+numDispExcedentes*2) != numPar)
    {
        nextion::enviaLog(tty,"Falta medMax en REGEXC?");
        *hayError = 1;
        return;
    }
    if (numDispExcedentes>MAXDISPEXC)
    {
        nextion::enviaLog(tty,"Muchos dispositivos en REGEXC");
        *hayError = 1;
        return;
    }

    /*
        int8_t numDispExcedentes;
        medida *medPTotal;
        medida *medkWhImp;
        medida *medkWhExp;
        float  kWhBalStart;
        bool hayBalkWh;
        medida *medReguladas[MAXDISPEXC];
        float maxPot[MAXDISPEXC];
     */

    medPTotal = medida::findMedida(pars[1]);
    medkWhImp = medida::findMedida(pars[2]);
    medkWhExp = medida::findMedida(pars[3]);
    for (uint8_t d=1;d<=numDispExcedentes;d++)
    {
        medReguladas[d-1] = medida::findMedida(pars[2+d*2]);
        maxPot[d-1] = atof(pars[3+d*2]);
    }
    return;
}


regExc::~regExc()
{
}

const char *regExc::diNombre(void)
{;
    return "REGEXC";
}

const char *regExc::diTipo(void)
{
    return "REGEXC";
}

int8_t regExc::init(void)
{
    kWhBalStart = 0.0f;
    hayBalkWh = false;
    float pCero = 0.0f;
    for (uint8_t d=1;d<=numDispExcedentes;d++)
        medReguladas[d-1]->set(&pCero,1);
    return 0;
}

uint8_t regExc::calcula(uint8_t , uint8_t hora, uint8_t , uint8_t )
{
    if (!hayBalkWh && medkWhImp->esValida() && medkWhExp->esValida())
    {
        kWhBalStart = medkWhExp->diValor() - medkWhImp->diValor();
        horaRefkWh = hora;
        hayBalkWh = true;
    }
    return 0;
}

void regExc::addTime(uint16_t , uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    // calcula cada 5s
    if ((seg % 5)!=0 || ds!=0)
        return;
    // reseteo kWh cada hora
    if (hora != horaRefkWh)
    {
        kWhBalStart = medkWhExp->diValor() - medkWhImp->diValor();
        horaRefkWh = hora;
    }
    // mira lo que todavia podemos incrementar
    float pExc = -medPTotal->diValor();
    if (hayBalkWh)
    {
        float kWhBal = medkWhExp->diValor() - medkWhImp->diValor() - kWhBalStart;
        float minRest = 60.0f - min;
        float pAdicW = kWhBal*60000.0f/minRest;
        pExc += pAdicW;
    }
    // limito la variacion de potencia a 200 W
    if (pExc > 200.0f)
        pExc = 200.0f;
    if (pExc < -200.0f)
        pExc = -200.0f;
    // reparte excesos donde quepan
    if (pExc>=0.0f)
    {
        for (uint8_t d=1;d<=numDispExcedentes;d++)
        {
            float potDisp = medReguladas[d-1]->diValor();
            if (potDisp < maxPot[d-1]) // puedo subir algo?
            {
                float incPotPosible = maxPot[d-1] - potDisp;
                if (incPotPosible >= pExc) // limita la potencia a la capacidad disponible
                    incPotPosible = pExc;
                pExc -= incPotPosible;     // queda menos potencia a repartir
                float nuevaPot = potDisp + incPotPosible;
                medReguladas[d-1]->set(&nuevaPot,1);
                // termina bucle si no hay nada mas que repartir
                if (pExc<1.0f)
                    break;
            }
            else
                medReguladas[d-1]->set(&potDisp,1); // refresco valor
        }
    }
    else
    // hay que reducir potencia
    {
        for (uint8_t d=numDispExcedentes;d>=1;d--)  // vemos si alguno tiene potencia asignada
        {
            float potDisp = medReguladas[d-1]->diValor();
            if (potDisp > 0.0f) // puedo quitar algo
            {
                float decPotPosible = potDisp;
                if (decPotPosible >= -pExc) // limita la bajada a cero
                    decPotPosible = -pExc;
                pExc += decPotPosible;     // queda menos potencia a repartir
                float nuevaPot = potDisp - decPotPosible;
                medReguladas[d-1]->set(&nuevaPot,1);
                // termina bucle si no hay nada mas que repartir
                if (pExc<-1.0f)
                    break;
            }
            else
                medReguladas[d-1]->set(&potDisp,1); // refresco el valor
        }
    }
}

void regExc::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"regExc");
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}

