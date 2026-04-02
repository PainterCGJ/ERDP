#ifndef __BOARD_H__
#define __BOARD_H__

#define STM32_BOARD 1
#define GD32_BOARD  2
#define BOARD_TYPE  STM32_BOARD

#if BOARD_TYPE == STM32_BOARD
#include "stm32f4xx.h"
/*
 * System UART Configuration
 */
#define SYS_COM_CFG \
    {ERDP_UART1, 115200, ERDP_UART_TX_RX, ERDP_GPIOA, ERDP_GPIO_PIN_9, GPIO_AF_USART1,  ERDP_GPIOA, ERDP_GPIO_PIN_10, GPIO_AF_USART1, 10}

/*
 * System LED Configuration
 */
#define SYS_LED_PIN  ERDP_GPIO_PIN_0
#define SYS_LED_PORT ERDP_GPIOC

/*
 * SPI Configuration
 */
#define SPI_INFO                                                                                               \
    {                                                                                                          \
        ERDP_SPI1,  ERDP_GPIOA,      ERDP_GPIO_PIN_5, GPIO_AF_SPI1,  ERDP_GPIOA,      ERDP_GPIO_PIN_7, GPIO_AF_SPI1, \
        ERDP_GPIOA, ERDP_GPIO_PIN_6, GPIO_AF_SPI1,       ERDP_GPIOA, ERDP_GPIO_PIN_4, GPIO_AF_SPI1,                  \
    }

#define SPI_CONFIG {ERDP_SPI_CLKMODE_0, ERDP_SPI_ENDIAN_MSB, SPI_BaudRatePrescaler_32, 6}

#elif BOARD_TYPE == GD32_BOARD
#include "gd32f4xx.h"
/*
 * System UART Configuration
 */
#define SYS_COM_CFG                                                        \
    {ERDP_UART3, 115200,     ERDP_UART_TX_RX, ERDP_GPIOA, ERDP_GPIO_PIN_0, \
     GPIO_AF_8,  ERDP_GPIOA, ERDP_GPIO_PIN_1, GPIO_AF_8,  10}

/*
 * System LED Configuration
 */
#define SYS_LED_PORT ERDP_GPIOC
#define SYS_LED_PIN  ERDP_GPIO_PIN_13
#endif

#endif    // __BOARD_H__
