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
#define SYS_COM_CFG                                                                 \
    {ERDP_UART1,     115200,     ERDP_UART_TX_RX,  ERDP_GPIOA,     ERDP_GPIO_PIN_9, \
     GPIO_AF_USART1, ERDP_GPIOA, ERDP_GPIO_PIN_10, GPIO_AF_USART1, 10}

/*
 * System LED Configuration
 */
#define SYS_LED_PIN  ERDP_GPIO_PIN_0
#define SYS_LED_PORT ERDP_GPIOC

/*
 * SPI Configuration
 */
/* SPI1
 *  MODE: MASTER
 *  MOSI: GPIOA, GPIO_PIN_5, GPIO_AF_SPI1
 *  MISO: GPIOA, GPIO_PIN_7, GPIO_AF_SPI1
 *  CLK: GPIOA, GPIO_PIN_6, GPIO_AF_SPI1
 *  CS: GPIOA, GPIO_PIN_4, GPIO_AF_SPI1
 */
#define SPI_MASTER_INFO                                                                                             \
    {                                                                                                               \
        ERDP_SPI1,  ERDP_GPIOA,      ERDP_GPIO_PIN_5, GPIO_AF_SPI1, ERDP_GPIOA,      ERDP_GPIO_PIN_7, GPIO_AF_SPI1, \
        ERDP_GPIOA, ERDP_GPIO_PIN_6, GPIO_AF_SPI1,    ERDP_GPIOA,   ERDP_GPIO_PIN_4, GPIO_AF_SPI1,                  \
    }
/*                         clock mode,         endian,              baudrate,                pri tx_dma, rx_dma*/
#define SPI_MASTER_CONFIG {ERDP_SPI_CLKMODE_0, ERDP_SPI_ENDIAN_MSB, SPI_BaudRatePrescaler_32, 6, false, false}

/* SPI2
 *  MODE: SLAVE
 *  MOSI: GPIOB, GPIO_PIN_15, GPIO_AF_SPI2
 *  MISO: GPIOB, GPIO_PIN_14, GPIO_AF_SPI2
 *  CLK: GPIOB, GPIO_PIN_13, GPIO_AF_SPI2
 *  CS: GPIOB, GPIO_PIN_12, GPIO_AF_SPI2
 */
#define SPI_SLAVE_INFO                                                                                                  \
    {                                                                                                                   \
        ERDP_SPI2,  ERDP_GPIOB,       ERDP_GPIO_PIN_15, GPIO_AF_SPI2, ERDP_GPIOB,       ERDP_GPIO_PIN_14, GPIO_AF_SPI2, \
        ERDP_GPIOB, ERDP_GPIO_PIN_13, GPIO_AF_SPI2,     ERDP_GPIOB,   ERDP_GPIO_PIN_12, GPIO_AF_SPI2,                   \
    }
/*                         clock mode,         endian,              baudrate,                pri tx_dma, rx_dma*/
#define SPI_SLAVE_CONFIG {ERDP_SPI_CLKMODE_0, ERDP_SPI_ENDIAN_MSB, SPI_BaudRatePrescaler_32, 7, false, false}

/*
 * LCD Configuration
 */
/* SPI3
 *  MODE: MASTER
 *  MOSI: GPIOB, GPIO_PIN_5, GPIO_AF_SPI3
 *  MISO: GPIOB, GPIO_PIN_4, GPIO_AF_SPI3
 *  CLK: GPIOB, GPIO_PIN_3, GPIO_AF_SPI3
 *  CS: GPIOA, GPIO_PIN_15, GPIO_AF_SPI3
 */
#define LCD_SPI_INFO                                                                                                 \
    {                                                                                                                \
        ERDP_SPI3,  ERDP_GPIOB,      ERDP_GPIO_PIN_3, GPIO_AF_SPI3, ERDP_GPIOB,       ERDP_GPIO_PIN_5, GPIO_AF_SPI3, \
        ERDP_GPIOB, ERDP_GPIO_PIN_4, GPIO_AF_SPI3,    ERDP_GPIOA,   ERDP_GPIO_PIN_15, GPIO_AF_SPI3,                  \
    }

/*                         clock mode,         endian,              baudrate,                pri tx_dma, rx_dma*/
#define LCD_SPI_CONFIG {ERDP_SPI_CLKMODE_0, ERDP_SPI_ENDIAN_MSB, SPI_BaudRatePrescaler_32, 6, false, false}

#define LCD_DC_PORT ERDP_GPIOD
#define LCD_DC_PIN  ERDP_GPIO_PIN_13

#define LCD_BL_PORT ERDP_GPIOD
#define LCD_BL_PIN  ERDP_GPIO_PIN_12

#define LCD_RST_PORT ERDP_GPIOC
#define LCD_RST_PIN  ERDP_GPIO_PIN_2

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
