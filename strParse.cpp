#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"



/*
 * Divide string por espacios
 * Lo que haya entre [] se considera un solo grupo
 */
void divideString(char *buff, uint8_t *numPar, char *par[])
{
    //  sustituyo CR o LF por nulos
    for (uint8_t i = 0; i < strlen(buff); i++)
        if (buff[i] == '\n' || buff[i] == '\r')
            buff[i] = 0;
    *numPar = 0;
    while (*buff != 0)
    {
        char car = *buff;
        if (car == ' ')
        {
            *(buff++) = 0; // eliminamos espacios en blanco
            continue;
        }
        if (car == '[')
        {
            par[*numPar] = buff++;
            (*numPar)++;
            while (*buff)
            {
                if (*buff == ']') // fin de bloque, ok
                {
                    *buff++;
                    break;
                }
                if (*buff == 0) // final sin fin de bloque, mal
                {
                    printf("Error: '[' sin cierre\n");
                    *numPar = 0;
                    return;
                }
                buff++;
            }
            continue;
        }
        if (car == ',')
        {
            *(buff++) = 0;
            while (*buff == ' ') // eliminamos espacios en blanco
                *(buff++) = 0;
            continue;
        }
        // estamos en un parametro
        par[*numPar] = buff++;
        (*numPar)++;
        while (*buff != ' ' && *buff != ',' && *buff != '[' && *buff != 0) // recorremos parametros
        {
            *(buff++);
        }
    }
}

/*
 * Identifica parametros en bloques [a, b, c]
 */
void divideBloque(char *buff, uint8_t *numPar, char *par[])
{
    if (buff[0] != '[')
    {
        printf("Error, bloque no empieza por '['\n");
        *numPar = 0;
        return;
    }
    *(buff++);
    //  sustituyo CR o LF por nulos
    for (uint8_t i = 0; i < strlen(buff); i++)
        if (buff[i] == '\n' || buff[i] == '\r')
            buff[i] = 0;
    *numPar = 0;
    char *token = strtok(buff, ", ]");
    while (token != NULL)
    {
        par[*numPar] = token;
        (*numPar)++;
        token = strtok(NULL, ", ]");
    }
}