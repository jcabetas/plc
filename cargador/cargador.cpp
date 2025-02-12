/*
 * cargador.cpp
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
#include "colas.h"

void initADC(void);

cargador *cargador::cargadorPtr = NULL;
uint8_t cargador::numAjustes = 0;

extern struct queu_t colaMsgTxCan;
extern event_source_t sendMsgCAN_source;

char tipoMaestroStr[4][10] = {"Fijo","Esclavo","Maestro","Desc",};
char tipoControlPStr[4][10] = {"Fijo","Maximo","No Exp.","Desc"};

//typedef enum {CPDESCONOCIDO=0, CPFIJO, CPMAXIMO, CPNOEXPORTA} tipoControlP_t;
//typedef enum {TMDESCONOCIDO=0, TMFIJO, TMESCLAVO, TMMAESTRO} tipoMaestro_t;

// CARGADOR car.base.picc.0 ajustes [ID 1] Ptot [Ppunta 10000] [Pvalle 15000] coches SP 16 [mAmin 4300] Pve kWhTot kWhParc          3 Ive Ive Ive
// CARGADOR car.base.picc.0 ajustes [ID 1] Ptot [Ppunta 10000] [Pvalle 15000] coches SP 32 [mAmin 4300] Pve kWhTot kWhParc.base.txt 1 Ive

cargador::cargador(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{
    char buffer[50];
    if (numPar!=16 && numPar!=18)
    {
        nextion::enviaLog(tty,"#parametros incorrecto CARGADOR");
        *hayError = 1;
        return;
    }
    if (cargadorPtr!=NULL)
    {
        nextion::enviaLog(tty,"Solo puede haber un CARGADOR");
        *hayError = 2;
        return;
    }
    cargadorPtr = this;
    // inicializo datos
    for (uint8_t i=0;i<MAXCARGADORES;i++)
    {
        iMax[i] = 0.0f;
        numFasesMax[i] = 1;
        idCharger[i] = 0;
        setPointP[i] = 0.0f;
        minP[i] = 1800.0f;
        maxP[i] = 2300.0f; // 10A
        statusResis[i] = RDESCONOCIDO;
        tipoControlP[i] = CPDESCONOCIDO;
        tipoMaestro[i] = TMDESCONOCIDO;
        numFases[i] = 1;
        PCoche[i] = 0.0f;
        setPointPOld[i] = 9999;
        dsSinComunica[i] = 99;
    }
    for (uint8_t i=0;i<MAXAJUSTES;i++)
    {
        idPageCuando[i] = 0;
        statusPermP[i] = NULL;
        statusPermL[i] = NULL;
        statusPermV[i] = NULL;
        horaIni[i] = NULL;
        horaFin[i] = NULL;
        soloSol[i] = NULL;
        tipoControlPCuando[i] = NULL;
        IMaxSelf[i] = NULL;
    }
    ajusteAplicable = -1;
    factorP = 1.0f;
    permisoDeCarga = 0;
    pContratadaValle = NULL;
    pContratadaPunta = NULL;
    ondaNegOk = 0;
    antDsOndaNegOk = 1;
    osciladorOculto = 1;
    haCambiadoR = 0;
    numDatHist = 0;
    kWhIniCoche = -999.0f;
    numCoches = 1;
    medPTotalOld = 9999;
    medPCocheOld = 9999;
    medICocheOld = 9999;

    // antes  CARGADOR car.base.picc.0 [ID.ajustes 2] SP2 32 Pve2 kWhTot2 kWhParc2 1 Ive2
    //        CARGADOR car.base.picc.0 ajustes [ID 1] Ptot [Ppunta 10000] [Pvalle 15000] coches SP 32 Pve kWhTot kWhParc.base.txt 1 Ive

    nombre = campoNextion::addCampoNextion(pars[1], TIPONEXTIONCROP, hayError);
    idPageAjustes = nombres::incorpora(pars[2]);
    idChargerSelf = parametro::addParametroU16FlashMinMax(tty, pars[3],idPageAjustes,TIPONEXTIONSTR,0,0,99,hayError);
    idCharger[0] = idChargerSelf->valor();
    medPTotal = medida::findMedida(pars[4]);
    oldSP = 0.0f;
    if (medPTotal==NULL)
    {
        chsnprintf(buffer,sizeof(buffer),"No encontrada medida %s",pars[4]);
        nextion::enviaLog(tty,buffer);
        *hayError = 1;
        return;
    }
    pContratadaPunta = parametro::addParametroU16FlashMinMax(tty, pars[5],idPageAjustes,TIPONEXTIONSTR,0,100,20000,hayError);
    pContratadaValle = parametro::addParametroU16FlashMinMax(tty, pars[6],idPageAjustes,TIPONEXTIONSTR,0,100,20000,hayError);
    idPagecoches = nombres::incorpora(pars[7]);
    limpiaCochesScreen();
    enviaTxt(idPagecoches, "ID1", idChargerSelf->valor());
    enviaTxt(nombres::nomConId(idPagecoches), "cont1", tipoControlPStr[tipoControlPSelf]);
    medPSetPoint = medida::findMedida(pars[8]);
    if (medPSetPoint == NULL)
    {
        nextion::enviaLog(tty,"Medida SP no encontrada en CARGADOR");
        *hayError = 3;
        return;
    }
    float ceroFloat = 0.0f;
    medPSetPoint->set(&ceroFloat, 0);
    iMax[0] = atoi(pars[9]);
    maxP[0] = 235.0f*iMax[0];

    mAmin = parametro::addParametroU16FlashMinMax(tty, pars[10],idPageAjustes,TIPONEXTIONSTR,0,1000,10000,hayError);

    medPCoche = medida::findMedida(pars[11]);
    if (medPCoche == NULL)
    {
        nextion::enviaLog(tty,"Medida potencia coche no encontrada en CARGADOR");
        *hayError = 4;
        return;
    }
    medkWhCoche = medida::findMedida(pars[12]);
    if (medkWhCoche == NULL)
    {
        nextion::enviaLog(tty,"Medida kWh coche no encontrada en CARGADOR");
        *hayError = 5;
        return;
    }
    medkWhParcCoche = medida::findMedida(pars[13]);
    if (medkWhParcCoche == NULL)
    {
        nextion::enviaLog(tty,"Medida kWh parcial coche no encontrada en CARGADOR");
        *hayError = 6;
        return;
    }
    numFasesMax[0] = atoi(pars[14]);
    if (numFasesMax[0]==1)
        numFases[0] = 1;
    else
        numFases[0] = 0; // solo rellenar cuando de verdad se sepa el número de fases (o cuando el máximo es 1)
    medICoche[0] = medida::findMedida(pars[15]);
    if (medICoche[0] == NULL)
    {
        nextion::enviaLog(tty,"Medida Icoche no encontrada en CARGADOR");
        *hayError = 7;
        return;
    }
    if (numFasesMax[0]==3 && numPar!=18)
    {
        nextion::enviaLog(tty,"Cargador trifasico debe medir int. de tres fases");
        *hayError = 8;
    }
    if (numPar==18)
    {
        medICoche[1] = medida::findMedida(pars[16]);
        if (medICoche[1] == NULL)
        {
            nextion::enviaLog(tty,"Medida Ibcoche no encontrada en CARGADOR");
            *hayError = 9;
            return;
        }
        medICoche[2] = medida::findMedida(pars[17]);
        if (medICoche[2] == NULL)
        {
            nextion::enviaLog(tty,"Medida Iccoche no encontrada en CARGADOR");
            *hayError = 1;
            return;
        }
        minP[0] = 3400.0f; //<>5A
    }
    return;
}

// CARG-AJUSTES cuando [ajustFAM1.cuando.picc.4 0] [Ifija1 10] [punta1 0] [llano1 1] [valle1 1] [hora11 0] [hora12 6] [solosol1 0]
void cargador::defAjustes(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError)
{

    if (numPar!=10)
    {
        nextion::enviaLog(tty,"#parametros incorrecto CARG-AJUSTES");
        *hayError = 1;
        return;
    }
    if (cargadorPtr==NULL)
    {
        nextion::enviaLog(tty,"Define CARGADOR antes de CARG-AJUSTES");
        *hayError = 1;
        return;
    }
    if (numAjustes>=MAXAJUSTES)
    {
        nextion::enviaLog(tty,"Demasiados bloques CARG-AJUSTES");
        *hayError = 1;
        return;
    }
    uint16_t idPageWhen = nombres::incorpora(pars[1]);
    cargadorPtr->idPageCuando[numAjustes] = idPageWhen;
    cargadorPtr->tipoControlPCuando[numAjustes] = parametro::addParametroU16FlashMinMax(tty, pars[2],idPageWhen,TIPONEXTIONCROP,0,0,2,hayError); // 0:fijo  1:auto 2: maestro
    cargadorPtr->IMaxSelf[numAjustes] = parametro::addParametroU16FlashMinMax(tty, pars[3],idPageWhen,TIPONEXTIONSTR,0,0,32,hayError);

    cargadorPtr->statusPermP[numAjustes] = parametro::addParametroU16FlashMinMax(tty, pars[4],idPageWhen,TIPONEXTIONNUMERO,0,0,1,hayError);
    cargadorPtr->statusPermL[numAjustes] = parametro::addParametroU16FlashMinMax(tty, pars[5],idPageWhen,TIPONEXTIONNUMERO,0,0,1,hayError);
    cargadorPtr->statusPermV[numAjustes] = parametro::addParametroU16FlashMinMax(tty, pars[6],idPageWhen,TIPONEXTIONNUMERO,0,0,1,hayError);

    cargadorPtr->horaIni[numAjustes] = parametro::addParametroU16FlashMinMax(tty, pars[7],idPageWhen,TIPONEXTIONSTR,0,0,24,hayError);
    cargadorPtr->horaFin[numAjustes] = parametro::addParametroU16FlashMinMax(tty, pars[8],idPageWhen,TIPONEXTIONSTR,0,0,24,hayError);
    cargadorPtr->soloSol[numAjustes] = parametro::addParametroU16FlashMinMax(tty, pars[9],idPageWhen,TIPONEXTIONNUMERO,0,0,1,hayError);

    numAjustes++;
}



cargador::cargador(void)
{
}

cargador::~cargador()
{
    cargadorPtr = NULL;
    numAjustes = 0;
}


void cargador::setEstadoRes(uint16_t valorADC, estadoRes_t estR)
{
    if (cargadorPtr==NULL)
        return;
    cargadorPtr->haCambiadoR = 1;
    cargadorPtr->nuevaR = estR;
    // grabo en historico
    if (cargadorPtr->numDatHist<NUMREGSHIST)
    {
        cargadorPtr->datADCHist[cargadorPtr->numDatHist] = valorADC;
        cargadorPtr->estadosResHist[cargadorPtr->numDatHist] = estR;
        cargadorPtr->numDatHist++;
    }
}

estadoRes_t cargador::getEstadoRes(void)
{
    if (cargadorPtr==NULL)
        return RDESCONOCIDO;
    return cargadorPtr->statusResis[0];
}

const char *cargador::diTipo(void)
{
    return "cargador";
}


const char *cargador::diNombre(void)
{
    return "car";
}

uint8_t cargador::diId(void)
{
    if (cargadorPtr!=NULL)
        return cargadorPtr->idChargerSelf->valor();
    else
        return 0;
}

float cargador::getPVEtotal(void)
{
    float pCoches = 0.0f;
    for (int8_t car=0;car<=cargadorPtr->numCoches;car++)
    {
        estadoRes_t  statusRes= (estadoRes_t) cargadorPtr->statusResis[car];
        if (statusRes==RPIDECARGA || statusRes==RPIDEVENTILACION)
            pCoches += cargadorPtr->PCoche[car];
    }
    return pCoches;
}

void cargador::recibidoPorCan(uint8_t id)
{
    char buffer[10];
    // busco posicion
    for (uint8_t i=0;i<MAXCARGADORES;i++)
        if (cargadorPtr->idCharger[i]==id)
        {
            cargadorPtr->dsSinComunica[i] = 0;
            return;
        }
    // no existia, busco posicion libre para insertarlo
    for (uint8_t i=0;i<MAXCARGADORES;i++)
        if (cargadorPtr->idCharger[i]==0)
        {
            cargadorPtr->idCharger[i] = id;
            cargadorPtr->dsSinComunica[i] = 0;
            chsnprintf(buffer,sizeof(buffer),"ID%d",i+1);
            enviaTxt(cargadorPtr->idPagecoches, buffer, id);
            cargadorPtr->numCoches++;
            return;
        }
}

void cargador::limpiaCoche(uint8_t coche)
{
    char buffer[10];
    coche++; // los indices empiezan en 1
    chsnprintf(buffer,sizeof(buffer),"ID%d",coche);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, "-");
    chsnprintf(buffer,sizeof(buffer),"est%d",coche);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, "-");
    chsnprintf(buffer,sizeof(buffer),"cont%d",coche);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, "-");
    chsnprintf(buffer,sizeof(buffer),"mast%d",coche);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, "-");
    chsnprintf(buffer,sizeof(buffer),"I%d",coche);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, "-");
    chsnprintf(buffer,sizeof(buffer),"fases%d",coche);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, "-");
    chsnprintf(buffer,sizeof(buffer),"SP%d",coche);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, "-");
    chsnprintf(buffer,sizeof(buffer),"PVE%d",coche);
    enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), buffer, "-");
    chThdSleepMilliseconds(50);
}

void cargador::limpiaCochesScreen(void)
{
    for (uint8_t i=0;i<MAXCARGADORES;i++)
        limpiaCoche(i);
}

void cargador::enviaEstadoCargador2can(void)
{
    struct msgCanTx_t msgCanTx;
    uint16_t valorInt16;

    /*      Tipo estadoCargador
    *          Byte 2: estado R coche conectado (0: desconectado, 1, conectado, 2: pide, 3:ventilacion)
    *          Byte 3: estado control (4 bits inf) y maestro (4 bits supe)
    *          Byte 4: numFasesReal
    *          Byte 5..6: potencia minima en W
    *          Byte 7..8: potencia maxima en W
    */
    if (cargador::cochesConectados()==0) return; // no hay a quien enviar
    msgCanTx.msgEID = diId();
    msgCanTx.msgDLC = 8;
    msgCanTx.msg[0] = estadoCargador;
    msgCanTx.msg[1] = cargadorPtr->statusResis[0];
    msgCanTx.msg[2] = (cargadorPtr->tipoControlPSelf & 0xF ) | ((cargadorPtr->tipoMaestroSelf & 0xF) <<4);
    msgCanTx.msg[3] = cargadorPtr->numFases[0];
    valorInt16 = (uint16_t) cargadorPtr->minP[0];
    memcpy(&msgCanTx.msg[4],&valorInt16,2);
    valorInt16 = (uint16_t) cargadorPtr->maxP[0];
    memcpy(&msgCanTx.msg[6],&valorInt16,2);
    putQueu(&colaMsgTxCan, &msgCanTx);
    chEvtBroadcast(&sendMsgCAN_source);
}


void cargador::enviaPVE(void)
{
    struct msgCanTx_t msgCanTx;
    uint16_t valorInt16;
    if (cargador::cochesConectados()==0) return; // no hay a quien enviar
    // Byte 2..3: potencia consumida por VE
    msgCanTx.msgEID = diId();
    msgCanTx.msgDLC = 3;
    msgCanTx.msg[0] = potenciaVE;
    valorInt16 = (uint16_t) cargadorPtr->medPCoche->diValor(); // PCoche[0];
    memcpy(&msgCanTx.msg[1],&valorInt16,2);
    putQueu(&colaMsgTxCan, &msgCanTx);
    chEvtBroadcast(&sendMsgCAN_source);
}

void cargador::enviaIVE(void)
{
    struct msgCanTx_t msgCanTx;
    uint16_t valorInt16;
    if (cargador::cochesConectados()==0) return; // no hay a quien enviar
    // Byte 2..3: potencia consumida por VE
    msgCanTx.msgEID = diId();
    msgCanTx.msgDLC = 3;
    msgCanTx.msg[0] = IVE;
    valorInt16 = (uint16_t) (10.0f*cargadorPtr->medICoche[0]->diValor());
    memcpy(&msgCanTx.msg[1],&valorInt16,2);
    putQueu(&colaMsgTxCan, &msgCanTx);
    chEvtBroadcast(&sendMsgCAN_source);
}

void cargador::enviaPContratadas(void)
{
    struct msgCanTx_t msgCanTx;
    uint16_t valorInt16;
    if (cargador::cochesConectados()==0) return; // no hay a quien enviar
    // Byte 2..3: potencia punta, 4..5: valle
    msgCanTx.msgEID = diId();
    msgCanTx.msgDLC = 5;
    msgCanTx.msg[0] = pContratadas;
    valorInt16 = (uint16_t) cargadorPtr->pContratadaPunta->valor();
    memcpy(&msgCanTx.msg[1],&valorInt16,2);
    valorInt16 = (uint16_t) cargadorPtr->pContratadaValle->valor();
    memcpy(&msgCanTx.msg[3],&valorInt16,2);
    putQueu(&colaMsgTxCan, &msgCanTx);
    chEvtBroadcast(&sendMsgCAN_source);
}

uint8_t cargador::cochesConectados(void)
{
    if (cargadorPtr==NULL)
        return 0;
    return cargadorPtr->numCoches-1;
}



void cargador::aplicaTipoControl(void)
{
    // en funcion de los ajustes en "cuando", determina:
    // - Si tengo permiso de carga "permisoDeCarga"
    // - Tipo de control aplicable "tipoControlPSelf" (CPFIJO, CPMAXIMO, CPNOEXPORTA)
    // - Si soy maestro o esclavo "tipoMaestroSelf" (TMFIJO, TMESCLAVO, TMMAESTRO)
    //typedef enum {CPDESCONOCIDO=0, CPFIJO, CPMAXIMO, CPNOEXPORTA} tipoControlP_t;
    //typedef enum {TMDESCONOCIDO=0, TMFIJO, TMESCLAVO, TMMAESTRO} tipoMaestro_t;
    //
    struct tm fecha;
    getPeriodoTarifa perTarif = calendar::getPeriodoTarifa();
    calendar::getFecha(&fecha);
    uint8_t hora = fecha.tm_hour;
    uint8_t esDeDia = !calendar::esDeNoche();
    //
    /*
        parametroU16Flash *statusPermP[MAXAJUSTES];
        parametroU16Flash *statusPermL[MAXAJUSTES];
        parametroU16Flash *statusPermV[MAXAJUSTES];
        parametroU16Flash *horaIni[MAXAJUSTES];
        parametroU16Flash *horaFin[MAXAJUSTES];
        parametroU16Flash *soloSol[MAXAJUSTES];
        parametroU16Flash *tipoControlPCuando[MAXAJUSTES];
     */
    permisoDeCarga = 0;
    ajusteAplicable = -1;
    for (int8_t nAj=0;nAj<numAjustes;nAj++)
    {
        uint8_t aplica = 0;
        // aplica este ajuste?
        // suponemos condiciones OR
        if (perTarif==punta && statusPermP[nAj]->valor()==1)
            aplica = 1;
        else if (perTarif==llano && statusPermL[nAj]->valor()==1)
            aplica = 1;
        else if (perTarif==valle && statusPermV[nAj]->valor()==1)
            aplica = 1;
        else
        {
            //
            // si horaIni<horaFin (1-7) => aplica si h>=horaIni y h<horaFin
            // si hora>horaFin (22-6)   => aplica si h>=horaIni o h<horaFin
            uint8_t hIni = horaIni[nAj]->valor();
            uint8_t hFin = horaFin[nAj]->valor();
            if (hIni<hFin && (hora>=hIni && hora<hFin))
                aplica = 1;
            if (hIni>hFin && (hora>=hIni || hora<hFin))
                aplica = 1;
            //
            if (esDeDia && soloSol[nAj]->valor())
                aplica = 1;
        }
        if (aplica)
        {
            ajusteAplicable = nAj;
            permisoDeCarga = 1;
            tipoControlPSelf = (tipoControlP_t) tipoControlPCuando[nAj]->valor();
            //
            if (tipoControlPSelf==CPFIJO)
                tipoMaestroSelf = TMFIJO;
            else
            {
                // hay que controlar, intento ser maestro, pero mirando si es posible
                uint8_t soyMaestro = 1;
                for (int8_t car=1;car<numCoches;car++)
                {
                    if (idCharger[car]>idChargerSelf->valor())
                        continue;
                    tipoMaestro_t tipMaes = (tipoMaestro_t) tipoMaestro[car];
                    if (tipMaes==TMESCLAVO || tipMaes==TMMAESTRO)
                    {
                        // me gana por la mano (si el otro es esclavo,debe promoconar por su cuenta)
                        soyMaestro = 0;
                        break;
                    }
                }
                if (soyMaestro)
                    tipoMaestroSelf = TMMAESTRO;
                else
                    tipoMaestroSelf = TMESCLAVO;
            }
            enviaEstadoCargador2can();
            enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "cont1", (char *) tipoControlPStr[tipoControlPSelf]);
            enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "mast1", (char *) tipoControlPStr[tipoMaestroSelf]);
            return;
        }
    }
}
/*
 *  typedef enum { RDESCONOCIDO=0, RDESCONECTADO, RCONECTADO, RPIDECARGA, RPIDEVENTILACION} estadoRes_t;
    typedef enum { DESCONOCIDO=0, DESCONECTADO, RECIENCONECTADO, CARGANDO_MAX, CARGANDO_POCO,
               CARGADO, ESPERANDOCAPACIDAD, DIODOMAL } estadoCarga_t;
    typedef enum {FIJO=0, AUTO, MAESTRO} estadoControl_t;
 */
char estResStr[5][10] = {"??","Desc","Conec","Pide","PideVent"};
uint8_t cargador::calcula(uint8_t , uint8_t min, uint8_t seg, uint8_t ds)
{
//    if (min==0 && seg==0 && ds==0)
//        estimaValoresADC();
    // mira si es punta o valle
    if (ds==0 && seg==0 && min==0)
    {
        if (calendar::getPeriodoTarifa()==valle)
            pContratada = pContratadaValle->valor();
        else
            pContratada = pContratadaPunta->valor();
    }
    uint8_t hayCambios = 0;
    if (haCambiadoR)
    {
        statusResis[0] = nuevaR;
        haCambiadoR = 0;
        hayCambios = 1;
        enviaEstadoCargador2can();
        nextion::setBasePage();
    }
    if (ds==0)
    {
        if (medkWhCoche->esValida())
        {
            if (kWhIniCoche<-10.0f)
            {
                kWhIniCoche = medkWhCoche->diValor();
                float kWhParc = medkWhCoche->diValor() -  kWhIniCoche;
                medkWhParcCoche->set(&kWhParc, 1);
            }
            else
            {
                float kWhParc = medkWhCoche->diValor() -  kWhIniCoche;
                medkWhParcCoche->set(&kWhParc, 0);
            }
        }
    }
    estadoRes_t statusRes = statusResis[0]; // para evitar usar mucho la indirección
    if (statusRes != statusResisOld)
    {
        // se acaba de conectar?
        if (statusRes==RCONECTADO && statusResisOld==RDESCONECTADO)
        {
            if (numFasesMax[0]!=1)
                numFases[0] = 0;   // ya no sabemos cuantas fases activas tendrá el coche
            if (medkWhCoche->esValida())
            {
                kWhIniCoche = medkWhCoche->diValor();
                float kWhParc = medkWhCoche->diValor() - kWhIniCoche;
                medkWhParcCoche->set(&kWhParc, 1);
            }
            else
                kWhIniCoche = -999.9f;
        }
        // se acaba de desconectar?
//           if (statusResisOld[c]!=RDESCONOCIDO && statusRes==RDESCONECTADO)
//           {
//               kWhIniCoche[c] = medkWhCoche[c]->diValor();
//               float kWhParc = medkWhCoche[c]->diValor() -  kWhIniCoche[c];
//               medkWhParcCoche[c]->set(&kWhParc, 1);
//           }
        statusResisOld = statusResis[0];
//        if (ondaNegOk==1 && (statusRes==RPIDECARGA || statusRes==RPIDEVENTILACION))
//            palSetLine(LINE_RELE);
//        else
//            palClearLine(LINE_RELE);
        estadoCarga_t estC = statusCarga;
        if (statusRes==RDESCONECTADO)
            estC = DESCONECTADO;
        else if ((statusRes==RCONECTADO) && (estC==DESCONOCIDO || estC==DESCONECTADO))
            estC = RECIENCONECTADO;
        else if ((statusRes==RCONECTADO) && (estC==CARGANDO_MAX || estC==CARGANDO_POCO))
            estC = CARGADO;
        else if ((statusRes==RPIDECARGA) && (estC==DESCONOCIDO || estC==DESCONECTADO || estC==RECIENCONECTADO))
            estC = CARGANDO_MAX;
        statusCarga = estC;
        switch (statusRes)
        {
            case RDESCONOCIDO:
            case RDESCONECTADO:
                nombre->enviaValPicCN(0);
                break;
            case RCONECTADO:
                nombre->enviaValPicCN(1);
                break;
            case RPIDECARGA:
                nombre->enviaValPicCN(2);
                break;
            case RPIDEVENTILACION:
                nombre->enviaValPicCN(3);
                break;
        }
        enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "est1", (char *) estResStr[statusRes]);
        hayCambios = 1;
    }
    if (!osciladorOculto)
    {
        // chequea onda negativa. Como recien arrancado oscilador no hay valores de JDR1, hace falta comprobarlo
        uint32_t valNeg = ADC1->JDR1;
        if (valNeg>3800)
        {
            ondaNegOk = 1;
            antDsOndaNegOk = 1;
        }
        else
            if (antDsOndaNegOk)
                antDsOndaNegOk = 0;
            else
                ondaNegOk = 0;
    }
    if (ondaNegOk==1 && statusRes!=RDESCONECTADO && statusRes!=RDESCONOCIDO) // && medPSetPoint->diValor()>100.0f)
        palSetLine(LINE_RELE1);
    else
        palClearLine(LINE_RELE1);
    return hayCambios;
}

void cargador::addTime(uint16_t , uint8_t , uint8_t , uint8_t seg, uint8_t ds)
{
    if (ds==2)
    {
        maxP[0] = estimaFases()*225.0f*iMax[0];
        minP[0] = estimaFases()*225.0f*7.0f;
        if ((seg%5) == 0) // envia estado por CAN y a Nextion cada 5s
        {
            enviaEstadoCargador2can();
            enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "cont1", (char *) tipoControlPStr[tipoControlPSelf]);
            enviaTxt(nombres::nomConId(cargadorPtr->idPagecoches), "mast1", (char *) tipoMaestroStr[tipoMaestroSelf]);
            enviaTxt(cargadorPtr->idPagecoches, "fases1", estimaFases());
        }
    }
    if (ds==5)
    {
        enviaPVE();
        enviaIVE();
    }
    // comprueba comunicaciones con otros coches (5 seg. sin comunicaciones)
    for (uint8_t car=1;car<numCoches;car++)
    {
        if (dsSinComunica[car]<50)
            if (++dsSinComunica[car]==50)
                limpiaCoche(car);
    }
    if (seg==0 && ds==7)
        aplicaTipoControl();
    if ((tipoMaestro_t) tipoMaestroSelf == TMMAESTRO)
        controlConjunto(ds);
    controlPropio(seg, ds);
}

void cargador::trataOrdenNextion(char *vars[], uint16_t numPars)
{
    struct msgCanTx_t msgCanTx;
    // Orden desde Nextion: @orden,car,Pvalle,6000
    if (numPars==2 && !strcasecmp(vars[0],"Pvalle"))
    {
        pContratadaValle->set(atoi(vars[1]));
        pContratadaValle->enviaToNextion();
        if (calendar::getPeriodoTarifa()==valle)
            pContratada = pContratadaValle->valor();
        else
            pContratada = pContratadaPunta->valor();
        enviaPContratadas();
    }
    else if (numPars==2 && !strcasecmp(vars[0],"Ppunta"))
    {
        pContratadaPunta->set(atoi(vars[1]));
        pContratadaPunta->enviaToNextion();
        if (calendar::getPeriodoTarifa()==valle)
            pContratada = pContratadaValle->valor();
        else
            pContratada = pContratadaPunta->valor();
        enviaPContratadas();
    }
    //@orden,car,hora1,1,14
    else if (numPars==3 && !strcasecmp(vars[0],"hora1"))
    {
        uint8_t numAjust = atoi(vars[1])-1;
        if (numAjust>=MAXAJUSTES) //error!
            return;
        horaIni[numAjust]->set(atoi(vars[2]));
        horaIni[numAjust]->enviaToNextion();
        aplicaTipoControl();
    }
    //@orden,car,hora2,1,14
    else if (numPars==3 && !strcasecmp(vars[0],"hora2"))
    {
        uint8_t numAjust = atoi(vars[1])-1;
        if (numAjust>=MAXAJUSTES) //error!
            return;
        horaFin[numAjust]->set(atoi(vars[2]));
        horaFin[numAjust]->enviaToNextion();
        aplicaTipoControl();
    }
    //@orden,car,controlP,1,max (fijo,max,noexp)
    else if (numPars==3 && !strcasecmp(vars[0],"controlP"))
    {
        uint8_t numAjust = atoi(vars[1])-1;
        if (numAjust>=MAXAJUSTES) //error!
            return;
        //{CPDESCONOCIDO=0, CPFIJO, CPMAXIMO, CPNOEXPORTA} tipoControlP_t;
        tipoControlP_t nuevoCtrlP = CPDESCONOCIDO;
        if (!strncasecmp(vars[2],"fijo",4))
        {
            nuevoCtrlP = CPFIJO;
            tipoMaestroSelf = TMFIJO;
        }
        else if (!strncasecmp(vars[2],"max",4))
            nuevoCtrlP = CPMAXIMO;
        else if (!strncasecmp(vars[2],"noexp",5))
            nuevoCtrlP = CPNOEXPORTA;
        if (nuevoCtrlP != CPDESCONOCIDO)
        {
            tipoControlPCuando[numAjust]->set(nuevoCtrlP);
            tipoControlPCuando[numAjust]->enviaToNextion();
            aplicaTipoControl();
            enviaEstadoCargador2can();
        }
    }
    else if (numPars==3 && !strcasecmp(vars[0],"Ifija"))
    {
        uint8_t numAjust = atoi(vars[1])-1;
        if (numAjust>=MAXAJUSTES) //error!
            return;
        IMaxSelf[numAjust]->set(atoi(vars[2]));
        IMaxSelf[numAjust]->enviaToNextion();
    }
    else if (numPars==2 && !strcasecmp(vars[0],"mAmin"))
    {
        uint16_t Imin = atoi(vars[1]);
        if (Imin<1000 || Imin>10000) //error!
            return;
        mAmin->set(Imin);
        mAmin->enviaToNextion();
    }
    else if (numPars==2 && !strcasecmp(vars[0],"ID"))
    {
        uint8_t oldID = idChargerSelf->valor();
        idChargerSelf->set(atoi(vars[1]));
        aplicaTipoControl();
        idChargerSelf->enviaToNextion();
        idCharger[0] = idChargerSelf->valor();
        enviaTxt(cargadorPtr->idPagecoches, "ID1", cargadorPtr->idChargerSelf->valor());
        // notifica cambio por CAN. Byte 2: iD estación destino, Byte 3..4: potencia objetivo en W
        msgCanTx.msgEID = oldID;
        msgCanTx.msgDLC = 2;
        msgCanTx.msg[0] = cambiaID;
        msgCanTx.msg[1] = idCharger[0];
        putQueu(&colaMsgTxCan, &msgCanTx);
        chEvtBroadcast(&sendMsgCAN_source);
    }
    else if (numPars==3 && !strcasecmp(vars[0],"punta"))
    {
        uint8_t numAjust = atoi(vars[1])-1;
        if (numAjust>=MAXAJUSTES) //error!
            return;
        statusPermP[numAjust]->set(atoi(vars[2]));
        statusPermP[numAjust]->enviaToNextion();
        aplicaTipoControl();
    }
    else if (numPars==3 && !strcasecmp(vars[0],"llano"))
    {
        uint8_t numAjust = atoi(vars[1])-1;
        if (numAjust>=MAXAJUSTES) //error!
            return;
        statusPermL[numAjust]->set(atoi(vars[2]));
        statusPermL[numAjust]->enviaToNextion();
        aplicaTipoControl();
    }
    else if (numPars==3 && !strcasecmp(vars[0],"valle"))
    {
        uint8_t numAjust = atoi(vars[1])-1;
        if (numAjust>=MAXAJUSTES) //error!
            return;
        statusPermV[numAjust]->set(atoi(vars[2]));
        statusPermV[numAjust]->enviaToNextion();
        aplicaTipoControl();
    }
    //@orden,car,solosol,1,14
    else if (numPars==3 && !strcasecmp(vars[0],"solosol"))
    {
        uint8_t numAjust = atoi(vars[1])-1;
        if (numAjust>=MAXAJUSTES) //error!
            return;
        soloSol[numAjust]->set(atoi(vars[2]));
        soloSol[numAjust]->enviaToNextion();
        aplicaTipoControl();
    }
}

void cargador::print(BaseSequentialStream *tty)
{
    char buffer[80];
    chsnprintf(buffer,sizeof(buffer),"CARGADOR #Coches:%d", numCoches);
    if (tty!=NULL)
        nextion::enviaLog(tty, buffer);
}


