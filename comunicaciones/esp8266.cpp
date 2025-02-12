/*
 * esp8266.c
 *
 *  Created on: 1/8/2017
 *      Author: joaquin
 */

#include "ch.hpp"
#include "hal.h"

using namespace chibios_rt;

#include "../SMS/sms.h"
#include "chprintf.h"
#include "tty.h"
#include "gets.h"
#include "string.h"
//#include "infrared.h"
#include "lcd.h"


extern uint8_t hayWifi, hayGPRS, hayCAN, prefWifi, prefMovil, mqttMedCoche, mqttMedTotales, mqttEstado, smsFinCarga, smsEstado, smsBotonParo;
extern uint16_t mqqtTiempo;
extern uint8_t mqqtSSL, esp8266AP[21], esp8266Psswd[26];

uint8_t bufferGets8266[200];
extern uint8_t mqqtOk, wifiOk;
int32_t mqqtPortValor(void);
char *mqqtServerValor(void);
char *apnValor(void);

uint8_t conectarEsp8266TCPMqqt(BaseChannel  *pSD)
{
  uint8_t hayError, numParametros, *parametros[10];
  char buffer[80];
  mqqtOk = 0;

  // hayError = modemOrden("AT+CIPSTART=\"TCP\",\"jcfmain.synology.me\",1883\r\n",MS2ST(4000),&respuesta);
  chsnprintf(buffer,sizeof(buffer),"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",mqqtServerValor(),mqqtPortValor());
  hayError = modemOrden(pSD,buffer, bufferGets8266, sizeof(bufferGets8266),chTimeMS2I(4000));
  hayError = modemOrden(pSD,"AT+CIPMODE=1\r\n", bufferGets8266, sizeof(bufferGets8266),chTimeMS2I(1000));
  if (hayError!=0)
  {
    ponEnLCD(3,"Error Wifi CIPMODE!");
    chThdSleepMilliseconds(4000);
    ponEnLCD(0,"");
    ponEnLCD(1,"");
    ponEnLCD(2,"");
    return 3;
  }
  // Devuelve STATUS:2
  hayError = modemParametros(pSD,"AT+CIPSTATUS\r\n","STATUS:", bufferGets8266, sizeof(bufferGets8266),chTimeMS2I(500),&numParametros, parametros);
  if (hayError)
  {
    ponEnLCD(3,"Error Wifi CIPSTATUS");
    chThdSleepMilliseconds(4000);
    ponEnLCD(0,"");
    ponEnLCD(1,"");
    ponEnLCD(2,"");
    return 3;
  }
  hayError = modemParametros(pSD,"AT+CIPSEND\r\n",">", bufferGets8266, sizeof(bufferGets8266),chTimeMS2I(500),&numParametros, parametros);
  // da timeout porque no hay \r ni \n
  if (bufferGets8266[0]!='>')
  {
    ponEnLCD(3,"Error Wifi CIPSEND");
    chThdSleepMilliseconds(4000);
    ponEnLCD(0,"");
    ponEnLCD(1,"");
    ponEnLCD(2,"");
    return 3;
  }
  mqqtOk = 1;
  chLcdprintfFila(3,"Wifi conectada MQTT");
  chThdSleepMilliseconds(500);
  return 0;
}

uint8_t initEsp8266(BaseChannel  *pSD)
{
  uint8_t hayError, numIntentos, numParametros, *parametros[10];
  char buffer[80];
  char IPMAIN[20];
  ponEnLCD(0,"Inicializo Wifi");
  ponEnLCD(1,"");
  ponEnLCD(2,"");
  ponEnLCD(3,"");
  wifiOk = 0;
  numIntentos = 1;
  do
  {
    hayError = modemOrden(pSD,"AT\r\n",bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(500));
    if (!hayError) break;
    hayError = modemOrden(pSD,"AT\r\n",bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(500));
    if (!hayError) break;
    chLcdprintfFila(1,"Modo orden %d/3",numIntentos);
    osalThreadSleepMilliseconds(1300);
    modemOrden(pSD,"+++",bufferGets8266, sizeof(bufferGets8266),chTimeMS2I(1000));
    osalThreadSleepMilliseconds(1300);
  } while (hayError && numIntentos++<3);
  if (hayError==1)
  {
      ponEnLCD(1,"No responde, aborto");
      chThdSleepMilliseconds(4000);
      ponEnLCD(0,"");
      ponEnLCD(1,"");
      return 1;
  }
  ponEnLCD(1,"");
  // resetear modulo
  hayError = modemOrden(pSD,"AT+RST\r\n", bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(2000));

  //Disconnect from AP  AT+CWQAP
  //hayError = modemOrden(pSD,"AT+CWQAP\r\n", bufferGets8266, sizeof(bufferGets8266), TIME_MS2I(2000));
  // Version
  hayError = modemOrden(pSD,"AT+GMR\r\n", bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(2000));
  // Abandono modo AP
  hayError = modemOrden(pSD,"AT+CWMODE=1\r\n", bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(500));
  osalThreadSleepMilliseconds(2000);
  // Conecto a WIFI
//  hayError = modemOrden("AT+CWJAP=\"MOVISTAR_E33A\",\"5266CCC587A255498D33"\r\n",MS2ST(5000),&respuesta);
  chsnprintf(buffer,sizeof(buffer),"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",esp8266AP,esp8266Psswd);
  hayError = modemOrden(pSD,buffer, bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(15000));
  if (hayError!=0)
  {
      ponEnLCD(1,"No conecto a WIFI!!");
      chThdSleepMilliseconds(4000);
      ponEnLCD(0,"");
      ponEnLCD(1,"");
      ponEnLCD(2,"");
      return 1;
  }
  // IP
  hayError = modemParametros(pSD,"AT+CIFSR\r\n","CIFSR:", bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(500),&numParametros, parametros);
  if (hayError==0)
  {
    if (numParametros>=1 && parametros[0]!=NULL)
    {
        chLcdprintfFila(1,"IP %s",parametros[1]);
        strncpy(IPMAIN, (char *)parametros[0],sizeof(IPMAIN));
    }
  }
  // Devuelve STATUS:2
  hayError = modemParametros(pSD,"AT+CIPSTATUS\r\n","STATUS:", bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(500),&numParametros, parametros);
  // Modo IP
  //hayError = modemParametros("AT+CIPDOMAIN=\"jcfmain.synology.me\"\r\n","+CIPDOMAIN:",MS2ST(1000),&numParametros, parametros);
  chsnprintf(buffer,sizeof(buffer),"AT+CIPDOMAIN=\"%s\"\r\n",mqqtServerValor());
  hayError = modemParametros(pSD,buffer, "+CIPDOMAIN:", bufferGets8266, sizeof(bufferGets8266), chTimeMS2I(1000), &numParametros, parametros);
  if (hayError==0)
  {
    if (numParametros>=1 && parametros[0]!=NULL)
    {
        chLcdprintfFila(2,"MQTT %s",parametros[0]);
        strncpy(IPMAIN, (char *)parametros[0],sizeof(IPMAIN));
    }
  }
  wifiOk = 1;
  return 0;
}
