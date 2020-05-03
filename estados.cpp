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

uint8_t &estados::operator[](uint16_t numEstado)
{
    if (numEstado == 0 || numEstado > estados::numEstados)
        printf("Numero estado ilegal en []\n");
    return estado[numEstado - 1];
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
uint16_t estados::idNom[MAXSTATES] = {0};           // el id del nombre para un estado dado
uint8_t estados::definidoOut[MAXSTATES] = {0};
uint8_t estados::idNom2idEstado[MAXSTATES] = {0};   // el id del estado para un id del nombre (buscar por idNombre)

/*
*  para poner un nuevo estado
*  si existe, verificar que no hay dos que sean output 
*/
uint8_t estados::addEstado(char *nombre, uint8_t esOut, uint8_t *hayError)
{
    uint16_t idNombre;
    idNombre = nombres::busca(nombre);
    if (idNombre > 0)
    {
        // ahora busca el estado que tenga el nombre con ese id
        uint16_t idEstado = idNom2idEstado[idNombre];
        if (esOut)
        {
            if (definidoOut[idEstado-1])
            {
                printf("Error: salida %s (estado:%d) se define varias veces\n", nombre,idEstado);
                *hayError = 1;
                return 0;
            }
            definidoOut[idEstado-1] = 1;
        }
        return idEstado;
    }
    estado[numEstados] = 0;
    idNombre = nombres::incorpora(nombre);
    idNom[numEstados] = idNombre;
    idNom2idEstado[idNombre] = numEstados+1;
    definidoOut[numEstados] = esOut;
    numEstados++;
    return numEstados;
}

// indice 1..numEstados
uint8_t estados::diEstado(uint16_t numEstado)
{
    if (numEstado == 0 || numEstado > estados::numEstados)
    {
        printf("Numero de estado ilegal %d en diEstado\n",numEstado);
        return 0;
    }
    return estado[numEstado - 1];
}

// indice 1..numEstados
void estados::ponEstado(uint16_t numEstado, uint8_t valor)
{
    if (numEstado == 0 || numEstado > estados::numEstados)
    {
        printf("Numero estado ilegal %d en ponEstado\n",numEstado);
        return;
    }
    if (estado[numEstado - 1] != valor)
        hayCambios = 1;
    estado[numEstado - 1] = valor;
}

uint8_t estados::estadosInitOk(void)
{
    uint8_t esOk = 1;
    for (uint16_t est = 1; est <= estados::numEstados; est++)
    {
        if (!estados::definidoOut[est-1])
        {
            printf("Error: %s (estado #%d) nadie lo define\n", estados::nombre(est),est);
            esOk = 0;
        }
    }
    return esOk;
}

const char *estados::nombre(uint16_t numEstado)
{
    if (numEstado == 0 || numEstado > estados::numEstados)
    {
        printf("Numero estado %d ilegal en nombre\n",numEstado);
        return "Estado ilegal";
    }
    return nombres::nomConId(idNom[numEstado - 1]);
};

void estados::printCabecera(void)
{
    printf("          ");
    for (uint16_t est = 1; est <= estados::numEstados; est++)
    {
        uint16_t id = estados::idNom[est];
        printf(" %10s", estados::nombre(est));
    }
    printf("\n");
}

//         estados::print(ds,hora,min,seg,ds);
void estados::print(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    printf("%2d:%02d %2d.%d", hora, min, seg, ds);
    for (uint16_t est = 1; est <= estados::numEstados; est++)
        printf(" %10d", diEstado(est));
    printf("\n");
}
