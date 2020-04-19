#ifndef PLC_PARAMETROS_H_
#define PLC_PARAMETROS_H_

#include <stdint.h>

#define MAXPARAMETROS 80

class parametroU16
{
public:
    static parametroU16 *parametrosU16[MAXPARAMETROS];
    static uint16_t numParamsU16;
    virtual uint16_t valor(void) = 0;
    static parametroU16 *addParametro(char *parStr, uint8_t *hayError);
};

class parametroU16Valor : public parametroU16
{
private:
    uint16_t valU16;
public:
    parametroU16Valor(uint16_t valor);
    uint16_t valor(void);
};

class parametroU16Flash : public parametroU16
{
private:
    uint16_t valU16;
public:
    parametroU16Flash(char *nombre, uint16_t valorIni, uint16_t valorMin, uint16_t valorMax);
    uint16_t valor(void);
};

#endif /* PLC_PARAMETROS_H_ */
