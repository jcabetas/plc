#ifndef PLC_PARAMETROS_H_
#define PLC_PARAMETROS_H_

#include <stdint.h>

#define MAXPARAMETROS 80
class parametroU16;


/*

parametro
   |_______ 

*/

// esta clase es para llevar la cuenta
// de objetos parametros, y luego borrarlos
class parametro
{
private:
    static uint16_t numParams;
    static parametro *params[MAXPARAMETROS];
public:
    parametro();
    static parametroU16 *addParametroU16(char *parStr, uint8_t *hayError);
    static void deleteAll(void);
    static void printAll(void);
    virtual ~parametro() = 0;
    virtual void print() = 0;
};

class parametroFlash
{
private:
    static uint16_t numParamsFlash;
    static parametroFlash *paramsFlash[MAXPARAMETROS];
public:
    parametroFlash();
    virtual void leeDeFlash() = 0;
};

class parametroU16
{
public:
    virtual uint16_t valor(void) = 0;
};

class parametroU16Valor : public parametro, public parametroU16
{
private:
    uint16_t valU16;
public:
    ~parametroU16Valor();
    parametroU16Valor(uint16_t valor);
    uint16_t valor(void);
    void print();
};

class parametroU16Flash : public parametro, public parametroFlash, public parametroU16
{
private:
    uint16_t valU16;
    uint16_t valMin;
    uint16_t valMax;
    uint16_t valIni;
    uint16_t idNombre;
public:
    ~parametroU16Flash();
    parametroU16Flash(char *nombre, uint16_t valorIni, uint16_t valorMin, uint16_t valorMax);
    void leeDeFlash();
    uint16_t valor(void);
    void print();
};

#endif /* PLC_PARAMETROS_H_ */
