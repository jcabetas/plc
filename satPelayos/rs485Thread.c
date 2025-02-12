#include <ch.h>
#include "hal.h"
#include <pal.h>
#include "serial.h"
#include "rs485.h"
#include "board.h"
#include "lcd.h"
#include "printf.h"

//int outbyte(int c);
//uint8_t chgetch(void);
//uint8_t *chgets(void);

extern Mutex MtxRs485;
//extern int valorPuerta;
extern uint8_t nombEstacion;
extern satelite satelites[MAXSATELITES];
extern int numSatelites;

char bufferTx[40],bufferRx[40];
char buffer[40];

extern Thread *procesoGestorThreads;
extern Thread *procesoPLC;
extern Thread *proceso485;
extern uint8_t estadoRs485; // 0: parado, 1: activo, 2: activo con problemas

char bufferRx[40];
char buffSats[40];


void exploraSatelites(void)
{
	char Est;
	char SatsFounded[20];
	const char dirs[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    int result,estabaArrancado,numTry, numSatsExisted;
    uint8_t s;
    if (procesoPLC==NULL) estabaArrancado =0; else estabaArrancado=1;
	chMsgSend(procesoGestorThreads,(msg_t) 2); // para PLC
	while (1==1)
	{
		if (procesoPLC==NULL) break;
		chThdSleepMilliseconds(20); // espero a que se pare
	}
    ponEnLCD(0,"Buscando satelites");
    ponEnLCD(1,"");
    ponEnLCD(2,"");
    ponEnLCD(3,"");
    SatsFounded[0] = 0;
    numSatsExisted = 0;

    for (s=0; s<sizeof(dirs);s++)
    {
    	Est = dirs[s];
        chsnprintf(buffSats,sizeof(buffSats),"Probando '%c'",Est);
        ponEnLCD(1,buffSats);
        /*
        ponMensajeLargo(bufferTx, sizeof(bufferTx),Est,0);
        for (numTry=0;numTry<=2;numTry++)
        {
            chprintStrCOM1(bufferTx);
            result = leeRespuesta(bufferRx,sizeof(bufferRx));
            if (result>=0) break;
        }
        //ponEnLCD(1,"");
        if (result>=0)
        {
            //chprintf("%c ok",Est);
        	if (numSatsExisted<20)
        	{
				SatsFounded[numSatsExisted++] = Est;
				SatsFounded[numSatsExisted] = 0;
				ponEnLCD(2,SatsFounded);
        	}
	//            chprintf("\r\nLlamo a ponTipoSatelite s:%d result:%d buff:%s\r\n",s,result,bufferRx);
        }
        else
        {
        */
		ponMensaje(bufferTx, sizeof(bufferTx),Est,0);
		for (numTry=0;numTry<=2;numTry++)
		{
			chprintStrCOM1(bufferTx);
			result = leeRespuesta(bufferRx,sizeof(bufferRx));
			if (result>=0) break;
		}
		//ponEnLCD(1,"");
		if (result>=0)
		{
			//chprintf("%c ok",Est);
			if (numSatsExisted<20)
			{
				SatsFounded[numSatsExisted++] = Est;
				SatsFounded[numSatsExisted] = 0;
				ponEnLCD(2,SatsFounded);
			}
	//            chprintf("\r\nLlamo a ponTipoSatelite s:%d result:%d buff:%s\r\n",s,result,bufferRx);
		}
    }
    ponEnLCD(0,"");
    ponEnLCD(1,"");
    chsnprintf(buffSats,sizeof(buffSats),"Satelites: %d",numSatsExisted);
    ponEnLCD(3,buffSats);
//   	chThdSleep(3000);
   	if (estabaArrancado==1)
   		chMsgSend(procesoGestorThreads,(msg_t) 1); // arranca PLC
}

int okSatelites(void)
{
	char Est;
    int result,s,numTry, numErrores;
    numErrores = 0;
    ponEnLCD(0,"Buscando satelites");
    ponEnLCD(1,"");
    ponEnLCD(2,"");
    ponEnLCD(3,"");
    for (s=0;s<numSatelites;s++)
    {
    	if (satelites[s].debeRastrearse == 0) continue;
    	Est = satelites[s].nombre;
        chsnprintf(buffSats,sizeof(buffSats),"Probando '%c'",Est);
        ponEnLCD(1,buffSats);
        ponMensajeLargo(bufferTx, sizeof(bufferTx),Est,0);
        for (numTry=0;numTry<=2;numTry++)
        {
            chprintStrCOM1(bufferTx);
            ponEnLCD(2,bufferTx);
            result = leeRespuesta(bufferRx,sizeof(bufferRx));
            if (result>=0)
            {
                chsnprintf(buffSats,sizeof(buffSats),"%c OK Largo",Est);
                ponEnLCD(3,buffSats);
               	chThdSleep(200);
            	break;
            }
        }
        if (result<0)
        {
			ponMensaje(bufferTx, sizeof(bufferTx),Est,0);
			for (numTry=0;numTry<=2;numTry++)
			{
				chprintStrCOM1(bufferTx);
				ponEnLCD(2,bufferTx);
				result = leeRespuesta(bufferRx,sizeof(bufferRx));
				if (result>=0)
				{
	                chsnprintf(buffSats,sizeof(buffSats),"%c OK Corto",Est);
	                ponEnLCD(3,buffSats);
	               	chThdSleep(200);
					break;
				}
			}
        }
        //ponEnLCD(1,"");
        if (result>=0)
        {
            //chprintf("%c ok",Est);
//            chprintf("\r\nLlamo a ponTipoSatelite s:%d result:%d buff:%s\r\n",s,result,bufferRx);
//          	chThdSleep(200);
            ponTipoSatelite(s,result,bufferRx);
        }
        else
        {
            numErrores++;
            chsnprintf(buffSats,sizeof(buffSats),"No encuentro %c",Est);
            ponEnLCD(3,buffSats);
            chprintf("No encuentro satelite %c\r\n",Est);
           	chThdSleep(1000);
        }
    }
    ponEnLCD(1,"");
    ponEnLCD(2,"");
    chsnprintf(buffSats,sizeof(buffSats),"Satelites ok: %d/%d",numSatelites-numErrores,numSatelites);
    ponEnLCD(3,buffSats);
   	chThdSleep(200);
   	if (numErrores>0)
   		return 0;
   	else
   		return 1;
}


static WORKING_AREA(waThreadRs485, 256);
/*
 * Comunicacion master RS485
 */

static msg_t Thread485Maestro(void __attribute__((unused)) *arg) {
    int valor,result,i,error,timeToRetry, todosOk;
    uint32_t entradas;
    chRegSetThreadName("rs485");
    timeToRetry = 0;
    estadoRs485 = 1;
    do
    {
    	todosOk = 1;
        for (i=0;i<numSatelites;i++)
        {
        	if (satelites[i].status == 0 && timeToRetry != 0) continue;
            chMtxLock(&MtxRs485);
            valor = satelites[i].valoresSalida;
            chMtxUnlock();
            if (satelites[i].tipoSatelite==1)
            	ponMensaje(bufferTx, sizeof(bufferTx),satelites[i].nombre,valor);
            else
            	ponMensajeLargo(bufferTx, sizeof(bufferTx),satelites[i].nombre,valor);
            chprintStrCOM1(bufferTx);
            // Leo respuesta hasta "*"
            result = leeRespuesta(bufferRx,sizeof(bufferRx));
            if (result > 0)
            {
                //ponEnLCD(1,"Ok");
                //chsnprintf(buffMsg485,sizeof(buffMsg485),"%s: %s",hallaPuntCadena(posCadAjuste), estados[nuevoEstado]);
                //ponEnLCD(0,buffMsg485);

                entradas = leeRespuestaAOk(i,bufferRx,&error);
                if (error>=0)
                {
                    chMtxLock(&MtxRs485);
                    satelites[i].valoresEntrada = entradas;
                    satelites[i].status = 1;
                    chMtxUnlock();
                }
            }
            else
            {
                if (result == -1 && satelites[i].status==1)
                	ponMsgEnLCD("Timeout");
                estadoRs485 = 2;
                chMtxLock(&MtxRs485);
                satelites[i].valoresEntrada = 0;
                satelites[i].status = 0;
                satelites[i].numErrores += 1;
                chMtxUnlock();
                todosOk = 0;
            }
            if (result == -2)
            	ponMsgEnLCD("Buffer excedido");
        	chThdSleep(5);
        }
        if (todosOk==0)
        	estadoRs485 = 1;
        else
        	estadoRs485 = 2;
    	timeToRetry += 5*numSatelites;
        if (chThdShouldTerminate())
        {
            for (i=0;i<numSatelites;i++)
            {
            	satelites[i].status = 0;
            	satelites[i].valoresEntrada = 0;
            }
        	estadoRs485 = 0;
            chThdExit((msg_t) 1);
        }
    	chThdSleep(50);
    	timeToRetry += 50;
    	if (timeToRetry>5000) timeToRetry = 0;
    } while (1==1);
    return 0;
}


/*
 * Comunicacion RS485 esclavo
 */
static msg_t Thread485Esclavo(void __attribute__((unused)) *arg) {
    int posBuff = 0;
    do
    {
        msg_t chInp = sdGetTimeout(&SD1,5000);
        if (chInp==Q_TIMEOUT)
        {
            posBuff = 0;
			continue;
        }
        char ch = (char) chInp;
        //chprintf("%c",ch);
		if (ch==':')
			posBuff = 0;
		else
			if (posBuff==0)
				continue;
        buffer[posBuff] = ch;
        if (++posBuff>38) posBuff = 38;
        if (ch=='*')
        {
            buffer[posBuff] = 0;
		    procesaMensaje(buffer,posBuff);
            posBuff = 0;
        }
    } while (1==1);
     return 0;
}


/**
 * @brief ordenar parar al 485
 *
 */

void pararRs485(void)
{
    if (proceso485 == NULL)
    {
        return;
    }
    chThdTerminate(proceso485);
    chThdWait(proceso485);
    proceso485 = NULL;
    ponEnLCD(3,"rs485 terminado");
}


// devuelve exito del start
uint8_t startRs485(void)
{
    int okSat;
    if (proceso485 != NULL)
    {
        ponEnLCD(3,"Paro primero rs485");
        pararRs485();
//        chThdSleepMilliseconds(200);
    }
    if (numSatelites<=0)
    {
    	ponMsgEnLCD("No necesito rs485");
    	estadoRs485 = 0;
        proceso485 = NULL;
        return 1;
    }
    if (nombEstacion=='M' || nombEstacion=='m')
    {
    	okSat = okSatelites();
    	proceso485 = chThdCreateStatic(waThreadRs485, sizeof(waThreadRs485), NORMALPRIO, Thread485Maestro, NULL);
    	ponEnLCD(3,"Arrancado rs485Master");
    	return okSat;
    	//chThdSleepMilliseconds(200);
    }
    else
    {
    	proceso485 = chThdCreateStatic(waThreadRs485, sizeof(waThreadRs485), NORMALPRIO, Thread485Esclavo, NULL);
    	ponEnLCD(3,"Arrancado rs485Escl");
    	return 1;
    	//chThdSleepMilliseconds(500);
    }
}

