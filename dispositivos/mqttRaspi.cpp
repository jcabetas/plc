/*
 * mqttRaspi.cpp
 *
 *  Created on: 8 abr. 2022
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;
#include "dispositivos.h"
#include "nextion.h"
#include "colas.h"
#include <stdlib.h>
#include <string.h>

/*
 * Gestion de protocolo mqtt:
 * =========================
 * - Se necesita un ordenador con el broker mqtt instalado
 * - Los dispositivos que necesiten recibir mensajes mqtt, envian solicitud al ordenador indicando número identificador, topic,
 *   y si es JSON la variable info que quieres descifrar
 *      {"evento":"suscribir","idDisp":"1","topic":"stat/3E5176/RESULT","info":"POWER"}
 *      {"evento":"suscribir","idDisp":"3","topic":"presSala"}
 *   El ordenador al recibir el topic enviara identificador y valor (si no es JSON), y si es tipo JSON envia tambien un JSON con los valores pedidos
 *      {"evento":"mqttMsg","idDisp":"1", "valor":3200, "info":"ENERGY.POWER"}
 * - Para enviar un mensaje, lo pide especificando el topic y el valor
 *
 *  Ejemplo
 *  =======
 *       MQTT 192.168.8.30 1883 joaquin JeW31ZT9Rdx
 *       SONOFF 3E5176 calCocina Pcocina EnerCocina
 *       MQTT2STATUS garaje/presencia hayGente
 *
 * MQQT:
 * =====
 * - cuando se inicia, pide a Raspi la conexion con mensaje
 *      PLC=>RASPI {"evento":"mqtt","host":"192.168.8.30","puerto":"1883","user":"joaquin","password":"JeW31ZT9Rdx"}
 * - Lleva una cuenta de dispositivos registrados, y les envía y recibe datos (creo que no hace falta thread)
 *
 * SONOFF
 * ======
 * Envio de ordenes a interruptores inteligentes mediante mqtt y Raspi
 * Por ejemplo, el SONOFF POW
 * - Se pone en marcha con "cmnd/3E5176/power" "ON"
 * - Se desconecta con msg="OFF"
 * - El estado se comunica cuando cambia con stat/3E5176/RESULT {"POWER":"ON"}
 *                           y stat/3E5176/POWER  ON
 *            se puede pedir con cmnd/3E5176/power ""
 * - La medida viene tele/3E5176/SENSOR    payload: {"Time":"2022-04-08T11:54:46","ENERGY":{"TotalStartTime":"2022-04-08T09:42:25","Total":0.000,"Yesterday":0.000,"Today":0.000,"Period": 0,"Power": 1,"ApparentPower":13,"ReactivePower":13,"Factor":0.10,"Voltage":221,"Current":0.059}}
 * - Se puede pedir con -t "cmnd/3E5176/Status" "10"
 * -    => stat/3E5176/STATUS10    payload: {"StatusSNS":{"Time":"2022-04-08T12:04:13","ENERGY":{"TotalStartTime":"2022-04-08T09:42:25","Total":0.000,"Yesterday":0.000,"Today":0.000,"Power": 0,"ApparentPower": 0,"ReactivePower": 0,"Factor":0.00,"Voltage":221,"Current":0.000}}}
 *
 *
 * SONOFFPOW:
 * - Cuando se inicia, pide a MQTT la suscripción a una info concreta, que lo envia a RASPI
 *      PLC=>RASPI {"evento":"suscribir","idDisp":"1","topic":"stat/3E5176/RESULT","info":"POWER"}
 *      PLC=>RASPI {"evento":"suscribir","idDisp":"1","topic":"tele/3E5176/SENSOR","info":"ENERGY.TOTAL"}
 *      PLC=>RASPI {"evento":"suscribir","idDisp":"2","topic":"tele/487661/SENSOR","info":"ENERGY.TOTAL"}
 * - Los mensajes del PLC, se envian a MQTT que informara a SONFOFF con
 *      RASPI=>PLC {"evento":"mqttMsg","idDisp":"1","info":"ENERGY.POWER","valor":3200}
 * - Si quiere enviar mensaje
 *      PLC=>RASPI {"evento":"enviarMqtt","topic":"cmnd/3E5176/power","msg":"OFF"}
 *
 * MQTT2STATUS
 * ===========
 * Por ejemplo, la detección de presencia
 * - Cuando se inicia, pide a MQTT la suscripción, que lo envia a RASPI
 *      PLC=>RASPI {"evento":"suscribir","idDisp":"3","topic":"presSala"}
 *      PLC=>RASPI {"evento":"suscribir","idDisp":"4","topic":"presGaraje"}
 * - Los mensajes del PLC, se envian a MQTT que informara a mqtt2status con
 *      RASPI=>PLC {"evento":"mqttMsg","idDisp":"4","valor":1}
 *
 */

/*
 * tele/3E5176/LWT    payload: Online
  stat/3E5176/RESULT    payload: {"POWER":"ON"}
  stat/3E5176/POWER    payload: ON
  tele/3E5176/STATE    payload: {"Time":"2022-04-08T14:36:13","Uptime":"0T02:10:09","UptimeSec":7809,"Heap":26,"SleepMode":"Dynamic",
                        "Sleep":50,"LoadAvg":19,"MqttCount":1,"POWER":"OFF","Wifi":{"AP":1,"SSId":"B535_154B","BSSId":"E2:C1:D8:4B:15:91"
                        ,"Channel":11,"Mode":"11n","RSSI":94,"Signal":-53,"LinkCount":1,"Downtime":"0T00:00:03"}}
  tele/3E5176/SENSOR    payload: {"Time":"2022-04-08T14:36:13","ENERGY":{"TotalStartTime":"2022-04-08T09:42:25",
                      "Total":0.000,"Yesterday":0.000,"Today":0.000,"Period": 0,"Power": 0,"ApparentPower": 0,"ReactivePower": 0,
                      "Factor":0.00,"Voltage": 0,"Current":0.000}}
 *
 */

extern struct queu_t colaMsgTxWWW;
extern event_source_t enviarWWWsource;
//extern mutex_t MtxTxPlc;
void getMutex(void);
void releaseMutex(void);

uint16_t mqttRaspi::idHost = 0;
uint16_t mqttRaspi::port = 0;
uint16_t mqttRaspi::idUsuario = 0;
uint16_t mqttRaspi::idPassword = 0;
uint8_t mqttRaspi::definido = 0;
uint8_t mqttRaspi::estado = 0;
mqttRaspi *mqttRaspi::mqttRaspiPtr = 0;
dispMqtt *mqttRaspi::listDispositivosMqtt[MAXDISPOSITIVOSMQTT] = {0};
uint8_t mqttRaspi::numDispositivosMqtt = 0;
uint16_t mqttRaspi::dsEnEstado = 0;

/*
    static uint16_t idHost;
    static uint16_t port;
    static uint16_t idUsuario;
    static uint16_t idPassword;
    static uint8_t definido;
    static mqttRaspi *mqttRaspiPtr;
    static dispositivo *listDispositivosMqtt[MAXDISPOSITIVOSMQTT];
    static uint16_t numDispositivosMB;
 */
/*
 * Estados:
 *   estado = 0
 *   ==> pide conexion hasta que lo consigas, al principio y cada 20s
 *       cuando raspi confirme la conexion y suscripcion a #, lo pasamos a estado=1
 *   estado = 1
 *   ==> pide a los hijos que se suscriban a mensajes
 *       cuando todos se hayan suscrito y confirmado por Raspi, evolucionamos
 *   estado = 2
 *   ==> estado normal, llama a addTime de los hijos
 *
 *   Con un init, pasamos a estado=0
 */
// MQTT 192.168.8.30 1883 joaquin JeW31ZT9Rdx0
mqttRaspi::mqttRaspi(BaseSequentialStream *tty, uint8_t numPar, char *pars[], uint8_t *hayError)
{
    if (numPar!=5)
    {
        nextion::enviaLog(tty,"#parametros MQTT");
        *hayError = 1;
        return; // error
    }
    if (definido)
    {
        nextion::enviaLog(tty,"#MQTT repetido");
        *hayError = 1;
    }
    idHost = nombres::incorpora(pars[1]);
    port = atoi(pars[2]);
    idUsuario = nombres::incorpora(pars[3]);
    idPassword = nombres::incorpora(pars[4]);
    mqttRaspiPtr = this;
    estado = 0;
    numDispositivosMqtt = 0;
    definido = 1;
    dsEnEstado = 0;
};

mqttRaspi::~mqttRaspi()
{
    numDispositivosMqtt = 0;
    mqttRaspiPtr = NULL;
    definido = 0;
}

uint8_t mqttRaspi::attachMedidas(medida *, uint8_t , char **, uint8_t *)
{
    return 0;
}

uint8_t mqttRaspi::attachOutputPlacaMadre(pinOut *, uint8_t , char **, uint8_t *)
{
    return 0;
}

uint8_t mqttRaspi::attachInputPlacaMadre(pinInput *, uint8_t , char **, uint8_t *)
{
    return 0;
}

int8_t mqttRaspi::addDisp(dispMqtt *disp)
{
    if (numDispositivosMqtt>=MAXDISPOSITIVOSMQTT)
    {
        nextion::enviaLog(NULL,"#demasiados dispositivos MQTT");
        return -1;
    }
    listDispositivosMqtt[numDispositivosMqtt] = disp;
    numDispositivosMqtt++;
    return numDispositivosMqtt-1;
}


const char *mqttRaspi::diTipo(void)
{
    return "MQTT";
}

const char *mqttRaspi::diNombre(void)
{
    return "MQTT";
}

int8_t mqttRaspi::init(void)
{
    initMqtt();
    return 0;
}

void mqttRaspi::addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    if (estado==0) // no estoy siquiera conectado, cada 10s intentar conectar
    {
        dsEnEstado += dsInc;
        if (dsEnEstado>100)
        {
            enviaSolicitudConex();
            dsEnEstado = 0;
        }
        return;
    }
    else if (estado==1) // conectado, pedimos a los hijos que se suscriban
    {
        dsEnEstado += dsInc;
        if (dsEnEstado>100)
        {
            dsEnEstado = 0;
            uint8_t todoConectado = 1;
            for (uint8_t d=1;d<=numDispositivosMqtt;d++)
                listDispositivosMqtt[d-1]->suscribir(&todoConectado);
            if (todoConectado)
                estado = 2;
        }
    }
    for (uint8_t d=1;d<=numDispositivosMqtt;d++)
        listDispositivosMqtt[d-1]->addTime(dsInc, hora, min, seg, ds);
}

void mqttRaspi::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"MQTT %s %d %s %s",
               nombres::nomConId(idHost),port,nombres::nomConId(idUsuario),nombres::nomConId(idPassword));
    nextion::enviaLog(tty, buffer);
}

void mqttRaspi::initWWW(BaseSequentialStream *, uint8_t *)
{
    initMqtt();
}


void mqttRaspi::initMqtt(void)
{
    estado = 0;
    dsEnEstado = 9999;
    for (uint8_t d=1;d<=numDispositivosMqtt;d++)
        listDispositivosMqtt[d-1]->initMqtt();
}

void mqttRaspi::enviaSolicitudConex(void)
{
    //chMtxLock(&MtxTxPlc);
    getMutex();
    chprintf((BaseSequentialStream*)&SD1,"{\"evento\":\"mqttConex\",\"host\":\"%s\",\"puerto\":\"%d\",\"user\":\"%s\",\"password\":\"%s\"}\r",
                     nombres::nomConId(idHost),port,nombres::nomConId(idUsuario),nombres::nomConId(idPassword));
    //chMtxUnlock(&MtxTxPlc);
    releaseMutex();
    dsEnEstado = 0;
}

void mqttRaspi::actualizaEstadoConex(uint8_t estadoMqtt)
{
    if (estadoMqtt==1 && estado==0)  //recien conectado a MQTT
    {
        estado = 1;
        dsEnEstado = 9999; // fuerza a suscribirse a los hijos
    }
    else if (estadoMqtt == 0 && estado>0) // recien desconectado
    {
        estado = 0;
        dsEnEstado = 9999;
    }
}


void mqttRaspi::mqttDispSuscrito(uint16_t numDisp)
{
    dispMqtt *dispMqtt;
    // cuando recibimos confirmacion de suscripcion desde Raspi
    if (numDisp>=numDispositivosMqtt)
        return;
    dispMqtt = listDispositivosMqtt[numDisp];
    if (dispMqtt==NULL)
        return;
    dispMqtt->actualizaEstSusc(1);
}

//void mqttRaspi::mqttMsgRx(uint16_t numDisp, const char *nomReport, const char *valor)
void mqttRaspi::mqttMsgRx(uint16_t numDisp, const char *valor, const char *info)
{
    //        const char* nomReport = doc["idReport"].as<const char *>();
    //        const char* valor = doc["valor"].as<const char *>();
    dispMqtt *dispMqtt;
    // cuando recibimos un mensaje mqtt desde Raspi
    if (numDisp>=numDispositivosMqtt)
        return;
    dispMqtt = listDispositivosMqtt[numDisp];
    if (dispMqtt==NULL)
        return;
    dispMqtt->trataRx(valor, info);
}



