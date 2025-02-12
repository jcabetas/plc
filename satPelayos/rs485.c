#include "ch.h"
#include "hal.h"
#include "rs485.h"
#include "lcd.h"
#include "printf.h"
#include "plc.h"
#include "flash.h"
#include "string.h"
#include "sms.h"

int outbyte(int c);
uint8_t chgetch(void);
uint8_t *chgets(void);

extern Mutex MtxRs485;
extern int valorPuerta;
extern int tamanoPLC, offsetSelecc, offsetEstaciones;

satelite satelites[MAXSATELITES];
int numSatelites = 0;
uint32_t wordPLCto485, word485toPLC;


void anadeSatelite(char nombre)
{
    int i;
    // compruebo que no existe
    for (i=0;i<numSatelites;i++)
        if (satelites[i].nombre == nombre)
		{
			satelites[i].debeRastrearse = 1;
			return;
		}
    // no esta, lo pongo al final
    satelites[numSatelites].nombre = nombre;
    satelites[numSatelites].debeRastrearse = 1;
    satelites[numSatelites].status = 0;
    satelites[numSatelites].numErrores = 0;
    satelites[numSatelites].valoresSalida = 0;
    satelites[numSatelites].valoresEntrada = 0;
    satelites[numSatelites].tipoSatelite = 0;
    numSatelites++;
}


int init485DesdePlc(void)
{
    u16 *direccion;
    char estacion;
    wordPLCto485 = 0;
    word485toPLC = 0;
    leeDatosBasicosPLC();
    if (tamanoPLC == 0xFFFF)
    {
    	ponMsgEnLCD("Flash virgen\r\n");
        return -1;
    }
    direccion = (u16 *) StartAddrPlc;
    direccion += 1 + offsetEstaciones;
    // inicializo satelites
	numSatelites = 0;
    chprintf("Las estaciones485 que deben existir: ");
    while (1==1)
    {
        estacion = (char) leeHalfWord(&direccion);
        if (estacion==0) 
        {
            chprintf("\r\n");
            return 0;
        }
        chprintf("%c",estacion);
        anadeSatelite(estacion);
    }
}


void ponStatusSMSAll(void)
{
    int i;
    char buffSt[15];
    if (numSatelites==0)
    {
        enviaTexto("No hay satelites;");
        return;
    }
    for (i=0;i<numSatelites;i++)
    {
        chsnprintf(buffSt,sizeof(buffSt),"Stat %c:%d",satelites[i].nombre,satelites[i].status);
        enviaTexto(buffSt);
    }
}

void estadoSatelites(void)
{
	int i;
    chprintf("Estado de satelites:\r\n");
	for (i=0;i<numSatelites;i++)
    {
    	chprintf("%c: Estado:%d tipo:%d Input:%d Out:%d\r\n", satelites[i].nombre, satelites[i].status, satelites[i].tipoSatelite
    			,  satelites[i].valoresEntrada,  satelites[i].valoresSalida);
    }
}

void ponStatusSMSResumido(void)
{
    int i, numMal;
    char buffSt[40];
    if (numSatelites==0)
        return;
    numMal = 0;
    for (i=0;i<numSatelites;i++)
    {
        if (satelites[i].debeRastrearse==1 && satelites[i].status==0)
        {
            numMal++;
            chsnprintf(buffSt,sizeof(buffSt),"Stat %c:%d",satelites[i].nombre,satelites[i].status);
            enviaTexto(buffSt);
        }
    }
    if (numMal==0)
        enviaTexto("Comm Ok");
}


uint8_t tipoSatelite(int posSatelite, int longAnsw,char *buff)
{
    //  :SP0000C3*  elemental
    satelites[posSatelite].tipoSatelite = 0;
    if (longAnsw==7) return 1; // :M0800*
    if (longAnsw==9 && (buff[6]!='0' || buff[7]!='0')) return 2;
    //  :SP00000000*  largo sin ChkSum 
    if (longAnsw==11 && buff[8]=='0' && buff[9]=='0') return 3;
    //  :SP000000C3*  largo con ChkSum 
    if (longAnsw==11 && (buff[8]!='0' || buff[9]!='0')) return 4;
    return 0;
}

void ponTipoSatelite(int posSatelite, int longAnsw,char *buff)
{
    char buffSt[40];
    satelites[posSatelite].tipoSatelite = tipoSatelite(posSatelite, longAnsw, buff);
    if (satelites[posSatelite].tipoSatelite == 0)
    {
    	chsnprintf(buffSt,sizeof(buffSt),"%c no reconocido",satelites[posSatelite].nombre);
    	chprintf("Respuesta de %c no reconocida: '%s'",satelites[posSatelite].nombre, buff);
    	ponMsgEnLCD(buffSt);
    	chThdSleepMilliseconds(2000);
    }
    //else
    //	chprintf("%c tipo %d\r\n",satelites[posSatelite].nombre,satelites[posSatelite].tipoSatelite);
}


uint32_t leeRespuestaAOk(int numSatelite,char *bufferRx, int *error)
{
    uint32_t result = 0;
    int posIni, posFin, valor0, valor1, valor2;
    if (satelites[numSatelite].tipoSatelite<=0 || satelites[numSatelite].tipoSatelite>=5)
    {
        *error = -1;
        return 0;
    }
    result = 0;
    switch (satelites[numSatelite].tipoSatelite)
    {
      case 1:   posIni = 2;
                posFin = 5;
            	valor0 = leeHex(bufferRx,2,3);// :M0000C3*
            	valor1 = leeHex(bufferRx,4,5);
            	result = (valor1<<8) + valor0;
                break;
      case 2:
      case 3:
      case 4:   posIni = 2;  // :m000000XX*
                posFin = 7;
            	valor0 = leeHex(bufferRx,2,3);
            	valor1 = leeHex(bufferRx,4,5);
            	valor2 = leeHex(bufferRx,6,7);
            	result = (valor2<<16) + (valor1<<8) + valor0;
            	if (satelites[numSatelite].nombre=='A' && result>0)
            		posIni = 4;// debe ser 2
                break;
    }
    if (!checkHex(bufferRx,posIni,posFin)) 
    {
        *error = 0;//-2; vamos aolvidarnos del checksum or el momento
        return result;//0
    }

    *error = 0;
    return result;
}



int leeInput485(int numSatelite, int numPin)
{
    long valor;
    if (numSatelite>numSatelites) 
        return -1;
    chMtxLock(&MtxRs485);
    valor = satelites[numSatelite].valoresEntrada;
    chMtxUnlock();
    return ((valor>>numPin) & 1);
}

void asignaOutput485(int numSatelite, int numPin, int valorBool)
{
    long valor;
    if (numSatelite>numSatelites) 
        return;
    chMtxLock(&MtxRs485);
    valor = satelites[numSatelite].valoresSalida;
    if (valorBool == 0)
        valor &= ~(1<<numPin);
    else
        valor |= (1<<numPin);
    satelites[numSatelite].valoresSalida = valor;
    chMtxUnlock();
}

int choutbyteCOM1(int c)
{
	//uartStartSend(&UARTD1, 1, (char) c);
	sdPut(&SD1,(uint8_t) c);
    return c;
}


void chprintStrCOM1(char *str)
{
	wordPLCto485 = TXRX1_PIN;  //???¿¿¿¿????
    palSetPad(TXRX1_PORT, TXRX1_PIN);  /* Activo TX1 1 ms antes de emitir */
    chThdSleepMilliseconds(1);
    while (*str) {
        (void)choutbyteCOM1(*str);
        str++;
    }
}


int checkSum(char *buffer,int buffSize)
{
    int chkSum = 0;
    int i = 1;
    while (buffer[i]!='*' && buffer[i]!=0 && i<buffSize)
    {
        chkSum += (int) buffer[i];
        i++;
    }
    chkSum &= 0xFF;
    return chkSum;
}


void ponMensajeLargo(char *buffer, int buffSize, char estacion, int valor)
{
    /* Envio texto tipo :SP000000C3*   */
    int chkSum;
    uint8_t valor0, valor1, valor2;
    valor2 = (valor>>16) & 0xFF;
    valor1 = (valor>>8) & 0xFF;
    valor0 = valor & 0xFF;
    chsnprintf(buffer, buffSize, ":%cP%02X%02X%02X", estacion,valor0,valor1, valor2);
    chkSum = checkSum(buffer,buffSize);
    chsnprintf(buffer, buffSize, ":%cP%02X%02X%02X%02X*",estacion, valor0,valor1, valor2,chkSum);
}

void ponMensaje(char *buffer, int buffSize, char estacion, int valor)
{
    /* Envio texto tipo :SP0000C3*   */
    int chkSum;
    uint8_t valor0, valor1;
    valor1 = (valor>>8) & 0xFF;
    valor0 = valor & 0xFF;
    chsnprintf(buffer, buffSize, ":%cS%02X%02X", estacion,valor0,valor1);
    chkSum = checkSum(buffer,buffSize);
    chsnprintf(buffer, buffSize, ":%cS%02X%02X%02X*",estacion, valor0, valor1,chkSum);
}




int leeRespuesta(char *buffer, int bufferSize)
{
    int posBuff=0;
    do 
    {
        msg_t chInp = sdGetTimeout(&SD1,100);
        if (chInp==Q_TIMEOUT)
        {
            buffer[0] = 0;
            return -1;
        }
        char ch = (char) chInp;
        buffer[posBuff] = ch;
        // si hay demasiados caracteres devuelvo error
        if (++posBuff > bufferSize)
        {
            buffer[0] = 0;
            return -2;
        }
        if (ch=='*') 
        {
            return posBuff;
            break;
        }
    } while (1==1);
}



