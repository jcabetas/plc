/*
    ChibiOS/RT - Copyright (C) 2006-2007 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file rs485.h
 * @brief rs485 Driver macros
 * @addtogroup SERIAL
 * @{
 */

#ifndef _RS485_H_
#define _RS485_H_

#define MAXSATELITES 10
void initUart(void);
typedef struct  {
    char nombre;
    uint8_t debeRastrearse;
    int tipoSatelite;
    int status;
    int numErrores;
    uint32_t valoresSalida;
    uint32_t valoresEntrada;
    } satelite;
void initUART485(void);
void pararRs485(void);
uint8_t startRs485(void);
int checkSum(char *buffer,int buffSize);
void ponMensaje(char *buffer, int buffSize, char estacion, int valor);
void ponMensajeLargo(char *buffer, int buffSize, char estacion, int valor);
void chprintStrCOM1(char *str);
int leeRespuesta(char *buffer, int bufferSize);
void procesaMensaje(char *bufferRx,int posRx);
void enviaOk(int valInputs);
void enviaOkLargo(int valInputs);
void anadeSatelite(char nombre);
int leeInput485(int numSatelite, int numPin);
void asignaOutput485(int numSatelite, int numPin, int valor);
void anadeSatelite(char nombre);
void sateliteARastrear(char nombEst);
void ponTipoSatelite(int posSatelite, int longAnsw,char *buff);
uint32_t leeRespuestaAOk(int numSatelite,char *bufferRx,int *error);
int leeHex(char *buffer, int posIni, int posFin);
int checkHex(char *buffer, int posIni, int posFin);
int init485DesdePlc(void);
void ponStatusSMSResumido(void);
void ponStatusSMSAll(void);
void exploraSatelites(void);
int okSatelites(void);
void estadoSatelites(void);
#endif /* _RS485_H_ */

/** @} */
