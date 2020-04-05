#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "bloques.h"


bloque *logicHistory[20];
uint16_t numBloques = 0;
extern uint8_t hayCambios;


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
   int8_t numIter, hayCambiosEnCalculo;
   numIter = 0;
   hayCambiosEnCalculo = 0;
   do
     {
        hayCambios = 0;
        for (uint8_t i=0;i<numBlq;i++)
            logicas[i]->calcula();
        if (hayCambios)
            hayCambiosEnCalculo = 1;
     } while (hayCambios || ++numIter<10);
   hayCambios = hayCambiosEnCalculo;
   return numIter;
}


void simula(bloque **logicas, uint8_t numBlq)
{
  int8_t numIter, hayCambiosTime;
  uint8_t hora=8, min=0, seg=0, ds=0;
  hayCambios = 1;
  printf("En simula\n");
  for (uint8_t i=0;i<numBlq;i++)
     logicas[i]->init();
  estados::printCabecera();
  do
  {
     actualizaBloques(logicas, numBlq);
     hayCambiosTime = 0;
     // calculo hasta que no haya cambios
     hayCambios =0;
     for (uint8_t i=0;i<numBlq;i++)
         //ddTime(uint16_t ms, uint8_t hora, uint8_t min, uint8_t seg, uint8_t ds);
       logicas[i]->addTime(1, hora, min, seg, ds);
     if (hayCambios)
         hayCambiosTime = 1;
     numIter = actualizaBloques(logicas, numBlq);
     if (hayCambiosTime || hayCambios)
     {
         estados::print(hora,min,seg,ds);
         hayCambios = 0;
     }
     if (++ds>=10)
     {
         ds = 0;
         if (++seg>=60)
         {
             seg = 0;
             if (++min>=60)
             {
                 min = 0;
                 if (++hora>23)
                 {
                     hora = 0;
                     return;
                 }
             }
         }
     }
  } while (hora<9);
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
      if (buffer[0]=='#')
          continue;
     divideString(buffer,&numPar,par);
     if (numPar==0)
         continue;
     if (numPar>=4 && !strcmp("AND",par[0]))
        logicas[numBlq++] = new add(numPar, par);
     if (numPar>=4 && !strcmp("OR",par[0]))
        logicas[numBlq++] = new OR(numPar, par);
     if (numPar==4 && !strcmp("TIMER",par[0]))
        logicas[numBlq++] = new timer(numPar, par);
     if (numPar==10 && !strcmp("INPUTTEST",par[0]))
        logicas[numBlq++] = new inputTest(numPar, par);
     if ((numPar==3 || numPar==4) && !strcmp("PROGRAMADOR",par[0]))
        logicas[numBlq++] = new programador(numPar, par);
     if (numPar==5 && !strcmp("ZONA",par[0]))
        logicas[numBlq++] = new zona(numPar, par);
     if (numPar==7 && !strcmp("START",par[0]))
        logicas[numBlq++] = new start(numPar, par);
  }
  fclose(fich);
  printf("Terminado de leer\n");
  for (uint8_t i=0;i<numBlq;i++)
     logicas[i]->print();
  simula(logicas, numBlq);
}

