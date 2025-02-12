/*
 * tipoVars.c
 *
 *  Created on: 1/11/2019
 *      Author: jcabe
 */

#include "ch.hpp"
#include "hal.h"


using namespace chibios_rt;

#include "tipoVars.h"
#include "../w25q16/varsFlash.h"
#include "manejoVars.h"
#include "string.h"
#include "chprintf.h"
#include "lcd.h"
#include "../w25q16/varsFlash.h"
#include <stdio.h>

extern "C" {
    void leeVariables(void);
}

int16_t leeStringLCD(const char *msg, uint8_t soloNumeros, uint8_t *bufferPar, uint16_t bufferParSize);
void int2str(uint8_t valor, char *string);


opcionMenu::opcionMenu(const char *descTexto, void (*ptrClick)())
{
    strncpy(desc, descTexto, sizeof(desc));
    ptrClickMenu = ptrClick;
};
const char *opcionMenu::dispMenu()
{
    return (const char *) desc;
}
void opcionMenu::clickMenu()
{
    ptrClickMenu();
}

varStorable *varStorable::listaVarStorable[] = {0};
uint8_t varStorable::numVarStorables = 0;

varStorable::varStorable()
{
    if (numVarStorables<sizeof(listaVarStorable))
        listaVarStorable[numVarStorables++] = this;
    onChange = NULL;
}
void varStorable::leeVariables()
{
    for (uint8_t i=0;i<numVarStorables;i++)
    {
        listaVarStorable[i]->leeFlash();
    }
}

variableSelects::variableSelects(uint16_t idPar, uint8_t valorPar, uint8_t maxValorPar, const char descPar[])
{
    idVar = idPar;
    maxValor = maxValorPar;
    setVal(valorPar);
    strncpy(desc,descPar,sizeof(desc));
    desc[sizeof(desc)-1] = 0;
    //    if (leeFlash()) // si hay errores, inicializa flash
    //        escribeFlash();
}
void variableSelects::setVal(int32_t val)
{
    //    if (val>maxValor())
    //        val = maxValor();
    valor = val;
}
const char *variableSelects::dispMenu()
{
    snprintf(buff, sizeof(buff), "%s:%s", desc,getStr(valor));
    return (const char *) buff;
}
void variableSelects::clickMenu()
{
    gestionaMenuSelsCPP((variableSelects *)this);
    escribeFlash();
    if (onChange!=NULL)
        onChange();
}
uint8_t variableSelects::getValorNum()
{
    return valor;
}
//const char *variableSelects::getValorStr()
//{
//    return getStr(valor);
//}
char *variableSelects::getDesc()
{
    return desc;
}
int16_t variableSelects::escribeFlash()
{
    if (idVar>0)
        return escribeVarU8(&idVar, desc, valor);
    else
        return 0;
}

int16_t variableSelects::leeFlash()
{
    //    int16_t leeVariableU8(uint16_t posParam, const char *nombParam, uint8_t valorMin, uint8_t valorMax, uint8_t *valor);

    if (idVar>0)
        return leeVariableU8((uint16_t *)&idVar, (const char *)desc,(uint8_t) 0, (uint8_t)maxValor, 0, (uint8_t *)&valor);
    else
        return 0;
}


varNOSI::varNOSI(uint16_t idPar, uint8_t valorPar, const char descPar[]):
                variableSelects(idPar,valorPar,1,descPar)
{

}
varNOSI::varNOSI(uint16_t idPar, uint8_t valorPar, const char descPar[],void (*onChangePar)(void)):
                variableSelects(idPar,valorPar,1,descPar)
{
    onChange = onChangePar;
}
const char *varNOSI::getStr(uint8_t num)
{
    switch (num)
    {
    case 0:
        return "No";
        break;
    case 1:
        return "Si";
        break;
    default:
        return "??";
    }
}

varNOSIAUTO::varNOSIAUTO(uint16_t idPar, uint8_t valorPar, const char descPar[]):
              variableSelects(idPar,valorPar,2,descPar)
{

}
varNOSIAUTO::varNOSIAUTO(uint16_t idPar, uint8_t valorPar, const char descPar[],void (*onChangePar)(void)):
              variableSelects(idPar,valorPar,2,descPar)
{
    onChange = onChangePar;
}


const char *varNOSIAUTO::getStr(uint8_t num)
{
    switch (num)
    {
    case 0:
        return "No";
        break;
    case 1:
        return "Si";
        break;
    case 2:
        return "Auto";
        break;
    default:
        return "??";
    }
}


varBAUD::varBAUD(uint16_t idPar,uint32_t baud, const char descPar[]):
                variableSelects(idPar,baud2pos(baud),7,descPar)
{

}
varBAUD::varBAUD(uint16_t idPar,uint32_t baud, const char descPar[],void (*onChangePar)(void)):
                variableSelects(idPar,baud2pos(baud),7,descPar)
{
    onChange = onChangePar;
}
uint8_t varBAUD::baud2pos(uint32_t baud)
{
    for (uint8_t b=0;b<8;b++)
        if (baudPos[b]==baud)
            return b;
    // no encontrado, poner 9600
    return 3;
}
uint32_t varBAUD::baudVal(void)
{
    return baudPos[valor];
}
const char *varBAUD::getStr(uint8_t num)
{
    snprintf(buffBaud, sizeof(buffBaud), "%d", (int) baudPos[num]);
    return (const char *) buffBaud;
}


varMODOPOZO::varMODOPOZO(uint16_t idPar, uint8_t valorPar, const char descPar[]):
            variableSelects(idPar,valorPar,2,descPar)
{
}
varMODOPOZO::varMODOPOZO(uint16_t idPar, uint8_t valorPar, const char descPar[], void (*onChangePar)(void)):
            variableSelects(idPar,valorPar,2,descPar)
{
    onChange = onChangePar;
}
const char *varMODOPOZO::getStr(uint8_t num)
{
    switch (num)
    {
    case 0:
        return "Registrador";
        break;
    case 1:
        return "Llamacion";
        break;
    case 2:
        return "Pozo";
        break;
    default:
        return "??";
    }
}



varNUMERO::varNUMERO(uint16_t idPar, int32_t valorPar, int32_t valorMinPar, int32_t valorMaxPar, const char descPar[])
{
    idVar = idPar;
    valorMin = valorMinPar;
    valorMax = valorMaxPar;
    setVal(valorPar);
    strncpy(desc,descPar,sizeof(desc));
    desc[sizeof(desc)-1] = 0;
}
varNUMERO::varNUMERO(uint16_t idPar, int32_t valorPar, int32_t valorMinPar, int32_t valorMaxPar, const char descPar[], void (*onChangePar)(void))
{
    idVar = idPar;
    valorMin = valorMinPar;
    valorMax = valorMaxPar;
    setVal(valorPar);
    strncpy(desc,descPar,sizeof(desc));
    desc[sizeof(desc)-1] = 0;
    onChange = onChangePar;
}
void varNUMERO::setVal(int32_t val)
{
    if (val>valorMax)
        val = valorMax;
    if (val<valorMin)
        val = valorMin;
    valor = val;
}
const char *varNUMERO::dispMenu()
{
    snprintf(buffNUMERO, sizeof(buffNUMERO), "%s:%d", desc,(int) valor);
    return (const char *) buffNUMERO;
}
void varNUMERO::clickMenu()
{
    valor = leeNum(valor, valorMin, valorMax, (const char *) desc);
    escribeFlash();
}
//const char *varNUMERO::getStr(uint8_t num)
//{
//  snprintf(buffNUMERO, sizeof(buffNUMERO), "%d", (int) valor);
//  return (const char *) buffNUMERO;
//}
int32_t getValorVarNumero(varNUMERO *variable)
{
    return variable->getValorNum();
}

int32_t varNUMERO::getValorNum()
{
    return valor;
}
//const char *varNUMERO::getValorStr()
//{
//    return getStr(0);
//}
char *varNUMERO::getDesc()
{
    return desc;
}
int16_t varNUMERO::escribeFlash()
{
    if (idVar>0)
        return escribeVar32(&idVar, desc, valor);
    else
        return 0;
}
int16_t varNUMERO::leeFlash()
{
    if (idVar>0)
        return leeVariable32(&idVar, desc, valorMin, valorMax, valorMin, &valor);
    else
        return 0;
}

varSTR50::varSTR50(uint16_t idPar, const char *valorIni, const char descPar[])
{
    idVar = idPar;
    strncpy(valor,valorIni,sizeof(valor));
    strncpy(desc,descPar,sizeof(desc));
    desc[sizeof(desc)-1] = 0;
}
varSTR50::varSTR50(uint16_t idPar, const char *valorIni, const char descPar[], void (*onChangePar)(void))
{
    idVar = idPar;
    strncpy(valor,valorIni,sizeof(valor));
    strncpy(desc,descPar,sizeof(desc));
    desc[sizeof(desc)-1] = 0;
    onChange = onChangePar;
}
const char *varSTR50::dispMenu()
{
    snprintf(buffer, sizeof(buffer), "%s:%s", desc,valor);
    buffer[17] = 0;
    return (const char *) buffer;
}
void varSTR50::clickMenu()
{
    int16_t result = leeStringLCD((const char *)desc, 0, (uint8_t *) valor, sizeof(valor));
    if (result>=0)
        escribeFlash();
}
void varSTR50::setVal(char *val)
{
    strncpy(valor,val,sizeof(valor));
}
char *varSTR50::getValor()
{
    return (char *) valor;
}
char *varSTR50::getValor20Str()
{
    snprintf(buffer, sizeof(buffer), "%s",valor);
    buffer[20] = 0;
    return (char *) buffer;
}
char *varSTR50::getDesc()
{
    return desc;
}
int16_t varSTR50::escribeFlash()
{
    return escribeStr50(&idVar, desc, valor);
}
int16_t varSTR50::leeFlash()
{
    return leeStr50(&idVar, desc, "", valor);
}
/*
  public menuItem, public varStorable {
  uint16_t idVar;
  char valor[50];
  char desc[15];
  public:
    varSTR50(uint16_t idPar, char *valor, const char descPar[]);
    const char *dispMenu();
    void clickMenu();
    void setVal(char *val);
    const char *getValorStr();
    char *getDesc();
    void ajusta();
    int16_t escribeFlash();
    int16_t leeFlash();
};
 */


void leeVariables(void)
{
    varStorable::leeVariables();
}


