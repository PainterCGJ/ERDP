#ifndef __BOARD_H__
#define __BOARD_H__

#define STM32_BOARD 1
#define GD32_BOARD  2
#define BOARD_TYPE  GD32_BOARD

#if BOARD_TYPE == STM32_BOARD
/*
 * System UART Configuration
 */
#define SYS_COM_CFG \
    {ERDP_UART1, 115200, ERDP_UART_TX_RX, ERDP_GPIOA, ERDP_GPIO_PIN_9, 7, ERDP_GPIOA, ERDP_GPIO_PIN_10, 7, 10}

/*
 * System LED Configuration
 */
#define SYS_LED_PIN  ERDP_GPIO_PIN_0
#define SYS_LED_PORT ERDP_GPIOC

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
