/*
 * www.cpp
 *
 *  Created on: 20 ago. 2021
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"
using namespace chibios_rt;

#include "bloques.h"
#include "nextion.h"
#include "chprintf.h"
#include "dispositivos.h"
#include "calendar.h"
#include "ArduinoJson.h"
#include "string.h"
#include "tty.h"
#include "colas.h"

/*

class www : public bloque
{
protected:
    uint8_t definido = 0;
public:
    www(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    ~www() = 0;
    int8_t init(void);
    void print(BaseSequentialStream *tty);
    void addTime(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
    const char *diTipo(void);
    const char *diNombre(void);
};

*/

uint8_t www::definido = 0;
uint8_t www::plcActivo = 0;
www *www::ptrWWW = NULL;
extern struct queu_t colaMsgTxWWW;
mutex_t MtxTxPlc2;

void getMutex(void)
{
    chMtxLock(&MtxTxPlc2);
}

void releaseMutex(void)
{
    chMtxUnlock(&MtxTxPlc2);
}

// WWW
www::www(BaseSequentialStream *,uint8_t , char **, uint8_t *)
{
    ptrWWW = this;
    definido = 1;
    plcActivo = 0;
}

www::~www()
{
   ptrWWW = NULL;
   definido = 0;
}

uint8_t www::estaDefinido()
{
    return definido;
}

uint8_t www::estaActivo(void)
{
    return plcActivo;
}


int8_t www::init(void)
{

    initWWWThread();
    return 0;
}

void www::stop(void)
{
    mataWWWCom();
}

const char *www::diTipo(void)
{
    return "WWW";
}

const char *www::diNombre(void)
{
    return "WWW";
}

void www::print(BaseSequentialStream *tty)
{
    if (tty!=NULL)
        nextion::enviaLog(tty, "WWW");
}

void www::addTime(uint16_t , uint8_t , uint8_t , uint8_t , uint8_t )
{
}


//He recibido un JSON de Raspi, debo traducirlo a algo como @orden,smsOn,0
void www::trataRxWWW(char *json)
{
    StaticJsonDocument<300> doc;
    char *params[5];
    uint16_t numParam;
    char buffer[80];
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error) {
        json[15] = 0;
        chsnprintf(buffer, sizeof(buffer),"Error json:%s ...",json);
        nextion::enviaLog(NULL, buffer);
        return;
    }
    // Data from RASPI: {"evento":"update","datos":[{"tipo":"SINO","id":"enCasa","variable":"estado","valor":0}]}
    const char* evento = doc["evento"];
    // es "update" ("variable" debe ser "estado")?
    if (!strcasecmp(evento,"update"))
    {
        JsonArray arr = doc["datos"].as<JsonArray>();
        uint8_t siz = arr.size();
        // para cada dato recibido...
        for (int i=0;i<siz;i++)
        {
            const char *id = arr[i]["id"].as<const char *>();
            const char *variable = arr[i]["variable"].as<const char *>();
            const char *tipo = arr[i]["tipo"].as<const char *>();
            if (!strcasecmp("estado",variable))
            {
                int valor = arr[i]["valor"].as<int>();
                if (valor>=0 && valor<=2)
                {
                    // Envio @orden,id,valor
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,%d",id,valor);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);
                }
            }
            // es orden para programador?
            else if (!strcasecmp("PROG",tipo))
            {
                const char *accion = arr[i]["accion"].as<const char *>();
                if (!strcasecmp("setMinZ",accion))
                {
                    //{"evento":"update","datos":[{"tipo":"PROG","accion":"setMinZ","id":"riegoProg","zona":1,"codAB":"B","tiempo":"12"}]
                    //@orden,riego,setMinZ0,a,15
                    const char *AB = arr[i]["codAB"].as<const char *>();
                    uint8_t zona = arr[i]["zona"].as<int>();
                    const char *tiempo = arr[i]["tiempo"].as<const char *>();
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,setMinZ%d,%s,%s",id,zona-1,AB,tiempo);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);
                } else if (!strcasecmp("zona",accion))
                {
                    //{{"tipo":"PROG","accion":"zona","id":"riegoProg","zona":2}
                    //@orden,riego,zona 0 a
                    uint8_t zona = arr[i]["zona"].as<int>();
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,zona,%d,a",id,zona);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);
                }
                else if (!strcasecmp("start",accion))
                {
                    //{{"tipo":"PROG","accion":"arranca","id":"riegoProg"}
                    //@orden,riego,start,a-b
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,start,a",id);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);
                }
                else if (!strcasecmp("startb",accion))
                {
                    //{{"tipo":"PROG","accion":"arranca","id":"riegoProg"}
                    //@orden,riego,start,a-b
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,start,b",id);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);
                }
                else if (!strcasecmp("stop",accion))
                {
                    //{{"tipo":"PROG","accion":"stop","id":"riegoProg"}
                    //@orden,riego,stop
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,stop",id);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);
                }
                else if (!strncasecmp("SETDOW",accion,6))
                {
                    // {"tipo":"PROG","id":"riegoProg","accion":"SETDOW0","valor":"LMXJVS"}
                    // @orden,riego,setDOWX,LMD
                    const char *dowStr = arr[i]["valor"].as<const char *>();
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,%s,%s",id,accion,dowStr);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);
                }
                else if (!strncasecmp("SETH",accion,4) || !strncasecmp("SETM",accion,4))
                {
                    // {"tipo":"PROG","id":"riegoProg","accion":"setH0","tiempo":"9"}
                    // @orden,riego,setH0,13 o @orden,riego,setM0,58
                    uint16_t tiempo = arr[i]["tiempo"].as<int>();
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,%s,%d",id,accion,tiempo);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);
                }
                else if (!strncasecmp("SETESP",accion,6))
                {
                    // {"tipo":"PROG","id":"riegoProg","accion":"setEsp0","esB":1}
                    // @orden,riego,setEsp0,1
                    uint8_t esB = arr[i]["esB"].as<int>();
                    chsnprintf(buffer, sizeof(buffer),"@orden,%s,%s,%d",id,accion,esB);
                    parseStr((char *)buffer, params, ", ", &numParam);
                    bloque::ordenNextionBlq(params,numParam);

                }
            }
        }
    }
    else if (!strcasecmp(evento,"init"))
    {
        clearQueu(&colaMsgTxWWW);
        // Vuelca por SD6 valores iniciales
        //chMtxLock(&MtxTxPlc);
        getMutex();
        bloque::initWWW((BaseSequentialStream *)&NEXTIONSD);
        //chMtxUnlock(&MtxTxPlc);
        releaseMutex();
        plcActivo = 1;
        mqttRaspi::actualizaEstadoConex(0);
    }
    else if (!strcasecmp(evento,"mqttEstado"))
    {
        uint8_t estatus = doc["estado"].as<int>();
        mqttRaspi::actualizaEstadoConex(estatus);
    }
    else if (!strcasecmp(evento,"mqttMsg"))
    {
        // {"evento":"mqttMsg",""idDisp":"'+item.idDisp+'","idReport":"'+item.idReport+'","valor":"'+dato+'"}'
        uint16_t numDisp = doc["idDisp"].as<int>();
        const char* info = doc["info"].as<const char *>();
        const char* valor = doc["valor"].as<const char *>();
        mqttRaspi::mqttMsgRx(numDisp, valor, info);
    }
    else if (!strcasecmp(evento,"mqttSuscrito"))
    {
        // {"evento":"mqttSuscrito",""idDisp":"'+item.idDisp+'"}'
        uint16_t idDisp = doc["idDisp"].as<int>();
        mqttRaspi::mqttDispSuscrito(idDisp);
    }
    else if (!strcasecmp(evento,"fechahora"))
    {
        // recibo datos para estructura tm+ds en JSON
        // {"evento":"fechahora","datos":[2022,1,30,8,23,55,8]}
        JsonArray arr = doc["datos"].as<JsonArray>();
        uint8_t siz = arr.size();
        if (siz!=7)
            return;
        struct tm fecha;
        fecha.tm_year = arr[0].as<int>() - 1900;
        if (fecha.tm_year<120) // fecha < 2020???
            return;
        fecha.tm_mon = arr[1].as<int>() - 1;
        fecha.tm_mday = arr[2].as<int>();
        fecha.tm_hour = arr[3].as<int>();
        fecha.tm_min = arr[4].as<int>();
        fecha.tm_sec = arr[5].as<int>();
        uint8_t ds = arr[6].as<int>();
        completeYdayWday(&fecha); // pongo bien dia de la semana
        calendar::rtcSetFecha(&fecha, ds);
        fecha::actualizaCuandoPuedas();
        if (cargador::cochesConectados()>0)
        {
            uint16_t ds;
            rtcGetTM(&RTCD1, &fecha, &ds);         // leo hora local con ds
            calendar::enviaFechayHoraPorCAN(&fecha,ds);
        }
    }
}
