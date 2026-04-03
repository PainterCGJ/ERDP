/* erdp include */
#include "erdp_if_spi.h"

#include "erdp_if_gpio.h"
#include "erdp_if_stm32f4_def.h"

/* platform include */
#include "stm32f4xx.h"

extern void erdp_spi_irq_handler(ERDP_Spi_t spi);

static const uint32_t spi_instance[ERDP_SPI_NUM] = {
    (uint32_t)0,
    (uint32_t)SPI1,
    (uint32_t)SPI2,
    (uint32_t)SPI3,
};

static const uint32_t spi_pclk[ERDP_SPI_NUM] = {
    (uint32_t)0,
    (uint32_t)RCC_APB2Periph_SPI1,
    (uint32_t)RCC_APB1Periph_SPI2,
    (uint32_t)RCC_APB1Periph_SPI3,
};

static const rcc_clock_cmd_func_t rcc_clock_cmd_func[ERDP_SPI_NUM] = {
    NULL,
    RCC_APB2PeriphClockCmd,
    RCC_APB1PeriphClockCmd,
    RCC_APB1PeriphClockCmd,
};

static const uint8_t spi_irq_id[ERDP_SPI_NUM] = {
    (uint8_t)0,
    (uint8_t)SPI1_IRQn,
    (uint8_t)SPI2_IRQn,
    (uint8_t)SPI3_IRQn,
};

static const dma_cfg_t spi_dma_cfg[ERDP_SPI_NUM][DMA_DIR] = {
    {{UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL}, {UNUSED_VAL, UNUSED_VAL, UNUSED_VAL, UNUSED_VAL}},
    /* SPI1 TX */
    {{DMA2_Stream3, DMA_Channel_3, (uint32_t)&SPI1->DR, DMA_FLAG_TCIF3},
     /* SPI1 RX */
     {DMA2_Stream2, DMA_Channel_3, (uint32_t)&SPI1->DR, DMA_FLAG_TCIF2}},
    /* SPI2 TX */
    {{DMA1_Stream4, DMA_Channel_0, (uint32_t)&SPI2->DR, DMA_FLAG_TCIF4},
     /* SPI2 RX */
     {DMA1_Stream3, DMA_Channel_0, (uint32_t)&SPI2->DR, DMA_FLAG_TCIF3}},
    /* SPI3 TX */
    {{DMA1_Stream0, DMA_Channel_0, (uint32_t)&SPI3->DR, DMA_FLAG_TCIF0},
     /* SPI3 RX */
     {DMA1_Stream2, DMA_Channel_0, (uint32_t)&SPI3->DR, DMA_FLAG_TCIF2}},
};

uint32_t erdp_if_spi_get_PCLK(ERDP_Spi_t spi) { return spi_pclk[spi]; }
void erdp_if_spi_gpio_init(ERDP_SpiInfo_t *spi_info, ERDP_SpiMode_t mode) {
    GPIO_TypeDef *gpio_periph;
    uint32_t gpio_pin;
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;          // 复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;    // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          // 上拉

    RCC_AHB1PeriphClockCmd(erdp_if_gpio_get_PCLK(spi_info->miso_port), ENABLE);
    RCC_AHB1PeriphClockCmd(erdp_if_gpio_get_PCLK(spi_info->mosi_port), ENABLE);
    RCC_AHB1PeriphClockCmd(erdp_if_gpio_get_PCLK(spi_info->sck_port), ENABLE);
    RCC_AHB1PeriphClockCmd(erdp_if_gpio_get_PCLK(spi_info->cs_port), ENABLE);

    gpio_periph = (GPIO_TypeDef *)erdp_if_gpio_get_port(spi_info->mosi_port);
    gpio_pin = erdp_if_gpio_get_pin(spi_info->mosi_pin);
    GPIO_InitStructure.GPIO_Pin = gpio_pin;
    GPIO_Init(gpio_periph, &GPIO_InitStructure);
    GPIO_PinAFConfig(gpio_periph, spi_info->mosi_pin, spi_info->mosi_af);

    gpio_periph = (GPIO_TypeDef *)erdp_if_gpio_get_port(spi_info->miso_port);
    gpio_pin = erdp_if_gpio_get_pin(spi_info->miso_pin);
    GPIO_InitStructure.GPIO_Pin = gpio_pin;
    GPIO_Init(gpio_periph, &GPIO_InitStructure);
    GPIO_PinAFConfig(gpio_periph, spi_info->miso_pin, spi_info->miso_af);

    gpio_periph = (GPIO_TypeDef *)erdp_if_gpio_get_port(spi_info->sck_port);
    gpio_pin = erdp_if_gpio_get_pin(spi_info->sck_pin);
    GPIO_InitStructure.GPIO_Pin = gpio_pin;
    GPIO_Init(gpio_periph, &GPIO_InitStructure);
    GPIO_PinAFConfig(gpio_periph, spi_info->sck_pin, spi_info->sck_af);

    gpio_periph = (GPIO_TypeDef *)erdp_if_gpio_get_port(spi_info->cs_port);
    gpio_pin = erdp_if_gpio_get_pin(spi_info->cs_pin);
    GPIO_InitStructure.GPIO_Pin = gpio_pin;

    if (mode == ERDP_SPI_MODE_MASTER) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
        GPIO_Init(gpio_periph, &GPIO_InitStructure);
    } else if (mode == ERDP_SPI_MODE_SLAVE) {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(gpio_periph, &GPIO_InitStructure);
        GPIO_PinAFConfig(gpio_periph, spi_info->cs_pin, spi_info->cs_af);
    }
}

static void erdp_if_spi_dma_init(ERDP_Spi_t spi, ERDP_SpiCfg_t *spi_cfg) {
    if ((uint32_t)spi_dma_cfg[spi][DMA_DIR_MEMORY_TO_PERIPH].dma_stream >
        (uint32_t)DMA2)    // 得到当前stream是属于DMA2还是DMA1
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);    // DMA2时钟使能

    } else {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);    // DMA1时钟使能
    }
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_Memory0BaseAddr = 0;                                 // DMA 存储器0地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                    // 存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = 0;                                      // 数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;           // 外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                    // 存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    // 外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;            // 存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                              // 使用普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                      // 中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;            // 存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    // 外设突发单次传输
    if (spi_cfg->enable_tx_dma) {
        DMA_InitStructure.DMA_Channel = spi_dma_cfg[spi][DMA_DIR_MEMORY_TO_PERIPH].dma_channel;    // 通道选择
        DMA_InitStructure.DMA_PeripheralBaseAddr =
            spi_dma_cfg[spi][DMA_DIR_MEMORY_TO_PERIPH].peripheral_base_addr;    // DMA外设地址0
        DMA_Init(spi_dma_cfg[spi][DMA_DIR_MEMORY_TO_PERIPH].dma_stream, &DMA_InitStructure);
    }
    if (spi_cfg->enable_rx_dma) {
        DMA_InitStructure.DMA_Channel = spi_dma_cfg[spi][DMA_DIR_PERIPH_TO_MEMORY].dma_channel;    // 通道选择
        DMA_InitStructure.DMA_PeripheralBaseAddr =
            spi_dma_cfg[spi][DMA_DIR_PERIPH_TO_MEMORY].peripheral_base_addr;    // DMA外设地址0
        DMA_Init(spi_dma_cfg[spi][DMA_DIR_PERIPH_TO_MEMORY].dma_stream, &DMA_InitStructure);
    }
}

void erdp_if_spi_init(ERDP_Spi_t spi, ERDP_SpiMode_t mode, ERDP_SpiCfg_t *spi_cfg, ERDP_SpiDataSize_t data_size) {
    SPI_InitTypeDef SPI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    rcc_clock_cmd_func[spi](erdp_if_spi_get_PCLK(spi), ENABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;

    if (mode == ERDP_SPI_MODE_MASTER) {
        SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
        SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    } else if (mode == ERDP_SPI_MODE_SLAVE) {
        SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
        SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
        NVIC_InitStructure.NVIC_IRQChannel = spi_irq_id[spi];
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    }

    if (data_size == ERDP_SPI_DATASIZE_8BIT) {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    } else if (data_size == ERDP_SPI_DATASIZE_16BIT) {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    }

    if (spi_cfg->clk_mode == ERDP_SPI_CLKMODE_0) {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    } else if (spi_cfg->clk_mode == ERDP_SPI_CLKMODE_1) {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    } else if (spi_cfg->clk_mode == ERDP_SPI_CLKMODE_2) {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    } else if (spi_cfg->clk_mode == ERDP_SPI_CLKMODE_3) {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    }

    if (spi_cfg->endian == ERDP_SPI_ENDIAN_MSB) {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    } else if (spi_cfg->endian == ERDP_SPI_ENDIAN_LSB) {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    }

    SPI_InitStructure.SPI_BaudRatePrescaler = spi_cfg->prescale;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_CalculateCRC((SPI_TypeDef *)spi_instance[spi], DISABLE);
    SPI_Init((SPI_TypeDef *)spi_instance[spi], &SPI_InitStructure);
    SPI_Cmd((SPI_TypeDef *)spi_instance[spi], ENABLE);
    erdp_if_spi_dma_init(spi, spi_cfg);
}

void erdp_if_spi_deinit(ERDP_Spi_t spi) {
    SPI_DeInit((SPI_TypeDef *)spi_instance[spi]);
    rcc_clock_cmd_func[spi](erdp_if_spi_get_PCLK(spi), DISABLE);
}

void erdp_if_spi_enable(ERDP_Spi_t spi, bool enable) {
    if (enable) {
        SPI_Cmd((SPI_TypeDef *)spi_instance[spi], ENABLE);
    } else {
        SPI_Cmd((SPI_TypeDef *)spi_instance[spi], DISABLE);
    }
}
void erdp_if_spi_send(ERDP_Spi_t spi, uint16_t data) { SPI_SendData((SPI_TypeDef *)spi_instance[spi], data); }

void erdp_if_spi_send_dma(ERDP_Spi_t spi, uint8_t *data, uint32_t len) {
    DMA_Stream_TypeDef *s = spi_dma_cfg[spi][DMA_DIR_MEMORY_TO_PERIPH].dma_stream;
    uint32_t tc_flag = spi_dma_cfg[spi][DMA_DIR_MEMORY_TO_PERIPH].transfer_done_flag;
    DMA_Cmd(s, DISABLE);
    while (DMA_GetCmdStatus(s) != DISABLE) {
    }                             // 等 EN 真正清零
    DMA_ClearFlag(s, tc_flag);    // 至少清 TCIF，最好把该stream相关错误/半传输也清掉
    s->M0AR = (uint32_t)data;
    DMA_SetCurrDataCounter(s, len);
    SPI_I2S_DMACmd((SPI_TypeDef *)spi_instance[spi], SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(s, ENABLE);
}

bool erdp_if_spi_dma_transfer_complete(ERDP_Spi_t spi) {
    return DMA_GetFlagStatus(spi_dma_cfg[spi][DMA_DIR_MEMORY_TO_PERIPH].dma_stream,
                             spi_dma_cfg[spi][DMA_DIR_MEMORY_TO_PERIPH].transfer_done_flag) == SET;
}

uint16_t erdp_if_spi_recv(ERDP_Spi_t spi) { return SPI_ReceiveData((SPI_TypeDef *)spi_instance[spi]); }

bool erdp_if_spi_transfer_complete(ERDP_Spi_t spi) {
    return SPI_GetFlagStatus((SPI_TypeDef *)spi_instance[spi], SPI_I2S_FLAG_BSY) == RESET;
}

bool erdp_if_spi_transmit_buffer_empty(ERDP_Spi_t spi) {
    return SPI_GetFlagStatus((SPI_TypeDef *)spi_instance[spi], SPI_I2S_FLAG_TXE) == SET;
}

bool erdp_if_spi_receive_buffer_not_empty(ERDP_Spi_t spi) {
    return SPI_GetFlagStatus((SPI_TypeDef *)spi_instance[spi], SPI_I2S_FLAG_RXNE) == SET;
}

void SPI0_IRQHandler(void) { erdp_spi_irq_handler(ERDP_SPI0); }

void SPI1_IRQHandler(void) { erdp_spi_irq_handler(ERDP_SPI1); }

void SPI2_IRQHandler(void) { erdp_spi_irq_handler(ERDP_SPI2); }

void SPI3_IRQHandler(void) { erdp_spi_irq_handler(ERDP_SPI3); }
