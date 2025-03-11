/*
    ChibiOS - Copyright (C) 2006..2020 Giovanni Di Sirio

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

/*
 * This file has been automatically generated using ChibiStudio board
 * generator plugin. Do not edit manually.
 */

#ifndef BOARD_H
#define BOARD_H

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*
 * Setup for STMicroelectronics STM32 Nucleo144-H743ZI board.
 */

/*
 * Board identifier.
 */
#define BOARD_ST_NUCLEO144_H743ZI
#define BOARD_NAME                  "STMicroelectronics STM32 Nucleo144-H743ZI"

/*
 * Ethernet PHY type.
 */
#define BOARD_PHY_ID                MII_LAN8742A_ID
#define BOARD_PHY_RMII

/*
 * Board oscillators-related settings.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                32768U
#endif

#define STM32_LSEDRV                (3U << 3U)

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                25000000U
#endif

//#define STM32_HSE_BYPASS

/*
 * MCU type as defined in the ST header.
 */
#define STM32H743xx

/*
 * IO pins assignments.
 */
#define GPIOA_TX4                   0U
#define GPIOA_RX4                   1U
#define GPIOA_TX2                   2U
#define GPIOA_RX2                   3U
#define GPIOA_SPI1_CS               4U
#define GPIOA_SCK1                  5U
#define GPIOA_MISO1                 6U
#define GPIOA_MOSI1                 7U
#define GPIOA_RCC_MCO_1             8U
#define GPIOA_TX1                   9U
#define GPIOA_RX1                   10U
#define GPIOA_USB1_DN               11U
#define GPIOA_USB1_DP               12U
#define GPIOA_SWDIO                 13U
#define GPIOA_SWCLK                 14U
#define GPIOA_TIM2CH1               15U

#define GPIOB_PIN0                  0U
#define GPIOB_PIN1                  1U
#define GPIOB_QSPI_CLK              2U
#define GPIOB_SPIx_CLK              3U
#define GPIOB_SPIX_MISO             4U
#define GPIOB_SENSOR3               5U
#define GPIOB_QSPI_NCS              6U
#define GPIOB_DCMI_VSYNC            7U
#define GPIOB_I2C1_SCL              8U
#define GPIOB_I2C1_SDA              9U
#define GPIOB_I2C2_SCL              10U
#define GPIOB_I2C2_SDA              11U
#define GPIOB_RST_RFM               12U
#define GPIOB_SCK2                  13U
#define GPIOB_MISO2                 14U
#define GPIOB_MOSI2                 15U

#define GPIOC_SPI1_CS2              0U
#define GPIOC_4_20mA_1              1U
#define GPIOC_D_C                   2U
#define GPIOC_4_20mA_2              3U
#define GPIOC_VPILOT                4U
#define GPIOC_PIN5                  5U
#define GPIOC_DIO0                  6U
#define GPIOC_SENSOR4               7U
#define GPIOC_SDMMC1_D0             8U
#define GPIOC_SDMMC1_D1             9U
#define GPIOC_SDMMC1_D2             10U
#define GPIOC_SDMMC1_D3             11U
#define GPIOC_SDMMC1_CK             12U
#define GPIOC_SW2                   13U
#define GPIOC_OSC32_IN              14U
#define GPIOC_OSC32_OUT             15U

#define GPIOD_CAN1RX                0U
#define GPIOD_CAN1TX                1U
#define GPIOD_SDMMC1_CMD            2U
#define GPIOD_DCMI_D5               3U
#define GPIOD_SENSOR1               4U
#define GPIOD_SENSOR2               5U
#define GPIOD_SPIx_CS               6U
#define GPIOD_SPIx_MOSI             7U
#define GPIOD_RELE1                 8U
#define GPIOD_RELE2                 9U
#define GPIOD_RELE3                 10U
#define GPIOD_QSPI_IO0              11U
#define GPIOD_QSPI_IO1              12U
#define GPIOD_QSPI_IO3              13U
#define GPIOD_RELE4                 14U
#define GPIOD_NSS                   15U

#define GPIOE_CANSILENT             0U
#define GPIOE_TXRX                  1U
#define GPIOE_QSPI_IO2              2U
#define GPIOE_LED                   3U
#define GPIOE_RESETCON              4U
#define GPIOE_DCMI_D6               5U
#define GPIOE_IRQCON                6U
#define GPIOE_RX7                   7U
#define GPIOE_TX7                   8U
#define GPIOE_ENHM10                9U
#define GPIOE_LCD_LED               10U
#define GPIOE_LCD_CS                11U
#define GPIOE_LCD_SCL               12U
#define GPIOE_LCD_WR_RS             13U
#define GPIOE_LCD_SDA               14U
#define GPIOE_STHM10                15U

#define GPIOF_PIN0                  0U
#define GPIOF_PIN1                  1U
#define GPIOF_PIN2                  2U
#define GPIOF_PIN3                  3U
#define GPIOF_PIN4                  4U
#define GPIOF_PIN5                  5U
#define GPIOF_PIN6                  6U
#define GPIOF_PIN7                  7U
#define GPIOF_PIN8                  8U
#define GPIOF_PIN9                  9U
#define GPIOF_PIN10                 10U
#define GPIOF_PIN11                 11U
#define GPIOF_PIN12                 12U
#define GPIOF_PIN13                 13U
#define GPIOF_PIN14                 14U
#define GPIOF_PIN15                 15U

#define GPIOG_PIN0                  0U
#define GPIOG_PIN1                  1U
#define GPIOG_PIN2                  2U
#define GPIOG_PIN3                  3U
#define GPIOG_PIN4                  4U
#define GPIOG_PIN5                  5U
#define GPIOG_USB_FS_PWR_EN         6U
#define GPIOG_USB_FS_OVCR           7U
#define GPIOG_PIN8                  8U
#define GPIOG_PIN9                  9U
#define GPIOG_PIN10                 10U
#define GPIOG_RMII_TX_EN            11U
#define GPIOG_PIN12                 12U
#define GPIOG_RMII_TXD0             13U
#define GPIOG_PIN14                 14U
#define GPIOG_PIN15                 15U

#define GPIOH_OSC_IN                0U
#define GPIOH_OSC_OUT               1U
#define GPIOH_PIN2                  2U
#define GPIOH_PIN3                  3U
#define GPIOH_PIN4                  4U
#define GPIOH_PIN5                  5U
#define GPIOH_PIN6                  6U
#define GPIOH_PIN7                  7U
#define GPIOH_PIN8                  8U
#define GPIOH_PIN9                  9U
#define GPIOH_PIN10                 10U
#define GPIOH_PIN11                 11U
#define GPIOH_PIN12                 12U
#define GPIOH_PIN13                 13U
#define GPIOH_PIN14                 14U
#define GPIOH_PIN15                 15U

#define GPIOI_PIN0                  0U
#define GPIOI_PIN1                  1U
#define GPIOI_PIN2                  2U
#define GPIOI_PIN3                  3U
#define GPIOI_PIN4                  4U
#define GPIOI_PIN5                  5U
#define GPIOI_PIN6                  6U
#define GPIOI_PIN7                  7U
#define GPIOI_PIN8                  8U
#define GPIOI_PIN9                  9U
#define GPIOI_PIN10                 10U
#define GPIOI_PIN11                 11U
#define GPIOI_PIN12                 12U
#define GPIOI_PIN13                 13U
#define GPIOI_PIN14                 14U
#define GPIOI_PIN15                 15U

#define GPIOJ_PIN0                  0U
#define GPIOJ_PIN1                  1U
#define GPIOJ_PIN2                  2U
#define GPIOJ_PIN3                  3U
#define GPIOJ_PIN4                  4U
#define GPIOJ_PIN5                  5U
#define GPIOJ_PIN6                  6U
#define GPIOJ_PIN7                  7U
#define GPIOJ_PIN8                  8U
#define GPIOJ_PIN9                  9U
#define GPIOJ_PIN10                 10U
#define GPIOJ_PIN11                 11U
#define GPIOJ_PIN12                 12U
#define GPIOJ_PIN13                 13U
#define GPIOJ_PIN14                 14U
#define GPIOJ_PIN15                 15U

#define GPIOK_PIN0                  0U
#define GPIOK_PIN1                  1U
#define GPIOK_PIN2                  2U
#define GPIOK_PIN3                  3U
#define GPIOK_PIN4                  4U
#define GPIOK_PIN5                  5U
#define GPIOK_PIN6                  6U
#define GPIOK_PIN7                  7U
#define GPIOK_PIN8                  8U
#define GPIOK_PIN9                  9U
#define GPIOK_PIN10                 10U
#define GPIOK_PIN11                 11U
#define GPIOK_PIN12                 12U
#define GPIOK_PIN13                 13U
#define GPIOK_PIN14                 14U
#define GPIOK_PIN15                 15U

/*
 * IO lines assignments.
 */
/*
#define LINE_RMII_REF_CLK           PAL_LINE(GPIOA, 1U)
#define LINE_RMII_MDIO              PAL_LINE(GPIOA, 2U)
#define LINE_RMII_CRS_DV            PAL_LINE(GPIOA, 7U)
#define LINE_USB_SOF                PAL_LINE(GPIOA, 8U)
#define LINE_MCO1                   PAL_LINE(GPIOA, 8U)
#define LINE_USB_VBUS               PAL_LINE(GPIOA, 9U)
#define LINE_USB_ID                 PAL_LINE(GPIOA, 10U)
#define LINE_USB_DM                 PAL_LINE(GPIOA, 11U)
#define LINE_USB_DP                 PAL_LINE(GPIOA, 12U)
#define LINE_SWDIO                  PAL_LINE(GPIOA, 13U)
#define LINE_SWCLK                  PAL_LINE(GPIOA, 14U)
#define LINE_T_JTDI                 PAL_LINE(GPIOA, 15U)
#define LINE_LED1                   PAL_LINE(GPIOB, 0U)
#define LINE_LED_GREEN              PAL_LINE(GPIOB, 0U)
#define LINE_LED                    PAL_LINE(GPIOB, 0U)
#define LINE_SWO                    PAL_LINE(GPIOB, 3U)
#define LINE_LED2                   PAL_LINE(GPIOB, 7U)
#define LINE_LED_BLUE               PAL_LINE(GPIOB, 7U)
#define LINE_RMII_TXD1              PAL_LINE(GPIOB, 13U)
#define LINE_LED3                   PAL_LINE(GPIOB, 14U)
#define LINE_LED_RED                PAL_LINE(GPIOB, 14U)
#define LINE_RMII_MDC               PAL_LINE(GPIOC, 1U)
#define LINE_RMII_RXD0              PAL_LINE(GPIOC, 4U)
#define LINE_RMII_RXD1              PAL_LINE(GPIOC, 5U)
#define LINE_BUTTON                 PAL_LINE(GPIOC, 13U)
#define LINE_OSC32_IN               PAL_LINE(GPIOC, 14U)
#define LINE_OSC32_OUT              PAL_LINE(GPIOC, 15U)
#define LINE_USART3_RX              PAL_LINE(GPIOD, 8U)
#define LINE_STLK_RX                PAL_LINE(GPIOD, 8U)
#define LINE_USART3_TX              PAL_LINE(GPIOD, 9U)
#define LINE_STLK_TX                PAL_LINE(GPIOD, 9U)
#define LINE_USB_FS_PWR_EN          PAL_LINE(GPIOG, 6U)
#define LINE_USB_FS_OVCR            PAL_LINE(GPIOG, 7U)
#define LINE_RMII_TX_EN             PAL_LINE(GPIOG, 11U)
#define LINE_RMII_TXD0              PAL_LINE(GPIOG, 13U)
#define LINE_OSC_IN                 PAL_LINE(GPIOH, 0U)
#define LINE_OSC_OUT                PAL_LINE(GPIOH, 1U)
*/
#define LINE_LED                    PAL_LINE(GPIOE, GPIOE_LED)
#define LINE_SW2                    PAL_LINE(GPIOC, GPIOC_SW2)

#define LINE_TX1                    PAL_LINE(GPIOA, GPIOA_TX1)
#define LINE_RX1                    PAL_LINE(GPIOA, GPIOA_RX1)
#define LINE_TX4                    PAL_LINE(GPIOA, GPIOA_TX4)
#define LINE_RX4                    PAL_LINE(GPIOA, GPIOA_RX4)
#define LINE_TXRX                   PAL_LINE(GPIOE, GPIOE_TXRX)
#define LINE_LORA_DIO               PAL_LINE(GPIOC, GPIOC_DIO0)
#define LINE_OSCILADOR              PAL_LINE(GPIOA, GPIOA_TIM2CH1)
#define LINE_4_20mA_1               PAL_LINE(GPIOC, GPIOC_4_20mA_1)
#define LINE_4_20mA_2               PAL_LINE(GPIOC, GPIOC_4_20mA_2)
#define LINE_RELE1                  PAL_LINE(GPIOD, GPIOD_RELE1)
#define LINE_SENSOR1                PAL_LINE(GPIOD, GPIOD_SENSOR1)
#define LINE_SENSOR2                PAL_LINE(GPIOD, GPIOD_SENSOR2)
#define LINE_LORANSS                PAL_LINE(GPIOD, GPIOD_NSS)
#define LINE_LORARESET              PAL_LINE(GPIOB, GPIOB_RST_RFM)
#define LINE_SPI3SCK                PAL_LINE(GPIOB, GPIOB_SPIx_CLK)
#define LINE_SPI3MISO               PAL_LINE(GPIOB, GPIOB_SPIX_MISO)
#define LINE_SPI3MOSI               PAL_LINE(GPIOD, GPIOD_SPIx_CS)
#define LINE_SPI3CS                 PAL_LINE(GPIOD, GPIOD_SPIx_MOSI)
#define LINE_CANSILENT              PAL_LINE(GPIOE, GPIOE_CANSILENT)
#define LINE_VPILOT                 PAL_LINE(GPIOC, GPIOC_VPILOT)
#define LINE_TIM2CH1                PAL_LINE(GPIOA, GPIOA_TIM2CH1)


/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT(n)           (0U << ((n) * 2U))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2U))
#define PIN_MODE_ALTERNATE(n)       (2U << ((n) * 2U))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2U))
#define PIN_ODR_LOW(n)              (0U << (n))
#define PIN_ODR_HIGH(n)             (1U << (n))
#define PIN_OTYPE_PUSHPULL(n)       (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1U << (n))
#define PIN_OSPEED_VERYLOW(n)       (0U << ((n) * 2U))
#define PIN_OSPEED_LOW(n)           (1U << ((n) * 2U))
#define PIN_OSPEED_MEDIUM(n)        (2U << ((n) * 2U))
#define PIN_OSPEED_HIGH(n)          (3U << ((n) * 2U))
#define PIN_PUPDR_FLOATING(n)       (0U << ((n) * 2U))
#define PIN_PUPDR_PULLUP(n)         (1U << ((n) * 2U))
#define PIN_PUPDR_PULLDOWN(n)       (2U << ((n) * 2U))
#define PIN_AFIO_AF(n, v)           ((v) << (((n) % 8U) * 4U))

/*
 * GPIOA setup:
 * PA0  - GPIOA_TX4                 (alternate 8).
 * PA1  - GPIOA_RX4                 (alternate 8).
 * PA2  - GPIOA_TX2                 (alternate 7).
 * PA3  - GPIOA_RX2                 (alternate 7).
 * PA4  - GPIOA_SPI1_CS             (output pushpull maximum).
 * PA5  - GPIOA_SCK1                (alternate 5).
 * PA6  - GPIOA_MISO1               (alternate 5).
 * PA7  - GPIOA_MOSI1               (alternate 5).
 * PA8  - GPIOA_RCC_MCO_1           (input pullup).
 * PA9  - GPIOA_TX1                 (alternate 5).
 * PA10 - GPIOA_RX1                 (alternate 5).
 * PA11 - GPIOA_USB1_DN             (alternate 10).
 * PA12 - GPIOA_USB1_DP             (alternate 10).
 * PA13 - GPIOA_SwDIO               (alternate 0).
 * PA14 - GPIOA_SWCLK               (alternate 0).
 * PA15 - GPIOA_TIM2CH1             (alternate 1).
 */
#define VAL_GPIOA_MODER             (PIN_MODE_ALTERNATE(GPIOA_TX4) |           \
                                     PIN_MODE_ALTERNATE(GPIOA_RX4) |\
                                     PIN_MODE_ALTERNATE(GPIOA_TX2) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_RX2) |           \
                                     PIN_MODE_OUTPUT(GPIOA_SPI1_CS) |           \
                                     PIN_MODE_ALTERNATE(GPIOA_SCK1) |           \
                                     PIN_MODE_ALTERNATE(GPIOA_MISO1) |           \
                                     PIN_MODE_ALTERNATE(GPIOA_MOSI1) |\
                                     PIN_MODE_INPUT(GPIOA_RCC_MCO_1) |    \
                                     PIN_MODE_ALTERNATE(GPIOA_TX1) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_RX1) |     \
                                     PIN_MODE_ALTERNATE(GPIOA_USB1_DN) |     \
                                     PIN_MODE_ALTERNATE(GPIOA_USB1_DP) |     \
                                     PIN_MODE_ALTERNATE(GPIOA_SWDIO) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_SWCLK) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_TIM2CH1))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_TX4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RX4) |\
                                     PIN_OTYPE_PUSHPULL(GPIOA_TX2) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RX2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SPI1_CS) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SCK1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MISO1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MOSI1) |\
                                     PIN_OTYPE_PUSHPULL(GPIOA_RCC_MCO_1) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_TX1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RX1) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB1_DN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB1_DP) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWCLK) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_TIM2CH1))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_HIGH(GPIOA_TX4) |       \
                                     PIN_OSPEED_HIGH(GPIOA_RX4) |  \
                                     PIN_OSPEED_HIGH(GPIOA_TX2) |     \
                                     PIN_OSPEED_HIGH(GPIOA_RX2) |       \
                                     PIN_OSPEED_HIGH(GPIOA_SPI1_CS) |       \
                                     PIN_OSPEED_HIGH(GPIOA_SCK1) |       \
                                     PIN_OSPEED_HIGH(GPIOA_MISO1) |       \
                                     PIN_OSPEED_HIGH(GPIOA_MOSI1) |   \
                                     PIN_OSPEED_HIGH(GPIOA_RCC_MCO_1) |       \
                                     PIN_OSPEED_HIGH(GPIOA_TX1) |      \
                                     PIN_OSPEED_HIGH(GPIOA_RX1) |        \
                                     PIN_OSPEED_HIGH(GPIOA_USB1_DN) |        \
                                     PIN_OSPEED_HIGH(GPIOA_USB1_DP) |        \
                                     PIN_OSPEED_HIGH(GPIOA_SWDIO) |         \
                                     PIN_OSPEED_HIGH(GPIOA_SWCLK) |         \
                                     PIN_OSPEED_HIGH(GPIOA_TIM2CH1))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_TX4) |         \
                                     PIN_PUPDR_PULLUP(GPIOA_RX4) |\
                                     PIN_PUPDR_FLOATING(GPIOA_TX2) |    \
                                     PIN_PUPDR_FLOATING(GPIOA_RX2) |         \
                                     PIN_PUPDR_PULLUP(GPIOA_SPI1_CS) |         \
                                     PIN_PUPDR_PULLUP(GPIOA_SCK1) |         \
                                     PIN_PUPDR_PULLUP(GPIOA_MISO1) |         \
                                     PIN_PUPDR_FLOATING(GPIOA_MOSI1) |  \
                                     PIN_PUPDR_PULLUP(GPIOA_RCC_MCO_1) |    \
                                     PIN_PUPDR_FLOATING(GPIOA_TX1) |   \
                                     PIN_PUPDR_PULLUP(GPIOA_RX1) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_USB1_DN) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_USB1_DP) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_SWDIO) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_SWCLK) |      \
                                     PIN_PUPDR_PULLUP(GPIOA_TIM2CH1))
#define VAL_GPIOA_ODR               (PIN_ODR_HIGH(GPIOA_TX4) |             \
                                     PIN_ODR_HIGH(GPIOA_RX4) |     \
                                     PIN_ODR_HIGH(GPIOA_TX2) |        \
                                     PIN_ODR_HIGH(GPIOA_RX2) |             \
                                     PIN_ODR_HIGH(GPIOA_SPI1_CS) |             \
                                     PIN_ODR_HIGH(GPIOA_SCK1) |             \
                                     PIN_ODR_HIGH(GPIOA_MISO1) |             \
                                     PIN_ODR_HIGH(GPIOA_MOSI1) |      \
                                     PIN_ODR_HIGH(GPIOA_RCC_MCO_1) |          \
                                     PIN_ODR_HIGH(GPIOA_TX1) |         \
                                     PIN_ODR_HIGH(GPIOA_RX1) |           \
                                     PIN_ODR_HIGH(GPIOA_USB1_DN) |           \
                                     PIN_ODR_HIGH(GPIOA_USB1_DP) |           \
                                     PIN_ODR_HIGH(GPIOA_SWDIO) |            \
                                     PIN_ODR_HIGH(GPIOA_SWCLK) |            \
                                     PIN_ODR_HIGH(GPIOA_TIM2CH1))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_TX4, 8U) |          \
                                     PIN_AFIO_AF(GPIOA_RX4, 8U) | \
                                     PIN_AFIO_AF(GPIOA_TX2, 7U) |    \
                                     PIN_AFIO_AF(GPIOA_RX2, 7U) |          \
                                     PIN_AFIO_AF(GPIOA_SPI1_CS, 0U) |          \
                                     PIN_AFIO_AF(GPIOA_SCK1, 5U) |          \
                                     PIN_AFIO_AF(GPIOA_MISO1, 5U) |          \
                                     PIN_AFIO_AF(GPIOA_MOSI1, 5U))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_RCC_MCO_1, 0U) |      \
                                     PIN_AFIO_AF(GPIOA_TX1, 5U) |      \
                                     PIN_AFIO_AF(GPIOA_RX1, 5U) |       \
                                     PIN_AFIO_AF(GPIOA_USB1_DN, 10U) |       \
                                     PIN_AFIO_AF(GPIOA_USB1_DP, 10U) |       \
                                     PIN_AFIO_AF(GPIOA_SWDIO, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_SWCLK, 0U) |         \
                                     PIN_AFIO_AF(GPIOA_TIM2CH1, 1U))


/*
 * GPIOB setup:
 *
 * PB0  - GPIOB_PIN0                (input pullup).
 * PB1  - GPIOB_PIN1                (input pullup).
 * PB2  - GPIOB_QSPI_CLK            (input pullup).
 * PB3  - GPIOB_SPIx_CLK            (input pullup).
 * PB4  - GPIOB_SPIX_MISO           (input pullup).
 * PB5  - GPIOB_SENSOR3             (input pullup).
 * PB6  - GPIOB_QSPI_NCS            (input pullup).
 * PB7  - GPIOB_DCMI_VSYNC          (input pullup).
 * PB8  - GPIOB_I2C1_SCL            (alternate 4).
 * PB9  - GPIOB_I2C1_SDA            (alternate 4).
 * PB10 - GPIOB_I2C2_SCL            (alternate 4).
 * PB11 - GPIOB_I2C2_SDA            (alternate 4).
 * PB12 - GPIOB_RST_RFM             (output pushpull).
 * PB13 - GPIOB_SCK2                (alternate 5).
 * PB14 - GPIOB_MISO2               (alternate 5).
 * PB15 - GPIOB_MOSI2               (alternate 5).
 */
#define VAL_GPIOB_MODER             (PIN_MODE_INPUT(GPIOB_PIN0) |          \
                                     PIN_MODE_INPUT(GPIOB_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOB_QSPI_CLK) |           \
                                     PIN_MODE_INPUT(GPIOB_SPIx_CLK) |        \
                                     PIN_MODE_INPUT(GPIOB_SPIX_MISO) |           \
                                     PIN_MODE_INPUT(GPIOB_SENSOR3) |           \
                                     PIN_MODE_INPUT(GPIOB_QSPI_NCS) |           \
                                     PIN_MODE_OUTPUT(GPIOB_DCMI_VSYNC) |          \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SCL) |           \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C1_SDA) |           \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C2_SCL) |          \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C2_SDA) |          \
                                     PIN_MODE_OUTPUT(GPIOB_RST_RFM) |          \
                                     PIN_MODE_ALTERNATE(GPIOB_SCK2) |  \
                                     PIN_MODE_ALTERNATE(GPIOB_MISO2) |          \
                                     PIN_MODE_ALTERNATE(GPIOB_MOSI2))
#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_QSPI_CLK) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPIx_CLK) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPIX_MISO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SENSOR3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_QSPI_NCS) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_DCMI_VSYNC) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C1_SCL) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C1_SDA) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C2_SCL) |      \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C2_SDA) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_RST_RFM) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SCK2) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOB_MISO2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_MOSI2))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_HIGH(GPIOB_PIN0) |          \
                                     PIN_OSPEED_HIGH(GPIOB_PIN1) |       \
                                     PIN_OSPEED_HIGH(GPIOB_QSPI_CLK) |       \
                                     PIN_OSPEED_HIGH(GPIOB_SPIx_CLK) |           \
                                     PIN_OSPEED_HIGH(GPIOB_SPIX_MISO) |       \
                                     PIN_OSPEED_HIGH(GPIOB_SENSOR3) |       \
                                     PIN_OSPEED_HIGH(GPIOB_QSPI_NCS) |       \
                                     PIN_OSPEED_HIGH(GPIOB_DCMI_VSYNC) |          \
                                     PIN_OSPEED_HIGH(GPIOB_I2C1_SCL) |       \
                                     PIN_OSPEED_HIGH(GPIOB_I2C1_SDA) |       \
                                     PIN_OSPEED_HIGH(GPIOB_I2C2_SCL) |      \
                                     PIN_OSPEED_HIGH(GPIOB_I2C2_SDA) |      \
                                     PIN_OSPEED_HIGH(GPIOB_RST_RFM) |      \
                                     PIN_OSPEED_HIGH(GPIOB_SCK2) |     \
                                     PIN_OSPEED_HIGH(GPIOB_MISO2) |          \
                                     PIN_OSPEED_HIGH(GPIOB_MOSI2))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_PULLUP(GPIOB_PIN0) |       \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_QSPI_CLK) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_SPIx_CLK) |          \
                                     PIN_PUPDR_PULLUP(GPIOB_SPIX_MISO) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_SENSOR3) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_QSPI_NCS) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_DCMI_VSYNC) |       \
                                     PIN_PUPDR_PULLUP(GPIOB_I2C1_SCL) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_I2C1_SDA) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_I2C2_SCL) |        \
                                     PIN_PUPDR_PULLUP(GPIOB_I2C2_SDA) |        \
                                     PIN_PUPDR_PULLUP(GPIOB_RST_RFM) |        \
                                     PIN_PUPDR_PULLUP(GPIOB_SCK2) |    \
                                     PIN_PUPDR_FLOATING(GPIOB_MISO2) |       \
                                     PIN_PUPDR_PULLUP(GPIOB_MOSI2))
#define VAL_GPIOB_ODR               (PIN_ODR_HIGH(GPIOB_PIN0) |              \
                                     PIN_ODR_HIGH(GPIOB_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOB_QSPI_CLK) |             \
                                     PIN_ODR_HIGH(GPIOB_SPIx_CLK) |              \
                                     PIN_ODR_HIGH(GPIOB_SPIX_MISO) |             \
                                     PIN_ODR_HIGH(GPIOB_SENSOR3) |             \
                                     PIN_ODR_HIGH(GPIOB_QSPI_NCS) |             \
                                     PIN_ODR_HIGH(GPIOB_DCMI_VSYNC) |              \
                                     PIN_ODR_HIGH(GPIOB_I2C1_SCL) |             \
                                     PIN_ODR_HIGH(GPIOB_I2C1_SDA) |             \
                                     PIN_ODR_HIGH(GPIOB_I2C2_SCL) |            \
                                     PIN_ODR_HIGH(GPIOB_I2C2_SDA) |            \
                                     PIN_ODR_HIGH(GPIOB_RST_RFM) |            \
                                     PIN_ODR_HIGH(GPIOB_SCK2) |        \
                                     PIN_ODR_HIGH(GPIOB_MISO2) |              \
                                     PIN_ODR_HIGH(GPIOB_MOSI2))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_PIN0, 0U) |          \
                                     PIN_AFIO_AF(GPIOB_PIN1, 0U) |          \
                                     PIN_AFIO_AF(GPIOB_QSPI_CLK, 0U) |          \
                                     PIN_AFIO_AF(GPIOB_SPIx_CLK, 0U) |           \
                                     PIN_AFIO_AF(GPIOB_SPIX_MISO, 0U) |          \
                                     PIN_AFIO_AF(GPIOB_SENSOR3, 0U) |          \
                                     PIN_AFIO_AF(GPIOB_QSPI_NCS, 0U) |          \
                                     PIN_AFIO_AF(GPIOB_DCMI_VSYNC, 0U))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_I2C1_SCL, 4U) |          \
                                     PIN_AFIO_AF(GPIOB_I2C1_SDA, 4U) |          \
                                     PIN_AFIO_AF(GPIOB_I2C2_SCL, 4U) |         \
                                     PIN_AFIO_AF(GPIOB_I2C2_SDA, 4U) |         \
                                     PIN_AFIO_AF(GPIOB_RST_RFM, 0U) |         \
                                     PIN_AFIO_AF(GPIOB_SCK2, 5U) |    \
                                     PIN_AFIO_AF(GPIOB_MISO2, 5U) |          \
                                     PIN_AFIO_AF(GPIOB_MOSI2, 5U))


/*
 * GPIOC setup:
 *
 * PC0  - GPIOC_SPI1_CS2            (output pushpull).
 * PC1  - GPIOC_4_20mA_1            (analog).
 * PC2  - GPIOC_D_C                 (output pushpull).
 * PC3  - GPIOC_4_20mA_2            (analog).
 * PC4  - GPIOC_VPILOT              (analog).
 * PC5  - GPIOC_PIN5                (input pullup).
 * PC6  - GPIOC_DIO0                (input pullup).
 * PC7  - GPIOC_SENSOR4             (input pullup).
 * PC8  - GPIOC_SDMMC1_D0           (input pullup).
 * PC9  - GPIOC_SDMMC1_D1           (input pullup).
 * PC10 - GPIOC_SDMMC1_D2           (input pullup).
 * PC11 - GPIOC_SDMMC1_D3           (input pullup).
 * PC12 - GPIOC_SDMMC1_CK           (input pullup).
 * PC13 - GPIOC_SW2                 (input pullup).
 * PC14 - GPIOC_OSC32_IN            (input floating).
 * PC15 - GPIOC_OSC32_OUT           (input floating).
 */
#define VAL_GPIOC_MODER             (PIN_MODE_OUTPUT(GPIOC_SPI1_CS2) |           \
                                     PIN_MODE_ANALOG(GPIOC_4_20mA_1) |   \
                                     PIN_MODE_INPUT(GPIOC_D_C) |           \
                                     PIN_MODE_ANALOG(GPIOC_4_20mA_2) |           \
                                     PIN_MODE_ANALOG(GPIOC_VPILOT) |  \
                                     PIN_MODE_INPUT(GPIOC_PIN5) |  \
                                     PIN_MODE_INPUT(GPIOC_DIO0) |           \
                                     PIN_MODE_INPUT(GPIOC_SENSOR4) |           \
                                     PIN_MODE_INPUT(GPIOC_SDMMC1_D0) |           \
                                     PIN_MODE_INPUT(GPIOC_SDMMC1_D1) |           \
                                     PIN_MODE_INPUT(GPIOC_SDMMC1_D2) |          \
                                     PIN_MODE_INPUT(GPIOC_SDMMC1_D3) |          \
                                     PIN_MODE_INPUT(GPIOC_SDMMC1_CK) |          \
                                     PIN_MODE_INPUT(GPIOC_SW2) |         \
                                     PIN_MODE_INPUT(GPIOC_OSC32_IN) |       \
                                     PIN_MODE_INPUT(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_SPI1_CS2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_4_20mA_1) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_D_C) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_4_20mA_2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_VPILOT) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_PIN5) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOC_DIO0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SENSOR4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDMMC1_D0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDMMC1_D1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDMMC1_D2) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDMMC1_D3) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDMMC1_CK) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SW2) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN) |   \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_HIGH(GPIOC_SPI1_CS2) |       \
                                     PIN_OSPEED_HIGH(GPIOC_4_20mA_1) |      \
                                     PIN_OSPEED_HIGH(GPIOC_D_C) |       \
                                     PIN_OSPEED_HIGH(GPIOC_4_20mA_2) |       \
                                     PIN_OSPEED_HIGH(GPIOC_VPILOT) |     \
                                     PIN_OSPEED_HIGH(GPIOC_PIN5) |     \
                                     PIN_OSPEED_HIGH(GPIOC_DIO0) |       \
                                     PIN_OSPEED_HIGH(GPIOC_SENSOR4) |       \
                                     PIN_OSPEED_HIGH(GPIOC_SDMMC1_D0) |       \
                                     PIN_OSPEED_HIGH(GPIOC_SDMMC1_D1) |       \
                                     PIN_OSPEED_HIGH(GPIOC_SDMMC1_D2) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SDMMC1_D3) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SDMMC1_CK) |      \
                                     PIN_OSPEED_HIGH(GPIOC_SW2) |        \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_IN) |   \
                                     PIN_OSPEED_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_PULLUP(GPIOC_SPI1_CS2) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_4_20mA_1) |   \
                                     PIN_PUPDR_PULLUP(GPIOC_D_C) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_4_20mA_2) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_VPILOT) |  \
                                     PIN_PUPDR_PULLUP(GPIOC_PIN5) |  \
                                     PIN_PUPDR_PULLUP(GPIOC_DIO0) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_SENSOR4) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_SDMMC1_D0) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_SDMMC1_D1) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_SDMMC1_D2) |        \
                                     PIN_PUPDR_PULLUP(GPIOC_SDMMC1_D3) |        \
                                     PIN_PUPDR_PULLUP(GPIOC_SDMMC1_CK) |        \
                                     PIN_PUPDR_PULLDOWN(GPIOC_SW2) |     \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_IN) |   \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))
#define VAL_GPIOC_ODR               (PIN_ODR_HIGH(GPIOC_SPI1_CS2) |             \
                                     PIN_ODR_HIGH(GPIOC_4_20mA_1) |         \
                                     PIN_ODR_HIGH(GPIOC_D_C) |             \
                                     PIN_ODR_HIGH(GPIOC_4_20mA_2) |             \
                                     PIN_ODR_HIGH(GPIOC_VPILOT) |        \
                                     PIN_ODR_HIGH(GPIOC_PIN5) |        \
                                     PIN_ODR_HIGH(GPIOC_DIO0) |             \
                                     PIN_ODR_HIGH(GPIOC_SENSOR4) |             \
                                     PIN_ODR_HIGH(GPIOC_SDMMC1_D0) |             \
                                     PIN_ODR_HIGH(GPIOC_SDMMC1_D1) |             \
                                     PIN_ODR_HIGH(GPIOC_SDMMC1_D2) |            \
                                     PIN_ODR_HIGH(GPIOC_SDMMC1_D3) |            \
                                     PIN_ODR_HIGH(GPIOC_SDMMC1_CK) |            \
                                     PIN_ODR_HIGH(GPIOC_SW2) |           \
                                     PIN_ODR_HIGH(GPIOC_OSC32_IN) |         \
                                     PIN_ODR_HIGH(GPIOC_OSC32_OUT))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_SPI1_CS2, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_4_20mA_1, 0U) |     \
                                     PIN_AFIO_AF(GPIOC_D_C, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_4_20mA_2, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_VPILOT, 0U) |    \
                                     PIN_AFIO_AF(GPIOC_PIN5, 0U) |    \
                                     PIN_AFIO_AF(GPIOC_DIO0, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_SENSOR4, 0U))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_SDMMC1_D0, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_SDMMC1_D1, 0U) |          \
                                     PIN_AFIO_AF(GPIOC_SDMMC1_D2, 0U) |         \
                                     PIN_AFIO_AF(GPIOC_SDMMC1_D3, 0U) |         \
                                     PIN_AFIO_AF(GPIOC_SDMMC1_CK, 0U) |         \
                                     PIN_AFIO_AF(GPIOC_SW2, 0U) |        \
                                     PIN_AFIO_AF(GPIOC_OSC32_IN, 0U) |      \
                                     PIN_AFIO_AF(GPIOC_OSC32_OUT, 0U))


/*
 * GPIOD setup:
 *
 * PD0  - GPIOD_CAN1RX                (alternate 9).
 * PD1  - GPIOD_CAN1TX                (alternate 9).
 * PD2  - GPIOD_SDMMC1_CMD            (input pullup).
 * PD3  - GPIOD_DCMI_D5               (input pullup).
 * PD4  - GPIOD_SENSOR1               (input pullup).
 * PD5  - GPIOD_SENSOR2               (input pullup).
 * PD6  - GPIOD_SPIx_CS               (output pushpull).
 * PD7  - GPIOD_SPIx_MOSI             (alternate 5).
 * PD8  - GPIOD_RELE1                 (output pushpull).
 * PD9  - GPIOD_RELE2                 (output pushpull).
 * PD10 - GPIOD_RELE3                 (output pushpull).
 * PD11 - GPIOD_QSPI_IO0              (input pullup).
 * PD12 - GPIOD_QSPI_IO1              (input pullup).
 * PD13 - GPIOD_QSPI_IO3              (input pullup).
 * PD14 - GPIOD_RELE4                 (output pushpull).
 * PD15 - GPIOD_NSS                   (output pushpull).
 */
#define VAL_GPIOD_MODER             (PIN_MODE_ALTERNATE(GPIOD_CAN1RX) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_CAN1TX) |           \
                                     PIN_MODE_INPUT(GPIOD_SDMMC1_CMD) |           \
                                     PIN_MODE_INPUT(GPIOD_DCMI_D5) |           \
                                     PIN_MODE_INPUT(GPIOD_SENSOR1) |           \
                                     PIN_MODE_INPUT(GPIOD_SENSOR2) |           \
                                     PIN_MODE_OUTPUT(GPIOD_SPIx_CS) |           \
                                     PIN_MODE_ALTERNATE(GPIOD_SPIx_MOSI) |           \
                                     PIN_MODE_OUTPUT(GPIOD_RELE1) |  \
                                     PIN_MODE_OUTPUT(GPIOD_RELE2) |  \
                                     PIN_MODE_OUTPUT(GPIOD_RELE3) |          \
                                     PIN_MODE_INPUT(GPIOD_QSPI_IO0) |          \
                                     PIN_MODE_INPUT(GPIOD_QSPI_IO1) |          \
                                     PIN_MODE_INPUT(GPIOD_QSPI_IO3) |          \
                                     PIN_MODE_OUTPUT(GPIOD_RELE4) |          \
                                     PIN_MODE_OUTPUT(GPIOD_NSS))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_CAN1RX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CAN1TX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SDMMC1_CMD) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_DCMI_D5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SENSOR1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SENSOR2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SPIx_CS) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SPIx_MOSI) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RELE1) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RELE2) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RELE3) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_QSPI_IO0) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_QSPI_IO1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_QSPI_IO3) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_RELE4) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_NSS))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_HIGH(GPIOD_CAN1RX) |       \
                                     PIN_OSPEED_HIGH(GPIOD_CAN1TX) |       \
                                     PIN_OSPEED_HIGH(GPIOD_SDMMC1_CMD) |       \
                                     PIN_OSPEED_HIGH(GPIOD_DCMI_D5) |       \
                                     PIN_OSPEED_VERYLOW(GPIOD_SENSOR1) |       \
                                     PIN_OSPEED_VERYLOW(GPIOD_SENSOR2) |       \
                                     PIN_OSPEED_HIGH(GPIOD_SPIx_CS) |       \
                                     PIN_OSPEED_HIGH(GPIOD_SPIx_MOSI) |       \
                                     PIN_OSPEED_VERYLOW(GPIOD_RELE1) |     \
                                     PIN_OSPEED_VERYLOW(GPIOD_RELE2) |     \
                                     PIN_OSPEED_VERYLOW(GPIOD_RELE3) |      \
                                     PIN_OSPEED_HIGH(GPIOD_QSPI_IO0) |      \
                                     PIN_OSPEED_HIGH(GPIOD_QSPI_IO1) |      \
                                     PIN_OSPEED_HIGH(GPIOD_QSPI_IO3) |      \
                                     PIN_OSPEED_VERYLOW(GPIOD_RELE4) |      \
                                     PIN_OSPEED_HIGH(GPIOD_NSS))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_PULLUP(GPIOD_CAN1RX) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_CAN1TX) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_SDMMC1_CMD) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_DCMI_D5) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_SENSOR1) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_SENSOR2) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_SPIx_CS) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_SPIx_MOSI) |         \
                                     PIN_PUPDR_PULLDOWN(GPIOD_RELE1) |  \
                                     PIN_PUPDR_PULLDOWN(GPIOD_RELE2) |  \
                                     PIN_PUPDR_PULLDOWN(GPIOD_RELE3) |        \
                                     PIN_PUPDR_PULLUP(GPIOD_QSPI_IO0) |        \
                                     PIN_PUPDR_PULLUP(GPIOD_QSPI_IO1) |        \
                                     PIN_PUPDR_PULLUP(GPIOD_QSPI_IO3) |        \
                                     PIN_PUPDR_PULLDOWN(GPIOD_RELE4) |        \
                                     PIN_PUPDR_PULLUP(GPIOD_NSS))
#define VAL_GPIOD_ODR               (PIN_ODR_HIGH(GPIOD_CAN1RX) |             \
                                     PIN_ODR_HIGH(GPIOD_CAN1TX) |             \
                                     PIN_ODR_HIGH(GPIOD_SDMMC1_CMD) |             \
                                     PIN_ODR_HIGH(GPIOD_DCMI_D5) |             \
                                     PIN_ODR_HIGH(GPIOD_SENSOR1) |             \
                                     PIN_ODR_HIGH(GPIOD_SENSOR2) |             \
                                     PIN_ODR_HIGH(GPIOD_SPIx_CS) |             \
                                     PIN_ODR_HIGH(GPIOD_SPIx_MOSI) |             \
                                     PIN_ODR_LOW(GPIOD_RELE1) |        \
                                     PIN_ODR_LOW(GPIOD_RELE2) |        \
                                     PIN_ODR_LOW(GPIOD_RELE3) |            \
                                     PIN_ODR_HIGH(GPIOD_QSPI_IO0) |            \
                                     PIN_ODR_HIGH(GPIOD_QSPI_IO1) |            \
                                     PIN_ODR_HIGH(GPIOD_QSPI_IO3) |            \
                                     PIN_ODR_LOW(GPIOD_RELE4) |            \
                                     PIN_ODR_HIGH(GPIOD_NSS))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_CAN1RX, 9U) |          \
                                     PIN_AFIO_AF(GPIOD_CAN1TX, 9U) |          \
                                     PIN_AFIO_AF(GPIOD_SDMMC1_CMD, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_DCMI_D5, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_SENSOR1, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_SENSOR2, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_SPIx_CS, 0U) |          \
                                     PIN_AFIO_AF(GPIOD_SPIx_MOSI, 5U))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_RELE1, 0U) |     \
                                     PIN_AFIO_AF(GPIOD_RELE2, 0U) |     \
                                     PIN_AFIO_AF(GPIOD_RELE3, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_QSPI_IO0, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_QSPI_IO1, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_QSPI_IO3, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_RELE4, 0U) |         \
                                     PIN_AFIO_AF(GPIOD_NSS, 0U))


/*
 * GPIOE setup:
 *
 * PE0  - GPIOE_CANSILENT            (output pushpull).
 * PE1  - GPIOE_TXRX                 (output pushpull).
 * PE2  - GPIOE_QSPI_IO2             (input pullup).
 * PE3  - GPIOE_LED                  (output pushpull).
 * PE4  - GPIOE_RESETCON             (output pushpull).
 * PE5  - GPIOE_DCMI_D6              (input pullup).
 * PE6  - GPIOE_IRQCON               (input pullup).
 * PE7  - GPIOE_RX7                  (alternate 7).
 * PE8  - GPIOE_TX7                  (alternate 7).
 * PE9  - GPIOE_ENHM10               (output pushpull).
 * PE10 - GPIOE_LCD_LED              (output pushpull).
 * PE11 - GPIOE_LCD_CS               (output pushpull).
 * PE12 - GPIOE_LCD_SCL              (alternate 5).
 * PE13 - GPIOE_LCD_WR_RS            (output pushpull).
 * PE14 - GPIOE_LCD_SDA              (alternate 5).
 * PE15 - GPIOE_STHM10               (input pullup).
 */
#define VAL_GPIOE_MODER             (PIN_MODE_OUTPUT(GPIOE_CANSILENT) |           \
                                     PIN_MODE_OUTPUT(GPIOE_TXRX) |           \
                                     PIN_MODE_INPUT(GPIOE_QSPI_IO2) |           \
                                     PIN_MODE_OUTPUT(GPIOE_LED) |           \
                                     PIN_MODE_OUTPUT(GPIOE_RESETCON) |           \
                                     PIN_MODE_INPUT(GPIOE_DCMI_D6) |           \
                                     PIN_MODE_INPUT(GPIOE_IRQCON) |           \
                                     PIN_MODE_ALTERNATE(GPIOE_RX7) |           \
                                     PIN_MODE_ALTERNATE(GPIOE_TX7) |           \
                                     PIN_MODE_OUTPUT(GPIOE_ENHM10) |           \
                                     PIN_MODE_OUTPUT(GPIOE_LCD_LED) |          \
                                     PIN_MODE_OUTPUT(GPIOE_LCD_CS) |          \
                                     PIN_MODE_ALTERNATE(GPIOE_LCD_SCL) |          \
                                     PIN_MODE_OUTPUT(GPIOE_LCD_WR_RS) |          \
                                     PIN_MODE_ALTERNATE(GPIOE_LCD_SDA) |          \
                                     PIN_MODE_INPUT(GPIOE_STHM10))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_CANSILENT) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TXRX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_QSPI_IO2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_RESETCON) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_DCMI_D6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_IRQCON) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_RX7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_TX7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_ENHM10) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LCD_LED) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LCD_CS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LCD_SCL) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LCD_WR_RS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LCD_SDA) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_STHM10))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_VERYLOW(GPIOE_CANSILENT) |       \
                                     PIN_OSPEED_HIGH(GPIOE_TXRX) |       \
                                     PIN_OSPEED_HIGH(GPIOE_QSPI_IO2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOE_LED) |       \
                                     PIN_OSPEED_VERYLOW(GPIOE_RESETCON) |       \
                                     PIN_OSPEED_HIGH(GPIOE_DCMI_D6) |       \
                                     PIN_OSPEED_HIGH(GPIOE_IRQCON) |       \
                                     PIN_OSPEED_HIGH(GPIOE_RX7) |       \
                                     PIN_OSPEED_HIGH(GPIOE_TX7) |       \
                                     PIN_OSPEED_VERYLOW(GPIOE_ENHM10) |       \
                                     PIN_OSPEED_VERYLOW(GPIOE_LCD_LED) |      \
                                     PIN_OSPEED_HIGH(GPIOE_LCD_CS) |      \
                                     PIN_OSPEED_HIGH(GPIOE_LCD_SCL) |      \
                                     PIN_OSPEED_HIGH(GPIOE_LCD_WR_RS) |      \
                                     PIN_OSPEED_HIGH(GPIOE_LCD_SDA) |      \
                                     PIN_OSPEED_VERYLOW(GPIOE_STHM10))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_PULLUP(GPIOE_CANSILENT) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_TXRX) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_QSPI_IO2) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_LED) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_RESETCON) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_DCMI_D6) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_IRQCON) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_RX7) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_TX7) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_ENHM10) |         \
                                     PIN_PUPDR_PULLUP(GPIOE_LCD_LED) |        \
                                     PIN_PUPDR_PULLUP(GPIOE_LCD_CS) |        \
                                     PIN_PUPDR_PULLUP(GPIOE_LCD_SCL) |        \
                                     PIN_PUPDR_PULLUP(GPIOE_LCD_WR_RS) |        \
                                     PIN_PUPDR_PULLUP(GPIOE_LCD_SDA) |        \
                                     PIN_PUPDR_PULLUP(GPIOE_STHM10))
#define VAL_GPIOE_ODR               (PIN_ODR_HIGH(GPIOE_CANSILENT) |             \
                                     PIN_ODR_HIGH(GPIOE_TXRX) |             \
                                     PIN_ODR_HIGH(GPIOE_QSPI_IO2) |             \
                                     PIN_ODR_HIGH(GPIOE_LED) |             \
                                     PIN_ODR_HIGH(GPIOE_RESETCON) |             \
                                     PIN_ODR_HIGH(GPIOE_DCMI_D6) |             \
                                     PIN_ODR_HIGH(GPIOE_IRQCON) |             \
                                     PIN_ODR_HIGH(GPIOE_RX7) |             \
                                     PIN_ODR_HIGH(GPIOE_TX7) |             \
                                     PIN_ODR_HIGH(GPIOE_ENHM10) |             \
                                     PIN_ODR_LOW(GPIOE_LCD_LED) |            \
                                     PIN_ODR_HIGH(GPIOE_LCD_CS) |            \
                                     PIN_ODR_HIGH(GPIOE_LCD_SCL) |            \
                                     PIN_ODR_HIGH(GPIOE_LCD_WR_RS) |            \
                                     PIN_ODR_HIGH(GPIOE_LCD_SDA) |            \
                                     PIN_ODR_HIGH(GPIOE_STHM10))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_CANSILENT, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_TXRX, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_QSPI_IO2, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_LED, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_RESETCON, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_DCMI_D6, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_IRQCON, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_RX7, 7U))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_TX7, 7U) |          \
                                     PIN_AFIO_AF(GPIOE_ENHM10, 0U) |          \
                                     PIN_AFIO_AF(GPIOE_LCD_LED, 0U) |         \
                                     PIN_AFIO_AF(GPIOE_LCD_CS, 0U) |         \
                                     PIN_AFIO_AF(GPIOE_LCD_SCL, 5U) |         \
                                     PIN_AFIO_AF(GPIOE_LCD_WR_RS, 0U) |         \
                                     PIN_AFIO_AF(GPIOE_LCD_SDA, 5U) |         \
                                     PIN_AFIO_AF(GPIOE_STHM10, 0U))

/*
 * GPIOF setup:
 *
 * PF0  - PIN0                      (input pullup).
 * PF1  - PIN1                      (input pullup).
 * PF2  - PIN2                      (input pullup).
 * PF3  - PIN3                      (input pullup).
 * PF4  - PIN4                      (input pullup).
 * PF5  - PIN5                      (input pullup).
 * PF6  - PIN6                      (input pullup).
 * PF7  - PIN7                      (input pullup).
 * PF8  - PIN8                      (input pullup).
 * PF9  - PIN9                      (input pullup).
 * PF10 - PIN10                     (input pullup).
 * PF11 - PIN11                     (input pullup).
 * PF12 - PIN12                     (input pullup).
 * PF13 - PIN13                     (input pullup).
 * PF14 - PIN14                     (input pullup).
 * PF15 - PIN15                     (input pullup).
 */
#define VAL_GPIOF_MODER             (PIN_MODE_INPUT(GPIOF_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOF_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN15))
#define VAL_GPIOF_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOF_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN15))
#define VAL_GPIOF_OSPEEDR           (PIN_OSPEED_VERYLOW(GPIOF_PIN0) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN1) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN3) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN4) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN5) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN6) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN7) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN10) |      \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN11) |      \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN12) |      \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN13) |      \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOF_PIN15))
#define VAL_GPIOF_PUPDR             (PIN_PUPDR_PULLUP(GPIOF_PIN0) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN5) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN15))
#define VAL_GPIOF_ODR               (PIN_ODR_HIGH(GPIOF_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOF_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN15))
#define VAL_GPIOF_AFRL              (PIN_AFIO_AF(GPIOF_PIN0, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN1, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN5, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN6, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN7, 0U))
#define VAL_GPIOF_AFRH              (PIN_AFIO_AF(GPIOF_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOF_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOF_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOF_PIN12, 0U) |         \
                                     PIN_AFIO_AF(GPIOF_PIN13, 0U) |         \
                                     PIN_AFIO_AF(GPIOF_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOF_PIN15, 0U))

/*
 * GPIOG setup:
 *
 * PG0  - PIN0                      (input pullup).
 * PG1  - PIN1                      (input pullup).
 * PG2  - PIN2                      (input pullup).
 * PG3  - PIN3                      (input pullup).
 * PG4  - PIN4                      (input pullup).
 * PG5  - PIN5                      (input pullup).
 * PG6  - USB_FS_PWR_EN             (output pushpull minimum).
 * PG7  - USB_FS_OVCR               (input floating).
 * PG8  - PIN8                      (input pullup).
 * PG9  - PIN9                      (input pullup).
 * PG10 - PIN10                     (input pullup).
 * PG11 - RMII_TX_EN                (alternate 11).
 * PG12 - PIN12                     (input pullup).
 * PG13 - RMII_TXD0                 (alternate 11).
 * PG14 - PIN14                     (input pullup).
 * PG15 - PIN15                     (input pullup).
 */
#define VAL_GPIOG_MODER             (PIN_MODE_INPUT(GPIOG_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN5) |           \
                                     PIN_MODE_OUTPUT(GPIOG_USB_FS_PWR_EN) | \
                                     PIN_MODE_INPUT(GPIOG_USB_FS_OVCR) |    \
                                     PIN_MODE_INPUT(GPIOG_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN10) |          \
                                     PIN_MODE_ALTERNATE(GPIOG_RMII_TX_EN) | \
                                     PIN_MODE_INPUT(GPIOG_PIN12) |          \
                                     PIN_MODE_ALTERNATE(GPIOG_RMII_TXD0) |  \
                                     PIN_MODE_INPUT(GPIOG_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOG_PIN15))
#define VAL_GPIOG_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOG_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_USB_FS_PWR_EN) |\
                                     PIN_OTYPE_PUSHPULL(GPIOG_USB_FS_OVCR) |\
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RMII_TX_EN) | \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_RMII_TXD0) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN15))
#define VAL_GPIOG_OSPEEDR           (PIN_OSPEED_VERYLOW(GPIOG_PIN0) |       \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN1) |       \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN3) |       \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN4) |       \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN5) |       \
                                     PIN_OSPEED_VERYLOW(GPIOG_USB_FS_PWR_EN) |\
                                     PIN_OSPEED_VERYLOW(GPIOG_USB_FS_OVCR) |\
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN10) |      \
                                     PIN_OSPEED_HIGH(GPIOG_RMII_TX_EN) |    \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN12) |      \
                                     PIN_OSPEED_HIGH(GPIOG_RMII_TXD0) |     \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOG_PIN15))
#define VAL_GPIOG_PUPDR             (PIN_PUPDR_PULLUP(GPIOG_PIN0) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN5) |         \
                                     PIN_PUPDR_FLOATING(GPIOG_USB_FS_PWR_EN) |\
                                     PIN_PUPDR_FLOATING(GPIOG_USB_FS_OVCR) |\
                                     PIN_PUPDR_PULLUP(GPIOG_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN10) |        \
                                     PIN_PUPDR_FLOATING(GPIOG_RMII_TX_EN) | \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN12) |        \
                                     PIN_PUPDR_FLOATING(GPIOG_RMII_TXD0) |  \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN15))
#define VAL_GPIOG_ODR               (PIN_ODR_HIGH(GPIOG_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN5) |             \
                                     PIN_ODR_LOW(GPIOG_USB_FS_PWR_EN) |     \
                                     PIN_ODR_HIGH(GPIOG_USB_FS_OVCR) |      \
                                     PIN_ODR_HIGH(GPIOG_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOG_RMII_TX_EN) |       \
                                     PIN_ODR_HIGH(GPIOG_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOG_RMII_TXD0) |        \
                                     PIN_ODR_HIGH(GPIOG_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOG_PIN15))
#define VAL_GPIOG_AFRL              (PIN_AFIO_AF(GPIOG_PIN0, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_PIN1, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_PIN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_PIN5, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_USB_FS_PWR_EN, 0U) | \
                                     PIN_AFIO_AF(GPIOG_USB_FS_OVCR, 0U))
#define VAL_GPIOG_AFRH              (PIN_AFIO_AF(GPIOG_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOG_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOG_RMII_TX_EN, 11U) |   \
                                     PIN_AFIO_AF(GPIOG_PIN12, 0U) |         \
                                     PIN_AFIO_AF(GPIOG_RMII_TXD0, 11U) |    \
                                     PIN_AFIO_AF(GPIOG_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOG_PIN15, 0U))

/*
 * GPIOH setup:
 *
 * PH0  - OSC_IN                    (input floating).
 * PH1  - OSC_OUT                   (input floating).
 * PH2  - PIN2                      (input pullup).
 * PH3  - PIN3                      (input pullup).
 * PH4  - PIN4                      (input pullup).
 * PH5  - PIN5                      (input pullup).
 * PH6  - PIN6                      (input pullup).
 * PH7  - PIN7                      (input pullup).
 * PH8  - PIN8                      (input pullup).
 * PH9  - PIN9                      (input pullup).
 * PH10 - PIN10                     (input pullup).
 * PH11 - PIN11                     (input pullup).
 * PH12 - PIN12                     (input pullup).
 * PH13 - PIN13                     (input pullup).
 * PH14 - PIN14                     (input pullup).
 * PH15 - PIN15                     (input pullup).
 */
#define VAL_GPIOH_MODER             (PIN_MODE_INPUT(GPIOH_OSC_IN) |         \
                                     PIN_MODE_INPUT(GPIOH_OSC_OUT) |        \
                                     PIN_MODE_INPUT(GPIOH_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN15))
#define VAL_GPIOH_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN15))
#define VAL_GPIOH_OSPEEDR           (PIN_OSPEED_HIGH(GPIOH_OSC_IN) |        \
                                     PIN_OSPEED_HIGH(GPIOH_OSC_OUT) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN3) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN4) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN5) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN6) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN7) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN10) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN11) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN12) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN13) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOH_PIN15))
#define VAL_GPIOH_PUPDR             (PIN_PUPDR_FLOATING(GPIOH_OSC_IN) |     \
                                     PIN_PUPDR_FLOATING(GPIOH_OSC_OUT) |    \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN5) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN15))
#define VAL_GPIOH_ODR               (PIN_ODR_HIGH(GPIOH_OSC_IN) |           \
                                     PIN_ODR_HIGH(GPIOH_OSC_OUT) |          \
                                     PIN_ODR_HIGH(GPIOH_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN15))
#define VAL_GPIOH_AFRL              (PIN_AFIO_AF(GPIOH_OSC_IN, 0U) |        \
                                     PIN_AFIO_AF(GPIOH_OSC_OUT, 0U) |       \
                                     PIN_AFIO_AF(GPIOH_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN5, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN6, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN7, 0U))
#define VAL_GPIOH_AFRH              (PIN_AFIO_AF(GPIOH_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOH_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN12, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN13, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOH_PIN15, 0U))

/*
 * GPIOI setup:
 *
 * PI0  - PIN0                      (input pullup).
 * PI1  - PIN1                      (input pullup).
 * PI2  - PIN2                      (input pullup).
 * PI3  - PIN3                      (input pullup).
 * PI4  - PIN4                      (input pullup).
 * PI5  - PIN5                      (input pullup).
 * PI6  - PIN6                      (input pullup).
 * PI7  - PIN7                      (input pullup).
 * PI8  - PIN8                      (input pullup).
 * PI9  - PIN9                      (input pullup).
 * PI10 - PIN10                     (input pullup).
 * PI11 - PIN11                     (input pullup).
 * PI12 - PIN12                     (input pullup).
 * PI13 - PIN13                     (input pullup).
 * PI14 - PIN14                     (input pullup).
 * PI15 - PIN15                     (input pullup).
 */
#define VAL_GPIOI_MODER             (PIN_MODE_INPUT(GPIOI_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN15))
#define VAL_GPIOI_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOI_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN15))
#define VAL_GPIOI_OSPEEDR           (PIN_OSPEED_VERYLOW(GPIOI_PIN0) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN1) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN3) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN4) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN5) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN6) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN7) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN10) |      \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN11) |      \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN12) |      \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN13) |      \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOI_PIN15))
#define VAL_GPIOI_PUPDR             (PIN_PUPDR_PULLUP(GPIOI_PIN0) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN5) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN15))
#define VAL_GPIOI_ODR               (PIN_ODR_HIGH(GPIOI_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN15))
#define VAL_GPIOI_AFRL              (PIN_AFIO_AF(GPIOI_PIN0, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN1, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN5, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN6, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN7, 0U))
#define VAL_GPIOI_AFRH              (PIN_AFIO_AF(GPIOI_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOI_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_PIN12, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_PIN13, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOI_PIN15, 0U))

/*
 * GPIOJ setup:
 *
 * PJ0  - PIN0                      (input pullup).
 * PJ1  - PIN1                      (input pullup).
 * PJ2  - PIN2                      (input pullup).
 * PJ3  - PIN3                      (input pullup).
 * PJ4  - PIN4                      (input pullup).
 * PJ5  - PIN5                      (input pullup).
 * PJ6  - PIN6                      (input pullup).
 * PJ7  - PIN7                      (input pullup).
 * PJ8  - PIN8                      (input pullup).
 * PJ9  - PIN9                      (input pullup).
 * PJ10 - PIN10                     (input pullup).
 * PJ11 - PIN11                     (input pullup).
 * PJ12 - PIN12                     (input pullup).
 * PJ13 - PIN13                     (input pullup).
 * PJ14 - PIN14                     (input pullup).
 * PJ15 - PIN15                     (input pullup).
 */
#define VAL_GPIOJ_MODER             (PIN_MODE_INPUT(GPIOJ_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOJ_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOJ_PIN15))
#define VAL_GPIOJ_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOJ_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOJ_PIN15))
#define VAL_GPIOJ_OSPEEDR           (PIN_OSPEED_VERYLOW(GPIOJ_PIN0) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN1) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN3) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN4) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN5) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN6) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN7) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN10) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN11) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN12) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN13) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOJ_PIN15))
#define VAL_GPIOJ_PUPDR             (PIN_PUPDR_PULLUP(GPIOJ_PIN0) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN5) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOJ_PIN15))
#define VAL_GPIOJ_ODR               (PIN_ODR_HIGH(GPIOJ_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOJ_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOJ_PIN15))
#define VAL_GPIOJ_AFRL              (PIN_AFIO_AF(GPIOJ_PIN0, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN1, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN5, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN6, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN7, 0U))
#define VAL_GPIOJ_AFRH              (PIN_AFIO_AF(GPIOJ_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOJ_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN12, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN13, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOJ_PIN15, 0U))

/*
 * GPIOK setup:
 *
 * PK0  - PIN0                      (input pullup).
 * PK1  - PIN1                      (input pullup).
 * PK2  - PIN2                      (input pullup).
 * PK3  - PIN3                      (input pullup).
 * PK4  - PIN4                      (input pullup).
 * PK5  - PIN5                      (input pullup).
 * PK6  - PIN6                      (input pullup).
 * PK7  - PIN7                      (input pullup).
 * PK8  - PIN8                      (input pullup).
 * PK9  - PIN9                      (input pullup).
 * PK10 - PIN10                     (input pullup).
 * PK11 - PIN11                     (input pullup).
 * PK12 - PIN12                     (input pullup).
 * PK13 - PIN13                     (input pullup).
 * PK14 - PIN14                     (input pullup).
 * PK15 - PIN15                     (input pullup).
 */
#define VAL_GPIOK_MODER             (PIN_MODE_INPUT(GPIOK_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOK_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOK_PIN15))
#define VAL_GPIOK_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOK_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOK_PIN15))
#define VAL_GPIOK_OSPEEDR           (PIN_OSPEED_VERYLOW(GPIOK_PIN0) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN1) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN2) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN3) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN4) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN5) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN6) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN7) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN8) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN9) |       \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN10) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN11) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN12) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN13) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN14) |      \
                                     PIN_OSPEED_VERYLOW(GPIOK_PIN15))
#define VAL_GPIOK_PUPDR             (PIN_PUPDR_PULLUP(GPIOK_PIN0) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN1) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN2) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN3) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN4) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN5) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN6) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN7) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN8) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN9) |         \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN13) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN14) |        \
                                     PIN_PUPDR_PULLUP(GPIOK_PIN15))
#define VAL_GPIOK_ODR               (PIN_ODR_HIGH(GPIOK_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOK_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOK_PIN15))
#define VAL_GPIOK_AFRL              (PIN_AFIO_AF(GPIOK_PIN0, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN1, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN2, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN3, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN4, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN5, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN6, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN7, 0U))
#define VAL_GPIOK_AFRH              (PIN_AFIO_AF(GPIOK_PIN8, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN9, 0U) |          \
                                     PIN_AFIO_AF(GPIOK_PIN10, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN11, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN12, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN13, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN14, 0U) |         \
                                     PIN_AFIO_AF(GPIOK_PIN15, 0U))

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* BOARD_H */
