#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "bloques.h"


bloque *logicHistory[5];
uint16_t numBloques = 0;
programador *programadores[3];


bloque::bloque()
{
	numBloques++;
	logicHistory[numBloques] = NULL;// = (bloque *)this;
}


void divideString(char *buff,uint8_t *numPar, char *par[])
{
    for (uint8_t i=0;i<strlen(buff);i++)
       if (buff[i]=='\n' || buff[i]=='\r')
          buff[i] = 0;
    *numPar = 0;
    char* token = strtok(buff, " "); 
    while (token != NULL) { 
        par[*numPar] = token;
        (*numPar)++;
        token = strtok(NULL, " "); 
    } 
}  

int8_t actualizaBloques(bloque **logicas, uint8_t numBlq)
{
   int8_t hayCambios, numIter;
   numIter = 0;
   do
     {
        hayCambios = 0;
        for (uint8_t i=0;i<numBlq;i++)
            hayCambios |= logicas[i]->calcula();
     } while (hayCambios || ++numIter>10);
   return numIter;
}


void simula(bloque **logicas, uint8_t numBlq, uint16_t dsFin)
{
  int8_t numIter;
  printf("En simula\n");
  for (uint8_t i=0;i<numBlq;i++)
     logicas[i]->init();
  estados::printCabecera();
  for (uint16_t ds=1;ds<=dsFin;ds++)
  {
     actualizaBloques(logicas, numBlq);
     // calculo hasta que no haya cambios
     for (uint8_t i=0;i<numBlq;i++)
       logicas[i]->addTime(100);
     numIter = actualizaBloques(logicas, numBlq);
     //printState(ds,numIter);
     estados::print(ds);
  }
}

int main(void)
{
  estados::init();
  uint8_t numPar;
  char *par[15];
  char buffer [120];
  bloque *logicas[20];
  FILE *fich;
  fich = fopen("plc.txt","r");
  if( fich == NULL) {
      perror("Error opening file");
      return(-1);
   }
  uint8_t numBlq = 0;
  while (fgets(buffer, sizeof(buffer),fich)!=NULL)
  {
     divideString(buffer,&numPar,par);
     if (numPar>=4 && !strcmp("AND",par[0]))
        logicas[numBlq++] = new add(numPar, par);
     if (numPar>=4 && !strcmp("OR",par[0]))
        logicas[numBlq++] = new OR(numPar, par);
     if (numPar==4 && !strcmp("TIMER",par[0]))
        logicas[numBlq++] = new timer(numPar, par);
     if (numPar==4 && !strcmp("INPUTTEST",par[0]))
        logicas[numBlq++] = new inputTest(numPar, par);
  }
  fclose(fich);
  printf("Terminado de leer\n");
  for (uint8_t i=0;i<numBlq;i++)
     logicas[i]->print();
  simula(logicas, numBlq, 20);
}

