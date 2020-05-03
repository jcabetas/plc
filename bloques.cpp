#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "bloques.h"


extern uint8_t hayCambios;

bloque *bloque::logicHistory[MAXBLOQUES];
uint16_t bloque::numBloques = 0;
bloque::bloque()
{
    logicHistory[numBloques] = this;
    numBloques++;
}

bloque::~bloque() 
{ 
    printf("Borrando bloque\n"); 
} 

void bloque::deleteAll(void)
{
    for (int16_t blq=0;blq<bloque::numBloques;blq++)
        delete logicHistory[blq];
    bloque::numBloques = 0;
}


int8_t bloque::initBloques(void)
{
    for (int16_t blq=0;blq<bloque::numBloques;blq++)
        logicHistory[blq]->init();
    return 0;
}

uint16_t bloque::numero()
{ 
    return bloque::numBloques;
}

void bloque::addTimeBloques(uint16_t dsInc, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    for (int16_t blq=0;blq<bloque::numBloques;blq++)
        logicHistory[blq]->addTime(dsInc, hora, min, seg, ds);
}

void bloque::printBloques()
{
    for (int16_t blq=0;blq<bloque::numBloques;blq++)
        logicHistory[blq]->print();
}

int8_t bloque::actualizaBloques(uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds)
{
    int8_t numIter, hayCambiosEnCalculo;
    numIter = 0;
    hayCambiosEnCalculo = 0;
    do
    {
        hayCambios = 0;
        for (uint8_t i = 0; i < bloque::numBloques; i++)
            logicHistory[i]->calcula(hora, min, seg, ds);
        if (hayCambios)
            hayCambiosEnCalculo = 1;
    } while (hayCambios || ++numIter < 10);
    hayCambios = hayCambiosEnCalculo;
    return numIter;
}
