/* erdp include */
#include "erdp_if_spi.h"

#include "erdp_if_gpio.h"

/* platform include */
#include "stm32f4xx.h"

extern void erdp_spi_irq_handler(ERDP_Spi_t spi);

const static uint32_t spi_instance[ERDP_SPI_NUM] = {
    (uint32_t)0,
    (uint32_t)SPI1,
    (uint32_t)SPI2,
    (uint32_t)SPI3,
};

const static uint32_t spi_pclk[ERDP_SPI_NUM] = {
    (uint32_t)0,
    (uint32_t)RCC_APB2Periph_SPI1,
    (uint32_t)RCC_APB1Periph_SPI2,
    (uint32_t)RCC_APB1Periph_SPI3,
};
typedef void (*rcc_clock_cmd_func_t)(uint32_t RCC_AHBxPeriph, FunctionalState NewState);
static const rcc_clock_cmd_func_t rcc_clock_cmd_func[ERDP_SPI_NUM] = {
    NULL,
    RCC_APB2PeriphClockCmd,
    RCC_APB1PeriphClockCmd,
    RCC_APB1PeriphClockCmd,
};

const static uint8_t spi_irq_id[ERDP_SPI_NUM] = {
    (uint8_t)0,
    (uint8_t)SPI1_IRQn,
    (uint8_t)SPI2_IRQn,
    (uint8_t)SPI3_IRQn,
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
