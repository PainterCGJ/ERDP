#ifndef __ERDP_IF_STM32F4_DEF_H__
#define __ERDP_IF_STM32F4_DEF_H__

#include "stm32f4xx.h"
typedef void (*rcc_clock_cmd_func_t)(uint32_t RCC_AHBxPeriph, FunctionalState NewState);
typedef struct{
    DMA_Stream_TypeDef *dma_stream;
    uint32_t dma_channel;
    uint32_t peripheral_base_addr;
    uint32_t transfer_done_flag;
}dma_cfg_t;
enum{
    DMA_DIR_MEMORY_TO_PERIPH,
    DMA_DIR_PERIPH_TO_MEMORY,

    DMA_DIR
};
#endif    //__ERDP_IF_STM32F4_DEF_H__
