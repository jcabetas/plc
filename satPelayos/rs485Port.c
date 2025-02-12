/*
 * rs485Port.c
 *
 *  Created on: 10/02/2012
 *      Author: joaquin
 */

#include "ch.h"
#include "hal.h"
/*
 * This callback is invoked when a transmission buffer has been completely
 * read by the driver.
 */
static void txend1Vacia(UARTDriver *uartp) {

  (void)uartp;
}


/*
 * This callback is invoked when a transmission has physically completed.
 */
static void txend2(UARTDriver *uartp) {

  (void)uartp;
  palClearPad(TXRX_PORT, TXRX_PIN); /* Desactivo TX */
}

static void txend2Vacia(UARTDriver *uartp) {

  (void)uartp;
}

/*
 * This callback is invoked on a receive error, the errors mask is passed
 * as parameter.
 */
static void rxerrVacia(UARTDriver *uartp, uartflags_t e) {

  (void)uartp;
  (void)e;
}

/*
 * This callback is invoked when a character is received but the application
 * was not ready to receive it, the character is passed as parameter.
 */
static void rxcharVacia(UARTDriver *uartp, uint16_t c) {

  (void)uartp;
  (void)c;
}

/*
 * This callback is invoked when a receive buffer has been completely written.
 */
static void rxendVacia(UARTDriver *uartp) {
  (void)uartp;
}

/*
 * UART driver configuration structure.
 */
static UARTConfig uart_cfg_2 = {
  txend1Vacia,
  txend2Vacia,
  rxendVacia,
  rxcharVacia,
  rxerrVacia,
  9600,
  0,
  USART_CR2_LINEN,
  0
};

static UARTConfig uart_cfg_3 = {
  txend1Vacia,
  txend2Vacia,
  rxendVacia,
  rxcharVacia,
  rxerrVacia,
  115200,
  0,
  USART_CR2_LINEN,
  0
};

void initUart(void) {
  uartStart(&UARTD2, &uart_cfg_2);  // modem,    9600 baudios
  uartStart(&UARTD3, &uart_cfg_3);  // consola,115200 baudios
}

