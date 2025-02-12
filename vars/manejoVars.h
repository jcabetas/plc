/*
 * manejoVars.h
 *
 *  Created on: 1/11/2019
 *      Author: jcabe
 */

#ifndef VARS_MANEJOVARS_H_
#define VARS_MANEJOVARS_H_

extern "C" {
    #include "ch.hpp"
    #include "hal.h"
    #include "tipoVars.h"
}

int8_t gestionaMenuVarsCPP(uint8_t numItemsMenu, menuItem *menuVars[]);
void gestionaMenuSelsCPP(variableSelects *varSel);
int32_t leeNum(int32_t valorPar, int32_t valorMinPar, int32_t valorMaxPar, const char descPar[]);
void devuelveTlfyNombre(uint8_t numEst, char *tlf, uint8_t sizetlf, char *nmbr, uint8_t sizenmbr);
uint8_t findEstPorTelefono(char *tlf);



#endif /* VARS_MANEJOVARS_H_ */
