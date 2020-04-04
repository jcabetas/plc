#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"


/*
 * estados
 */
void estados::init(void)
{
    numEstados = 0;
};


uint8_t &estados::operator [](uint16_t idx)
{
    return estado[idx];
}
/*
class estados {
  private:
    uint16_t numEstados;
  public:
    void init(void);
    uint8_t  addEstado(char *nombre, uint8_t esOut);
    uint8_t  estado[MAXSTATES];
    uint16_t idNom[MAXSTATES];
    uint8_t  definidoOut[MAXSTATES];
};
*/

uint16_t estados::numEstados = 0;
uint8_t estados::estado[MAXSTATES] = {0};
uint16_t estados::idNom[MAXSTATES] = {0};
uint8_t estados::definidoOut[MAXSTATES] = {0};

/*
*  para poner un nuevo estado
*  si existe, verificar que no hay dos que sean output 
*/
uint8_t estados::addEstado(char *nombre, uint8_t esOut)
{
    uint16_t id;
    id = nombres::busca(nombre);
    if (id > 0)
    {
       if (esOut)
       {
          printf("Error: salida %s se define varias veces\n",nombre);
          return 0;
       }
       return id;
    }
    numEstados++;
    estado[numEstados] = 0;
    idNom[numEstados] = nombres::incorpora(nombre);
    definidoOut[numEstados] = esOut;
    return numEstados;
}


uint8_t estados::diEstado(uint16_t numEstado)
{
    if (numEstado==0 || numEstado>=MAXSTATES)
        return 0;
    return estado[numEstado]; 
}


void estados::ponEstado(uint16_t numEstado, uint8_t valor)
{
    if (numEstado==0 || numEstado>=MAXSTATES)
        return;
    estado[numEstado] =  valor;
}

void printCabecera(void)
{
    
}

void estados::print(uint16_t ds)
{
    for (uint16_t est=1;est<numEstados;est++)
        printf("%d (%s)\n",est,nombres::nomConId(idNom[est]));
}

