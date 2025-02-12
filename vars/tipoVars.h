/*
 * tipoVars.h
 *
 *  Created on: 1/11/2019
 *      Author: jcabe
 */

#ifndef VARS_TIPOVARS_H_
#define VARS_TIPOVARS_H_


class menuItem {
  public:
    virtual const char *dispMenu() = 0;         // enseña valor en menu
    virtual void clickMenu() = 0;               // click sobre esa
};


class opcionMenu: public menuItem {
  char desc[20];
  void (*ptrClickMenu)();
  public:
    opcionMenu(const char *descTexto, void (*ptrClick)());
    const char *dispMenu();
    void clickMenu();
};

class varStorable {
public:
  static varStorable *listaVarStorable[60];
  static uint8_t numVarStorables;
  varStorable();
  static void leeVariables();
  virtual int16_t escribeFlash() = 0;
  virtual int16_t leeFlash() = 0;
  void (*onChange)(void);
};

class variableSelects: public menuItem, public varStorable {
  protected:
    uint16_t idVar;
    uint8_t valor;
    char desc[15];
  public:
      uint8_t maxValor;
      char buff[20];
      variableSelects(uint16_t idPar, uint8_t valorPar, uint8_t maxValorPar, const char descPar[]);
      const char *dispMenu();
      void clickMenu();
      void setVal(int32_t val);
      uint8_t getValorNum();
      const char *getValorStr();
      char *getDesc();
      void ajusta();
      int16_t escribeFlash();
      int16_t leeFlash();
      virtual const char *getStr(uint8_t num)=0;
};

class varNOSI final: public variableSelects {
  public:
    varNOSI(uint16_t idPar, uint8_t valorPar, const char descPar[]);
    varNOSI(uint16_t idPar, uint8_t valorPar, const char descPar[],void (*onChangePar)(void));
    virtual const char *getStr(uint8_t num);
};

class varNOSIAUTO final: public variableSelects {
  public:
    varNOSIAUTO(uint16_t idPar, uint8_t valorPar, const char descPar[]);
    varNOSIAUTO(uint16_t idPar, uint8_t valorPar, const char descPar[],void (*onChangePar)(void));
    const char *getStr(uint8_t num);
    uint8_t maxValor();
};

class varBAUD: public variableSelects {
  private:
      uint8_t baud2pos(uint32_t baud);
      const uint32_t baudPos[8]={1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
      char buffBaud[10];
  public:
    varBAUD(uint16_t idPar, uint32_t baud, const char descPar[]);
    varBAUD(uint16_t idPar, uint32_t baud, const char descPar[],void (*onChangePar)(void));
    const char *getStr(uint8_t num);
    uint8_t maxValor();
    uint32_t baudVal(void);
};


class varMODOPOZO: public variableSelects {
  public:
    varMODOPOZO(uint16_t idPar, uint8_t valorPar, const char descPar[]);
    varMODOPOZO(uint16_t idPar, uint8_t valorPar, const char descPar[],void (*onChangePar)(void));
    const char *getStr(uint8_t num);
    uint8_t maxValor();
};

class varNUMERO: public menuItem, public varStorable {
  uint16_t idVar;
  int32_t valor;
  int32_t valorMin, valorMax;
  char desc[15];
  char buffNUMERO[20];
  public:
    varNUMERO(uint16_t idPar, int32_t valorPar, int32_t valorMinPar, int32_t valorMaxPar, const char descPar[]);
    varNUMERO(uint16_t idPar, int32_t valorPar, int32_t valorMinPar, int32_t valorMaxPar, const char descPar[],void (*onChangePar)(void));
    const char *dispMenu();
    void clickMenu();
    void setVal(int32_t val);
    const char *getStr(uint8_t num);
    int32_t getValorNum();
    const char *getValorStr();
    char *getDesc();
    void ajusta();
    int16_t escribeFlash();
    int16_t leeFlash();
};

class varSTR50: public menuItem, public varStorable {
  uint16_t idVar;
  char valor[50];
  char buffer[50];
  char desc[15];
  public:
    varSTR50(uint16_t idPar, const char *valor, const char descPar[]);
    varSTR50(uint16_t idPar, const char *valor, const char descPar[],void (*onChangePar)(void));
    const char *dispMenu();
    void clickMenu();
    void setVal(char *val);
    char *getValor20Str();
    char *getValor();
    char *getDesc();
    void ajusta();
    int16_t escribeFlash();
    int16_t leeFlash();
};



#endif /* VARS_TIPOVARS_H_ */
