#include "erdp_if_dma.h"

#include "stm32f4xx.h"


extern void erdp_dma_irq_handler(ERDP_Dma_t dma, ERDP_DmaStream_t stream);
static const DMA_Stream_TypeDef *dma_instance[ERDP_DMA_NUM][ERDP_DMA_STREAM_NUM] = {
    {UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL},
    {DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3, DMA1_Stream4, DMA1_Stream5, DMA1_Stream6, DMA1_Stream7},
    {DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3, DMA2_Stream4, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7},
};
static const uint32_t dma_channel[ERDP_DMA_CHANNEL_NUM] = {
    DMA_Channel_0, DMA_Channel_1, DMA_Channel_2, DMA_Channel_3,
    DMA_Channel_4, DMA_Channel_5, DMA_Channel_6, DMA_Channel_7,
};

static const uint32_t dma_irq[ERDP_DMA_NUM][ERDP_DMA_STREAM_NUM] = {
    {UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL},
    {DMA1_Stream0_IRQn, DMA1_Stream1_IRQn, DMA1_Stream2_IRQn, DMA1_Stream3_IRQn, DMA1_Stream4_IRQn, DMA1_Stream5_IRQn,
     DMA1_Stream6_IRQn, DMA1_Stream7_IRQn},
    {DMA2_Stream0_IRQn, DMA2_Stream1_IRQn, DMA2_Stream2_IRQn, DMA2_Stream3_IRQn, DMA2_Stream4_IRQn, DMA2_Stream5_IRQn,
     DMA2_Stream6_IRQn, DMA2_Stream7_IRQn},
};

static const uint32_t dma_tc_flag[ERDP_DMA_STREAM_NUM] = {
    DMA_FLAG_TCIF0, DMA_FLAG_TCIF1, DMA_FLAG_TCIF2, DMA_FLAG_TCIF3,
    DMA_FLAG_TCIF4, DMA_FLAG_TCIF5, DMA_FLAG_TCIF6, DMA_FLAG_TCIF7,
};

void erdp_if_dma_init(ERDP_DmaCfg_t cfg) {
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if (cfg.dma == ERDP_DMA1) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);    // DMA1时钟使能
    } else if (cfg.dma == ERDP_DMA2) {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);    // DMA2时钟使能
    } else {
        return;
    }

    switch (cfg.stream_priority) {
        case ERDP_DMA_PRIORITY_LOW:
            DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
            break;
        case ERDP_DMA_PRIORITY_MEDIUM:
            DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
            break;
        case ERDP_DMA_PRIORITY_HIGH:
            DMA_InitStructure.DMA_Priority = DMA_Priority_High;
            break;
        case ERDP_DMA_PRIORITY_MAX:
            DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
            break;
        default:
            break;
    }

    switch (cfg.dir) {
        case ERDP_DMA_DIR_M2P:
            DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;    // 存储器到外设模式
            break;
        case ERDP_DMA_DIR_P2M:
            DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;    // 外设到存储器模式
            break;
        case ERDP_DMA_DIR_M2M:
            DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;    // 存储器到存储器模式
            break;
        default:
            break;
    }

    DMA_InitStructure.DMA_Memory0BaseAddr = 0;                                 // DMA 存储器0地址
    DMA_InitStructure.DMA_BufferSize = 0;                                      // 数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;           // 外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                    // 存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    // 外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;            // 存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                              // 使用普通模式
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;            // 存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    // 外设突发单次传输
    DMA_InitStructure.DMA_Channel = dma_channel[cfg.channel];
    DMA_InitStructure.DMA_PeripheralBaseAddr = cfg.dst_addr;
    DMA_Init((DMA_Stream_TypeDef *)dma_instance[cfg.dma][cfg.stream], &DMA_InitStructure);
    if (cfg.enable_tc_irq) {
        NVIC_InitStructure.NVIC_IRQChannel = dma_irq[cfg.dma][cfg.stream];
        // 设置抢占优先级和响应优先级
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = cfg.irq_priority;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        // 使能中断
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        // 初始化 NVIC
        NVIC_Init(&NVIC_InitStructure);
        DMA_ITConfig((DMA_Stream_TypeDef *)dma_instance[cfg.dma][cfg.stream], DMA_IT_TC, ENABLE);
    }
}

void erdp_if_dma_spi_transfer(ERDP_Dma_t dma, ERDP_DmaStream_t stream, ERDP_Spi_t spi, const uint8_t *data,
                              uint32_t len) {
    DMA_Stream_TypeDef *s = (DMA_Stream_TypeDef *)dma_instance[dma][stream];
    uint32_t tc_flag = dma_tc_flag[stream];
    DMA_Cmd(s, DISABLE);
    while (DMA_GetCmdStatus(s) != DISABLE) {
    }    // 等 EN 真正清零
    DMA_ClearFlag(s, tc_flag);    // 至少清 TCIF，最好把该stream相关错误/半传输也清掉
    s->M0AR = (uint32_t)data;
    DMA_SetCurrDataCounter(s, len);
    SPI_I2S_DMACmd((SPI_TypeDef *)erdp_if_spi_get_instance(spi), SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(s, ENABLE);
}

void DMA1_Stream0_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA1, ERDP_DMA_STREAM0); }
void DMA1_Stream1_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA1, ERDP_DMA_STREAM1); }
void DMA1_Stream2_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA1, ERDP_DMA_STREAM2); }
void DMA1_Stream3_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA1, ERDP_DMA_STREAM3); }
void DMA1_Stream4_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA1, ERDP_DMA_STREAM4); }
void DMA1_Stream5_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA1, ERDP_DMA_STREAM5); }
void DMA1_Stream6_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA1, ERDP_DMA_STREAM6); }
void DMA1_Stream7_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA1, ERDP_DMA_STREAM7); }

void DMA2_Stream0_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA2, ERDP_DMA_STREAM0); }
void DMA2_Stream1_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA2, ERDP_DMA_STREAM1); }
void DMA2_Stream2_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA2, ERDP_DMA_STREAM2); }
void DMA2_Stream3_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA2, ERDP_DMA_STREAM3); }
void DMA2_Stream4_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA2, ERDP_DMA_STREAM4); }
void DMA2_Stream5_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA2, ERDP_DMA_STREAM5); }
void DMA2_Stream6_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA2, ERDP_DMA_STREAM6); }
void DMA2_Stream7_IRQHandler(void) { erdp_dma_irq_handler(ERDP_DMA2, ERDP_DMA_STREAM7); }
