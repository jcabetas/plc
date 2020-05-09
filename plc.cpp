#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "bloques.h"


extern uint8_t hayCambios;

void divideString(char *buff, uint8_t *numPar, char *par[]);
void divideBloque(char *buff, uint8_t *numPar, char *par[]);

void simula(void)
{
    int8_t numIter, hayCambiosTime;
    uint8_t hora = 8, min = 0, seg = 0, ds = 0;
    hayCambios = 1;
    printf("En simula\n");
    bloque::initBloques();
    estados::printCabecera();
    do
    {
        bloque::actualizaBloques(hora, min, seg, ds);
        hayCambiosTime = 0;
        // calculo hasta que no haya cambios
        hayCambios = 0;
        bloque::addTimeBloques(1, hora, min, seg, ds);
        if (hayCambios)
            hayCambiosTime = 1;
        numIter = bloque::actualizaBloques(hora, min, seg, ds);
        if (hayCambiosTime || hayCambios)
        {
            estados::print(hora, min, seg, ds);
            hayCambios = 0;
        }
        if (++ds >= 10)
        {
            ds = 0;
            if (++seg >= 60)
            {
                seg = 0;
                if (++min >= 60)
                {
                    min = 0;
                    if (++hora > 23)
                    {
                        hora = 0;
                        return;
                    }
                }
            }
        }
    } while (hora < 25);
}


uint8_t leePlc(const char *nomFich)
{
    uint8_t numPar, numParTest, hayError;
    char *par[15], *parTest[15];
    char buffer[120];
    FILE *fich;
    fich = fopen(nomFich, "r");
    if (fich == NULL)
    {
        perror("Error opening file");
        return (-1);
    }
    bloque::vaciaBloques();
    nombres::init();
    estados::init();
    hayError = 0;
    while (fgets(buffer, sizeof(buffer), fich) != NULL)
    {
        if (buffer[0] == '#')
            continue;
        divideString(buffer, &numPar, par);
        if (numPar == 0)
            continue;
        // prueba parametros
        if (!strcmp("TEST", par[0]))
        {
            for (uint8_t i = 0; i < numPar; i++)
            //void divideBloque(char *buff,uint8_t *numPar, char *par[])
            {
                if (par[i][0] != '[')
                    printf("%s\n", par[i]);
                else
                {
                    printf(" bloque {");
                    divideBloque(par[i], &numParTest, parTest);
                    for (uint8_t j = 0; j < numParTest; j++)
                        printf("%s ", parTest[j]);
                    printf("}\n");
                }
            }
        }
        uint16_t numBlqOld = bloque::numero();
        if (numPar >= 4 && !strcmp("AND", par[0]))
            new add(numPar, par, &hayError);
        if (numPar >= 4 && !strcmp("OR", par[0]))
            new OR(numPar, par, &hayError);
        if ((numPar == 4 || numPar == 5) && !strcmp("TIMER", par[0]))
            new timer(numPar, par, &hayError);
        if ((numPar == 4 || numPar == 5) && !strcmp("TIMERNOREDISP", par[0]))
            new timerNoRedisp(numPar, par, &hayError);
        if (numPar == 10 && !strcmp("INPUTTEST", par[0]))
            new inputTest(numPar, par, &hayError);
        if ((numPar == 3 || numPar == 4) && !strcmp("PROGRAMADOR", par[0]))
            new programador(numPar, par, &hayError);
        if (numPar == 5 && !strcmp("ZONA", par[0]))
            new zona(numPar, par, &hayError);
        if (numPar == 7 && !strcmp("START", par[0]))
            new start(numPar, par, &hayError);
        if (numPar == 4 && !strcmp("FLIPFLOP", par[0]))
            new flipflop(numPar, par, &hayError);
        if ((numPar == 4 || numPar == 5) && !strcmp("DELAYON", par[0]))
            new delayon(numPar, par, &hayError);
        if (numPar == 3 && !strcmp("NOT", par[0]))
            new NOT(numPar, par, &hayError);
    }
    fclose(fich);
    printf("Terminado de leer\n");
    bloque::printBloques();
    if (hayError)
    {
        printf("** Abortado por errores\n");
        return 1;
    }
    return 0;
}


/*
    Ejecutar con: valgrind --leak-check=full --show-leak-kinds=all ./bloques
    To do: hacer clases de parámetros flash
*/
int main(void)
{
    leePlc("plc.txt");
    if (!estados::estadosInitOk())
        _exit(1);
    simula();
    bloque::deleteAll();
}
