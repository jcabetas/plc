/*
 * comunicaciones.h
 *
 *  Created on: 1/8/2017
 *      Author: joaquin
 */

#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

uint8_t initSIM800SMS(BaseChannel  *pSD, uint8_t verbose);
uint8_t initSIM800GPRS(BaseChannel  *pSD, uint8_t verbose);
uint8_t conectarSim800TCPMqqt(BaseChannel  *pSD, uint8_t useSSL, uint8_t verbose);
uint8_t hayProblemasComunicaciones(void);
uint8_t hayProblemasWifi(void);
uint8_t hayProblemasGprs(void);
uint8_t mataWifi(void);
void mataGprs(void);
void startWifi(uint8_t verbose);
void mataSMS(void);
uint8_t initEsp8266(BaseChannel  *pSD);
uint8_t conectarEsp8266TCPMqqt(BaseChannel  *pSD);
void startEnvioMedidasMqtt(void);
void initThreadSMS(BaseChannel  *pSD);
uint8_t modemOrden(BaseChannel  *pSD, const char *orden, uint8_t *buffer, uint8_t bufferSize, systime_t timeout);
uint8_t modemParametro(BaseChannel  *pSD, const char *orden, uint8_t *buffer, uint16_t bufferSize, systime_t timeout);
uint16_t modemParametros(BaseChannel  *pSD, const char *orden, const char *cadRespuesta, uint8_t *buffer, uint16_t bufferSize, systime_t timeout,
                         uint8_t *numParametros,uint8_t *parametros[]);
void chgetsNoEchoTimeOut(BaseChannel  *pSD,  uint8_t *buffer, uint16_t bufferSize, systime_t timeout, uint8_t *huboTimeout);
uint8_t chgetchTimeOut(BaseChannel  *pSD, systime_t timeout, uint8_t *huboTimeout);
int8_t sendSMS(BaseChannel  *pSD, char *msg, uint8_t *numTelefono);
uint8_t ponHoraConGprs(BaseChannel  *pSD);
uint8_t horaSMStoTM(uint8_t *cadena, struct tm *fecha);
uint16_t lee2car(uint8_t *buffer, uint16_t posIni,uint16_t minValor, uint16_t maxValor, uint8_t *hayError);


#endif /* COMUNICACIONES_H_ */
