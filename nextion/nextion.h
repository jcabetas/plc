/*
 * varsNextion.h
 *
 *  Created on: 13 jun. 2020
 *      Author: jcabe
 */

#include "bloques.h"

#ifndef NEXTION_NEXTION_H_
#define NEXTION_NEXTION_H_

#define LONGNAMENEXTION 20
#define MAXVARSNEXTION  40
#define NUMCAMPOSNEXTION 70

#define TIPONEXTIONSILENCIO 0
#define TIPONEXTIONSTR 1
#define TIPONEXTIONNUMERO 2
#define TIPONEXTIONGRAFICO 3
#define TIPONEXTIONCROP 4
#define TIPONEXTIONALL  99

#define NEXTIONSD   SD1

uint8_t hallaNombreyDatosNextion(const char *param, uint8_t tipoNextDefault, uint16_t *idName, uint16_t *idNameWWW, uint16_t *idPage, uint8_t *tipoNextion, uint8_t *picBase);
void divideEnPageyVarNextion(char *param, char **page, char **var);
//void enviaLog(BaseSequentialStream *tty, const char *valor, uint16_t msWait);
void enviaTxt(const char *pageName, const char *varName, const char *valor);
void enviaTxtSiEnPage(uint16_t idPage, const char *varName, const char *valor);
void enviaTxt(uint16_t idPage, uint16_t idVar, const char *valor);
void ponColorEnTexto(uint16_t idPage, const char *varName, uint16_t color);
void enviaTxtSiEnPage(uint16_t idPage, uint16_t idVar, const char *valor);
void enviaTxt(uint16_t idPage, uint16_t idVar, uint16_t valor);
void enviaTxt(uint16_t idPage, const char *varName, uint16_t valor);
void enviaPic(uint16_t idPage, uint16_t idVar, uint16_t valor);
void enviaPicc(uint16_t idPage, uint16_t idVar, uint16_t valor);
void enviaVal(const char *pageName, const char *varName, uint16_t valor);
void enviaVal(uint16_t idPage, uint16_t idVar, uint16_t valor);
void enviaValPic(uint16_t idPage, uint16_t idVar, uint8_t tipoNextion, uint8_t picBase, uint16_t valor);
void trataRxNextion(uint8_t *msgRx, uint8_t numBytes);



class campoNextion {
private:
    static campoNextion *camposNextion[NUMCAMPOSNEXTION];
    static uint16_t numCamposNxt;
    uint16_t idNombreNxt;
    uint16_t idNombreWWW;
    uint16_t idPageNxt;
    uint8_t  tipoNxt;
    uint8_t  picBaseNxt;
public:
    static campoNextion *addCampoNextion(char *nombre, uint16_t tipoNextion, uint8_t *hayError);
    campoNextion(uint16_t idNombre, uint16_t idPage, uint8_t tipoNextion, uint8_t picBaseNxt);
    static void deleteAll(void);
    uint16_t diPageNxt(void);
    uint16_t diNombreNxt(void);
    uint8_t  diTipoNxt(void);
    void enviaValPicCN(uint16_t valor);
    void enviaTxtCN(const char *valor);
};




class nextion {
private:
    static uint16_t idNombreTmpNxt;
    static uint16_t idNombreTmpNxtWWW;
    static uint16_t idPageTmpNxt;
    static uint16_t idNombreLog;
    static uint16_t idNombreLogWWW;
    static uint16_t idPageLog;
    static uint16_t idPageBlank;
    static uint16_t contDsInactividad;
    static uint16_t maxDsInactividad;
    static uint32_t baud;
    static uint16_t idPagActual;
    static void setBlankPage(void);
public:
    static void setNextion(BaseSequentialStream *tty,uint8_t numPar, char *pars[], uint8_t *hayError);
    static void setPagActual(uint16_t idPag);
    static uint16_t diPagActual();
    static void hayActividad(void);
    static void setBasePage(void);
    static void incDs(void);
    static void clearLog(void);
    static void clearLog(campoNextion *campoLog);
    static void enviaLog(BaseSequentialStream *tty, uint16_t idPageLog, uint16_t idNomLog, const char *msg);
    static void enviaLog(BaseSequentialStream *tty, const char *msg);
    static void enviaLog(BaseSequentialStream *tty, campoNextion *campoLog, const char *msg);

};

//class varNextion;
//
//class varNextion {
//    static uint16_t numVarsNextion;
//    static varNextion *varNextionList[MAXVARSNEXTION];
//    char nombVar[LONGNAMENEXTION];
//    uint8_t numPagina;
//public:
//    virtual void enviaToNextion(void) = 0;
//    virtual void recibeDeNextion(void) = 0;
//};
//
//class varNOSI final: public varNextion {
//  private:
//    uint8_t valor;
//  public:
//    varNOSI(char *nombVar, uint8_t valorIni);
//    void enviaToNextion(void);
//    void recibeDeNextion(void);
//};
//
//class varNOSIAUTO final: public varNextion {
//  private:
//    uint8_t valor;
//  public:
//    varNOSIAUTO(char *nombVar, uint8_t valorIni);
//    void enviaToNextion(void);
//    void recibeDeNextion(void);
//};
//
//class varBAUD: public varNextion {
//  private:
//    uint8_t valor;
////    const uint32_t baudPos[8]={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
//  public:
//    varBAUD(char *nombVar, uint32_t valorIni);
//    void enviaToNextion(void);
//    void recibeDeNextion(void);
//};
//
//
//class varMODOPOZO: public varNextion {
//private:
//  uint8_t valor;
//public:
//  varMODOPOZO(char *nombVar, uint8_t valorIni);
//  void enviaToNextion(void);
//  void recibeDeNextion(void);
//};
//
//class varNUMERO: public varNextion {
//  private:
//    int32_t valor;
//  public:
//    varNUMERO(char *nombVar, int32_t valorIni);
//    void enviaToNextion(void);
//    void recibeDeNextion(void);
//};

//class varSTR50: public menuItem, public varStorable {
//  private:
//    char valor[50];
//  public:
//    varSTR50(char *nombVar, char *varIni);
//    void enviaToNextion(void);
//    void recibeDeNextion(void);
//};


#endif /* NEXTION_NEXTION_H_ */
