/*
 * control.cpp
 *
 *  Created on: 23 jun. 2021
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "chprintf.h"

#include "bloques.h"
#include "dispositivos.h"
#include "colas.h"
#include "string.h"
#include "calendar.h"
#include "nextion.h"

extern struct queu_t colaMsgTxCan;
extern event_source_t sendMsgCAN_source;

void cargador::calculaNumFases(void)
{
    uint8_t numF, numFOld;
    if (numFasesMax[0]==1)
    {
        numFases[0] = 1;
        return;
    }
    numF = 0;
    numFOld = numFases[0];
    for (uint8_t f=0;f<3;f++)
        if (medICoche[f]!=NULL && medICoche[f]->diValor()>1.0f)
            numF++;
    if (numF>numFasesMax[0])
        numF = numFasesMax[0];
    if (numF>=1)
    {
        numFases[0] = numF;
        if (numFases[0]!=numFOld)
            enviaTxt(cargadorPtr->idPagecoches, "fases1", numFases[0]);
    }
    else
        numFases[0] = 0;//numFasesMax[0];
}

uint8_t cargador::estimaFases(void)
{
    if (numFases[0]==0)
        return 1;
    else
        return numFases[0];
}

float cargador::potencia2i(float pSetPoint)
{
    if (numFasesMax[0]==1)
        return pSetPoint/225.0f;
    return pSetPoint/(225.0f*((float) estimaFases()));
}

float cargador::i2potencia(float amp)
{
    if (numFasesMax[0]==1)
        return amp*225.0f;
    return amp*225.0f*((float) estimaFases());
}

float cargador::minPotencia(void)
{
    return 225.0f*5.0f*estimaFases();
}

void cargador::controlPropio(uint8_t seg, uint8_t ds)
{
    float iSP;
    char buffer[10];
    if (statusResis[0]==RDESCONOCIDO || statusResis[0]==RDESCONECTADO)
    {
        fijaAmperios(0.0);
        iSP = 0.0f;
        if (!osciladorOculto)
            ocultaOscilador();
    }
    else if (!permisoDeCarga)
    {
        iSP = 0.0f;
        float pSP = 225.0f*iSP*estimaFases();
        medPSetPoint->set(&pSP, 0);
        fijaAmperios(iSP);
        if (osciladorOculto)
            sacaOscilador();
    }
    else if ((tipoControlP_t) tipoControlPSelf==CPFIJO) // Ifija
    {
        // podemos cargar?
        if (permisoDeCarga)
        {
            // vemos la intensidad deseada y el permiso de cargar
            iSP = IMaxSelf[ajusteAplicable]->valor();
            if (iSP>iMax[0])
                iSP = iMax[0];
        }
        else
            iSP = 0.0f;
        float pSP = 225.0f*iSP*estimaFases();
        medPSetPoint->set(&pSP, 0);
        fijaAmperios(iSP);
        if (osciladorOculto)
            sacaOscilador();
    }
    else if ((tipoMaestro_t) tipoMaestroSelf!=TMFIJO) // Pongo lo que me diga el maestro
    {
        if (medPSetPoint->esValida())
        {
            float pSP = medPSetPoint->diValor();
            if (pSP<0.0f)
                pSP = 0.0f;
            iSP = potencia2i(pSP);
            if (iSP>iMax[0])
                iSP = iMax[0];
        }
        else
            iSP = 0.0f;
        if (osciladorOculto)
            sacaOscilador();
        fijaAmperios(iSP);
    }
//    if (!osciladorOculto)
//    {
//        // chequea onda negativa
//        uint32_t valNeg = ADC1->JDR1;
//        if (valNeg>3800)
//            ondaNegOk = 1;
//        else
//            ondaNegOk = 0;
//    }
    uint16_t medICocheInt = (uint16_t) 10*medICoche[0]->diValor();
    if (ds==3)
    {
        if ((numFases[0]==0 || seg%5==0 ) && medICoche[0]->diValor()>6.0) // parece que ya hay intensidades, mira si podemos estimar numero de fases
            calculaNumFases();
        if (medICocheOld!=medICocheInt)
        {
            chsnprintf(buffer,sizeof(buffer),"%.1f",medICoche[0]->diValor());
            enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "I1", buffer);
            medICocheOld = medICocheInt;
        }
    }
    uint16_t medPCocheInt = (uint16_t) medPCoche->diValor();
    if (ds==5 && medPCocheOld!=medPCocheInt)
    {
        char buffer[10];
        chsnprintf(buffer,sizeof(buffer),"%d",medPCocheInt);
        enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "PVE1", buffer);
        medPCocheOld = medPCocheInt;
    }
    uint16_t medPTotalInt = (uint16_t) medPTotal->diValor();
    if (ds==7 && medPTotalOld!=medPTotalInt)
    {
        char buffer[10];
        chsnprintf(buffer,sizeof(buffer),"%d",medPTotalInt);
        enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "pTot", buffer);
        medPTotalOld = medPTotalInt;
    }
}



void cargador::controlConjunto(uint8_t ds)
{
    float pParaCoches, pCoche;
//    float pCoches, margen;
    uint16_t potCar;
    struct msgCanTx_t msgCanTx;
    /*
     * Ejemplo:
     *  Pcontr = 10.000W
     *  Pc1 = 4.000
     *  Pc2 = 3.000
     *  Pc3 = 0.0
     *  Pcasa = 11.000
     *  => margen = 10.000 - 11.000 = -1000
     *  P disponible para coches = 10.000 - 11.000 + 4.000 + 3.000 = 6.000
     *  => repartimos potencia, sin subir de la actual
     *  Pc1 = min(4.000, 6000) = 4.000CMAESTRO
     *  Pc2 = min(3.000, 2000) = 2.000
     */
    /*
     * Algoritmo:
     * - Calculo potencia disponible para los coches
     * - Asigno esa potencia considerando:
     *   * Solo a coches que piden
     *   * Si Preal<Pmin+500 el minimo es Pmin+500 (para dejar arrancar)
     *   * Si Preal>Pmin+500, el máximo es Preal+500
     *
     */
    // actualizo valores propios en matriz de datos
    PCoche[0] = medPCoche->diValor();
//    // calcula potencia total de los coches
//    pCoches = medPCoche->diValor();
//    for (int8_t car=1;car<numCoches;car++)
//    {
//        estadoRes_t  statusRes= (estadoRes_t) statusResis[car];
//        if (statusRes==RPIDECARGA || statusRes==RPIDEVENTILACION)
//            pCoches += PCoche[car];
//    }
//    // vemos la potencia contratada y el permiso de cargar
//    float pCont = pContratada;
//    if (tipoControlPSelf==CPNOEXPORTA)
//        pCont = 0;
//    // potencia para los coches
//    margen = pCont + medPTotal->diValor();
//    pParaCoches = margen + pCoches;
//
    pParaCoches = medPTotal->diValor();

    // Asigno a coches sucesivamente
    for (int8_t car=0;car<numCoches;car++)
    {
        estadoRes_t  statusRes= (estadoRes_t) statusResis[car];
        if (permisoDeCarga && (statusRes==RCONECTADO || statusRes==RPIDECARGA || statusRes==RPIDEVENTILACION))
        {
            float pMinCarga = minP[car];
            if (PCoche[car] < pMinCarga)
            {
                if (pParaCoches > pMinCarga)
                    setPointP[car] = pMinCarga+900.0f;
                else
                    setPointP[car] = 0.0f;
            }
            else
            {
                if (pParaCoches > PCoche[car]+900.0f)
                {
                    setPointP[car] =  PCoche[car]+900.0f;
                    if (setPointP[car]>pParaCoches)
                        setPointP[car] = pParaCoches;
                }
                else
                    setPointP[car] =  pParaCoches;
                if (setPointP[car] > maxP[car])
                    setPointP[car] = maxP[car];        // ... y envia el setPoint por CAN
                potCar = (uint16_t) setPointP[car];
                msgCanTx.msgEID = diId();
                msgCanTx.msgDLC = 4;
                msgCanTx.msg[0] = setSP;
                msgCanTx.msg[1] = idCharger[car];
                memcpy(&msgCanTx.msg[2],&potCar,2);
                putQueu(&colaMsgTxCan, &msgCanTx);
                chEvtBroadcast(&sendMsgCAN_source);
            }
        }
        else
            setPointP[car] = 0.0f;
        // pParaCoches -= setPointP[car];
        // Ojo, descuento medida real, no el setpoint!!
        pParaCoches -= PCoche[car];
        // me aplico lo que me haya salido ...
        if (car==0 && tipoMaestroSelf!=TMFIJO) // control auto para mi?
        {
            pCoche = setPointP[car];
            medPSetPoint->set(&pCoche,0);
        }
        // ... y envia el setPoint por CAN
        potCar = (uint16_t) setPointP[car];
        msgCanTx.msgEID = diId();
        msgCanTx.msgDLC = 4;
        msgCanTx.msg[0] = setSP;
        msgCanTx.msg[1] = idCharger[car];
        memcpy(&msgCanTx.msg[2],&potCar,2);
        putQueu(&colaMsgTxCan, &msgCanTx);
        chEvtBroadcast(&sendMsgCAN_source);
        // actualiza tabla de coches
        if (ds==2)
        {
            char buffer[5];
            char buffer2[10];
            // actualiza tabla solo si hay cambios en setpoint
            if (potCar!=setPointPOld[car])
            {
                chsnprintf(buffer,sizeof(buffer),"SP%d",car+1);
                chsnprintf(buffer2,sizeof(buffer2),"%d",potCar);
                enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, buffer2);
                setPointPOld[car]= potCar;
            }
        }
        if (ds==0 && car==0)
        {
            if (kWhIniCoche>=-10.0f)
            {
                float kWhParc = medkWhCoche->diValor() - kWhIniCoche;
                medkWhParcCoche->set(&kWhParc, 0);
            }
        }
    }
}

