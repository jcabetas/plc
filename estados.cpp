#include "bloques.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"


/*
 * estados
 */

estados est;
uint8_t hayCambios;

void estados::init(void)
{
    numEstados = 0;
};


uint8_t &estados::operator [](uint16_t idx)
{
    return estado[idx-1];
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
    estado[numEstados] = 0;
    id = nombres::incorpora(nombre);
    uint16_t numEs = numEstados;
    idNom[numEstados] = id;
    definidoOut[numEstados] = esOut;
    numEstados++;
    return numEstados;
}

// indice 1..numEstados
uint8_t estados::diEstado(uint16_t numEstado)
{
    if (numEstado==0 || numEstado>estados::numEstados)
        return 0;
    return estado[numEstado-1];
}

// indice 1..numEstados
void estados::ponEstado(uint16_t numEstado, uint8_t valor)
{
    if (numEstado>estados::numEstados)
        return;
    if (estado[numEstado-1] !=  valor)
        hayCambios = 1;
    estado[numEstado-1] =  valor;
}

const char *estados::nombre(uint16_t numEstado)
{
    return nombres::nomConId(idNom[numEstado-1]);
};

void estados::printCabecera(void)
{
    printf("          ");
    for (uint16_t est=0;est<estados::numEstados;est++)
    {
        uint16_t id = estados::idNom[est];
        printf(" %10s",estados::nombre(est));
    }
    printf("\n");
}

//         estados::print(ds,hora,min,seg,ds);
void estados::print(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    printf("%2d:%02d %2d.%d",hora,min,seg,ds);
    for (uint16_t est=1;est<estados::numEstados+1;est++)
        printf(" %10d",diEstado(est));
    printf("\n");
}

