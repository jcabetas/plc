/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "chprintf.h"


#include "ccportab.h"

void testMB(void);
int canTest(void);
void testADC(void);


/*
 * This is a periodic thread that does absolutely nothing except flashing
 * a LED.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palSetLine(LINE_LED);
    chThdSleepMilliseconds(50);
    palClearLine(LINE_LED);
    chThdSleepMilliseconds(200);
  }
}

static const SerialConfig ser_cfg = {
    115200, 0, 0, 0,
};

void initSerial1(void)
{
    palClearLine(LINE_TX1);
    palSetLine(LINE_RX1);
    palSetLineMode(LINE_TX1, PAL_MODE_ALTERNATE(7));
    palSetLineMode(LINE_RX1, PAL_MODE_ALTERNATE(7));
    sdStart(&SD1,&ser_cfg);
    chprintf((BaseSequentialStream*) &SD1,"Hola SD1\r\n");
}



/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 1 using the driver default configuration.
   */
  initSerial1();
  //canTest();

  /*
   * #define STM32H7_REV_Z 0x1001
   * #define STM32H7_REV_Y 0x1003
   * #define STM32H7_REV_X 0x2001
   * #define STM32H7_REV_V 0x2003
   */
  uint32_t code = DBGMCU->IDCODE; //0x20036450
  uint16_t revCode = code >>16;
  chprintf((BaseSequentialStream*) &SD1,"revCode:%4X",revCode);
  if (revCode == 0x1001)
      chprintf((BaseSequentialStream*) &SD1,".. es RevZ\r\n");
  else if (revCode == 0x1003)
      chprintf((BaseSequentialStream*) &SD1,".. es RevY\r\n");
  else if (revCode == 0x2001)
      chprintf((BaseSequentialStream*) &SD1,".. es RevX\r\n");
  else if (revCode == 0x2003)
      chprintf((BaseSequentialStream*) &SD1,".. es RevV\r\n");
  else
      chprintf((BaseSequentialStream*) &SD1,".. rev. desconocida\r\n");


  testADC();

  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (1) {
    chThdSleepMilliseconds(500);
  }
}
