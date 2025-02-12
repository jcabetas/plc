#include <ch.h>
#include <pal.h>
#include "serial.h"
#include "rs485.h"
#include "board.h"
#include "lcd.h"
#include "printf.h"

extern uint8_t nombEstacion;
extern uint32_t wordPLCto485, word485toPLC;
extern Mutex MtxRs485;

int checkHex(char *buffer, int posIni, int posFin)
{
	int i;
	char ch;
	for (i=posIni;i<=posFin;i++)
	{
		ch = buffer[i];
		if (ch>='a') ch &= 0x6F;     // lo paso a mayusculas
		if (ch>='A' && ch<='F') continue;
		if (ch>='0' && ch<='9') continue;
		//chprintf("en pos %d, %c no es hex. Despues:%c\r\n",i,chOld,ch);
		return 0;
	}
	return 1;
}

int leeHex(char *buffer, int posIni, int posFin)
{
	int valor = 0;
	int i,valorCh;
	int multiplicador = 1;
	char ch;
	for (i=posFin;i>=posIni;i--)
	{
		ch = buffer[i];
		valorCh = -1;
        if (ch>='0' && ch<='9') valorCh = (int)(ch-'0');
		if (ch>='a') ch &= 0x6F;     // lo paso a mayusculas
		if (ch>='A' && ch<='F') valorCh = (int)(10+ch-'A');
		valor += valorCh*multiplicador;
		multiplicador = multiplicador<<4;
	}
	return valor;
}


void enviaOk(int valor)
{
    char bufferTx[40];
    /* Envio texto tipo :m00E400*   */
    int chkSum;
    chsnprintf(bufferTx, sizeof(bufferTx), ":M%04X", valor);
    chkSum = checkSum(bufferTx,6);
    chsnprintf(bufferTx, sizeof(bufferTx), ":M%06X%02X*",valor,chkSum);
    chprintStrCOM1(bufferTx);
}

void enviaOkLargo(int valor)
{
    char bufferTx[40];
    /* Envio texto tipo :m0000E400*   */
    int chkSum;
    chsnprintf(bufferTx, sizeof(bufferTx), ":m%06X", valor);
    chkSum = checkSum(bufferTx,8);
    chsnprintf(bufferTx, sizeof(bufferTx), ":m%06X%02X*",valor,chkSum);
    chprintStrCOM1(bufferTx);
}

 

/* procesa mensaje 
- Formato de ordenes   
  => 	":" Inicio de orden
	A: Direccion
	Orden:
       S: Set ":AS4521FC*" (poner salidas -2 bytes-)
	    45: Valor de puerto de salida (45)
        21: Valor de 2ndo puerto de salida
		(no tiene salidas => no hace caso)
       I: Información ":AIFA*" (devuelve salidas -2 bytes-)
    FC: Checksum (desde Direccion hasta byte anterior a checksum)
    *: La orden termina en *
- En bufferRx almaceno el :, pero no el *
- posRx apunta a la siguiente posicion libre. En concreto despues del *
*/
void procesaMensaje(char *bufferRx,int posRx)
{
	int chksum=0,chkSumMsg,i, valor0, valor1, valor2;
	/* compruebo checksum */
	for (i=1;i<=posRx-4;i++)
  		chksum += (int) bufferRx[i];
    chksum &= 0xFF;
	if (!checkHex(bufferRx,posRx-3,posRx-2)) 
	{
		//chprintf("Caracteres no hex entre %d y %d\r\n",posRx-3,posRx-2);
		return;
	}
   	chkSumMsg = leeHex(bufferRx,posRx-3,posRx-2);
	if (chksum!=chkSumMsg)
   	{
   		return;
   	}
	if (bufferRx[1]!=nombEstacion) /* comprueba direccion */
   		return;
//	chprintf("%s\r\n",bufferRx);
	if (bufferRx[2]=='S') 	  //:AS4521FC*
   	{
   		if (posRx!=10)
	  		return;
		if (!checkHex(bufferRx,3,6)) return;
		valor0 = leeHex(bufferRx,3,4);
		valor1 = leeHex(bufferRx,5,6);
   		//int valorLeido = leeHex(bufferRx,3,6);

        chMtxLock(&MtxRs485);
   		word485toPLC = valor0 + (valor1<<8);
        chMtxUnlock();
		//chprintf(" tipo S %d\r\n",valorLeido);
		enviaOk(wordPLCto485);
	}
	if (bufferRx[2]=='P') /* Nueva orden: 3 bytes de salidas :AS452187FC*/
   	{
   		if (posRx!=12)
	  		return;
		if (!checkHex(bufferRx,3,8)) return;
//   		int valorLeido = leeHex(bufferRx,3,8);
		valor0 = leeHex(bufferRx,3,4);
		valor1 = leeHex(bufferRx,5,6);
		valor2 = leeHex(bufferRx,7,8);
        chMtxLock(&MtxRs485);
   		word485toPLC = valor0 + (valor1<<8) + (valor2<<16);
        chMtxUnlock();
		//chprintf(" tipo P %d\r\n",valorLeido);
        //ponEnLCD(0,"envioOkLargo");
		enviaOkLargo(wordPLCto485);
        //ponEnLCD(0,"vuelvo");
   	}
	if (bufferRx[2]=='I') // :AIFC*
   	{
   		if (posRx!=6) 
	  		return;
		//chprintf(" tipo I\r\n");
   		enviaOk(wordPLCto485);
   	}
	if (bufferRx[2]=='i') // :AiFC*
   	{
   		if (posRx!=6)
	  		return;
		//chprintf(" tipo i\r\n");
   		enviaOkLargo(wordPLCto485);
   	}
}



