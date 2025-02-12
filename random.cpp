/*
 * random.c
 *
 *  Created on: 26/12/2019
 *      Author: jcabe
 */

#include "ch.hpp"
#include "hal.h"
extern "C" {
    void initRandom(void);
}
using namespace chibios_rt;


class animal
{
  uint8_t valor;
  animal(uint8_t initVal)
  {
    valor = initVal;
  }
  ~animal()
  {

  }
};

uint32_t randomU32(void) {
  static uint32_t last_value=0;
  static uint32_t new_value=0;
//  uint32_t ssr;
  uint32_t error_bits = 0;
  error_bits = RNG_SR_SEIS | RNG_SR_CEIS;
  while (new_value==last_value) {
    /* Check for error flags and if data is ready. */
    if ( ((RNG->SR & error_bits) == 0) && ( (RNG->SR & RNG_SR_DRDY) == 1 ) )
      new_value=RNG->DR;
    else
    {
        if (RNG->SR & RNG_SR_SEIS)
        {
            // What you should do is clear the SEIS bit, then clear and set the RNGEN bit to reinitialize
            // and restart the RNG.
            RNG->SR &= ~RNG_SR_SEIS;
            RNG->CR &= ~RNG_CR_RNGEN;
            RNG->CR |=  RNG_CR_RNGEN;
        }
        new_value=RNG->DR;
//        ssr = RNG->SR;
        break;
    }
  }
  last_value=new_value;
  return new_value;
}

int32_t randomNum(int32_t numMin, int32_t numMax)
{
    uint16_t numAleat = randomU32() & 0xFFFF;
    return (numMin+numAleat*(numMax-numMin)/0xFFFF);
}


void initRandom(void)
{
//    trngInit();
//    trngObjectInit(&TRNG1);
//    trngStart(&TRNG1,NULL);

    rccEnableAHB2(RCC_AHB2ENR_RNGEN, 0);
    RNG->CR |= RNG_CR_IE;
    RNG->CR |= RNG_CR_RNGEN;

}

