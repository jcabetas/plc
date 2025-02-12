/*
 * leeMsgCan.cpp
 *
 *  Created on: 31 oct. 2021
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;

#include "dispositivos.h"
#include "string.h"
#include "stdlib.h"
#include "tty.h"


//enum tipoMsgCan_t {cambiaID=1, medidaCan, state,confCargador,setSP, estadoCargador, potenciaVE, medidaCasa, IVE, pContratadas, setFechaHora};

uint8_t can::leeMsg(CANRxFrame rxmsg)
{
    tipoMsgCan_t tipoMsg;
    uint8_t idCargador;
    uint8_t valorEstado;
    uint16_t idCanState;
    uint16_t idCanMed;
    uint16_t valorInt16, valorInt216;
    float valorFloat;

    /* Formato:
    * - SID: id estacion
    * - Data
    *   Byte 1: tipo de mensaje cambiaID=1, medidaCan, state,confCargador,setSP, estadoCargador, potenciaVE, medidaCasa
    *      Tipo cambiaID
    *          Byte 2: nuevoID
    *      Tipo medidaCan
    *          Byte 2..5 valor float
    *          Byte 6..7 idMedCan
    *      Tipo state
    *          Byte 2    valor uint8_t
    *          Byte 3..4 IdStateCan
    *      Tipo confCargador
    *          Byte 2: Imax
    *          Byte 3: NumFasesMax
    *      Tipo setSP
    *          Byte 2: iD estación destino
    *          Byte 3..4: potencia objetivo en W
    *      Tipo estadoCargador
    *          Byte 2: estado R coche conectado (0: desconectado, 1, conectado, 2: pide, 3:ventilacion)
    *          Byte 3: estado control (4 bits inf) y maestro (4 bits supe)
    *          Byte 4: numFasesReal
    *          Byte 5..6: potencia minima en W
    *          Byte 7..8: potencia maxima en W
    *      Tipo potenciaVE
    *          Byte 2..3: potencia consumida por VE
    *      Tipo medidaCasa
    *          Byte 2..3: potencia de casa en W
    *      Tipo IVE
    *          Byte 2..3: intensidad *10
    *      Tipo pContratadas
    *          Byte 2..3: pPunta
    *          Byte 4..5: pValle
    *      Tipo setFechaHora:
    *          Byte 2: anyo (todos en formato tm)
    *          Byte 3: mes
    *          Byte 4: dia
    *          Byte 5: hora
    *          Byte 6: min
    *          Byte 7: seg
    *          Byte 8: ds
    */

    // tiene contenido?
    if (rxmsg.DLC==0)
        return 1;
    // id del que lo envía
    idCargador = rxmsg.std.SID;
    // leo tipo de mensaje
    tipoMsg = (tipoMsgCan_t) rxmsg.data8[0];
    // resetea contador
    cargador::recibidoPorCan(rxmsg.std.SID);
    switch (tipoMsg)
    {
    case cambiaID: // Byte 1: nuevo ID
        if (rxmsg.DLC==2)
        {
            uint8_t idNuevo = rxmsg.data8[1];
            cargador::canCambiaID(idCargador, idNuevo);
        }
        return 1;
    case medidaCan: // can2med ?
        if (rxmsg.DLC==7)
        {
            memcpy((void *) &valorFloat, &rxmsg.data8[1], 4);
            memcpy((void *) &idCanMed, &rxmsg.data8[5], 2);
            for (uint8_t id=0;id<numCan2Med;id++)
            {
                if (idCanMed==idCan2Med[id] && can2med[id]!=0)
                {
                    can2med[id]->set(&valorFloat, 0);
                    return 0;
                }
            }
            return 1;
        }
        break;
    case state: // can2state?
        if (rxmsg.DLC==4)
        {
            valorEstado = rxmsg.data8[1];
            memcpy((void *) &idCanState, &rxmsg.data8[2], 2);
            for (uint8_t id=0;id<numCan2state;id++)
            {
                if (idCanState == idCan2state[id])
                {
                    estados::ponEstado(can2state[id], valorEstado);
                    return 0;
                }
            }
        }
        return 1;
    case confCargador: // Byte 2: Imax, Byte 3: NumFasesMax
        if (rxmsg.DLC==3)
        {
            uint8_t iMax = rxmsg.data8[1];
            uint8_t numFasesMax = rxmsg.data8[2];
            cargador::canSetConfig(idCargador, iMax, numFasesMax);
        }
        return 1;
    case setSP: // Byte 2: iD estación destino, Byte 3..4: potencia objetivo en W
        if (rxmsg.DLC==4)
        {
            uint8_t idDestino = rxmsg.data8[1];
            memcpy((void *) &valorInt16, &rxmsg.data8[2], 2);
            cargador::canSetSP(idCargador, idDestino, valorInt16);
        }
        return 1;
    case estadoCargador: // estadoCargador: Byte 2: estado R, Byte 3: estado maestro, Byte 4: numFasesReal, Byte 5..6: potencia minima en W, Byte 7..8: potencia maxima en W
        if (rxmsg.DLC==8)
        {
            estadoRes_t estadoR = (estadoRes_t) rxmsg.data8[1];
            // estado control (4 bits inf) y maestro (4 bits supe)
            tipoControlP_t estadoControl = (tipoControlP_t) (rxmsg.data8[2] & 0xF);
            tipoMaestro_t estadoMaestro = (tipoMaestro_t) ((rxmsg.data8[2] & 0xF0)>>4);
            uint8_t numFasesReal = rxmsg.data8[3];
            memcpy((void *) &valorInt16, &rxmsg.data8[4], 2);
            memcpy((void *) &valorInt216, &rxmsg.data8[6], 2);
            cargador::canSetEstadoEnNextion(idCargador, estadoR, estadoControl, estadoMaestro, numFasesReal, valorInt16, valorInt216);
        }
        return 1;
    case potenciaVE: // Byte 2..3: potencia consumida por VE
        if (rxmsg.DLC==3)
        {
            memcpy((void *) &valorInt16, &rxmsg.data8[1], 2);
            cargador::canSetPVE(idCargador, valorInt16);
        }
        return 1;
    case medidaCasa: // Byte 2..3: potencia de casa en W
        if (rxmsg.DLC==3)
        {
            memcpy((void *) &valorInt16, &rxmsg.data8[1], 2);
            cargador::canSetPCasa(valorInt16);
        }
        return 1;
    case IVE: // Byte 2..3: intensidad VE multiplicada por 10
        if (rxmsg.DLC==3)
        {
            memcpy((void *) &valorInt16, &rxmsg.data8[1], 2);
            cargador::canSetIVE(idCargador, (float)(valorInt16/10.0f));
        }
        return 1;
    case pContratadas: // Byte 2..3: pPunta, Byte 4..5: pValle
        if (rxmsg.DLC==5)
        {
            memcpy((void *) &valorInt16, &rxmsg.data8[1], 2);
            memcpy((void *) &valorInt216, &rxmsg.data8[3], 2);
            cargador::canSetPContratadas(valorInt16, valorInt216);
        }
        return 1;
    case setFechaHora: // Byte 2: anyo, byte 3: mes, byte 4: dia Byte 5: hora, byte 6: min, byte 7: seg, byte 8:ds
        if (rxmsg.DLC==8)
        {
            calendar::cambiaFechaTM(rxmsg.data8[1], rxmsg.data8[2], rxmsg.data8[3], rxmsg.data8[4], rxmsg.data8[5],rxmsg.data8[6],rxmsg.data8[7]);
        }
        return 1;
    }
    return 1;
}




