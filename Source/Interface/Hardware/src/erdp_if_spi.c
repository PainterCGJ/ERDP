/* erdp include */
#include "erdp_if_spi.h"
#include "erdp_if_gpio.h"

/* platform include */
#include "gd32f4xx.h"

extern void erdp_spi_irq_handler(ERDP_Spi_t spi);

const static uint32_t spi_instance[ERDP_SPI_MAX] = {
    (uint32_t)SPI0,
    (uint32_t)SPI1,
    (uint32_t)SPI2,
    (uint32_t)SPI3,
};

const static uint8_t spi_irq_id[ERDP_SPI_MAX] = {
    (uint8_t)SPI0_IRQn,
    (uint8_t)SPI1_IRQn,
    (uint8_t)SPI2_IRQn,
    (uint8_t)SPI3_IRQn,
};

uint32_t erdp_if_spi_get_PCLK(ERDP_Spi_t spi)
{
    switch (spi)
    {
    case ERDP_SPI0:
        return (uint32_t)RCU_SPI0;
    case ERDP_SPI1:
        return (uint32_t)RCU_SPI1;
    case ERDP_SPI2:
        return (uint32_t)RCU_SPI2;
    case ERDP_SPI3:
        return (uint32_t)RCU_SPI3;
    default:
        return 0; // Invalid SPI
    }
}
void erdp_if_spi_gpio_init(ERDP_SpiGpioCfg_t *spi_gpio_cfg, ERDP_SpiMode_t mode)
{

    rcu_periph_clock_enable(erdp_if_gpio_get_PCLK(spi_gpio_cfg->miso_port));
    rcu_periph_clock_enable(erdp_if_gpio_get_PCLK(spi_gpio_cfg->mosi_port));
    rcu_periph_clock_enable(erdp_if_gpio_get_PCLK(spi_gpio_cfg->sck_port));
    rcu_periph_clock_enable(erdp_if_gpio_get_PCLK(spi_gpio_cfg->cs_port));

    gpio_mode_set(spi_gpio_cfg->mosi_port, GPIO_MODE_AF, GPIO_PUPD_NONE, spi_gpio_cfg->mosi_pin);
    gpio_output_options_set(spi_gpio_cfg->mosi_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, spi_gpio_cfg->mosi_pin);
    gpio_af_set(spi_gpio_cfg->mosi_port, spi_gpio_cfg->mosi_af, spi_gpio_cfg->mosi_pin);

    gpio_mode_set(spi_gpio_cfg->miso_port, GPIO_MODE_AF, GPIO_PUPD_NONE, spi_gpio_cfg->miso_pin);
    gpio_output_options_set(spi_gpio_cfg->miso_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, spi_gpio_cfg->miso_pin);
    gpio_af_set(spi_gpio_cfg->miso_port, spi_gpio_cfg->miso_af, spi_gpio_cfg->miso_pin);

    gpio_mode_set(spi_gpio_cfg->sck_port, GPIO_MODE_AF, GPIO_PUPD_NONE, spi_gpio_cfg->sck_pin);
    gpio_output_options_set(spi_gpio_cfg->sck_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, spi_gpio_cfg->sck_pin);
    gpio_af_set(spi_gpio_cfg->sck_port, spi_gpio_cfg->sck_af, spi_gpio_cfg->sck_pin);

    if (mode == ERDP_SPI_MODE_MASTER)
    {
        gpio_mode_set(spi_gpio_cfg->cs_port, GPIO_MODE_AF, GPIO_PUPD_NONE, spi_gpio_cfg->cs_pin);
        gpio_output_options_set(spi_gpio_cfg->cs_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, spi_gpio_cfg->cs_pin);
    }
    else if (mode == ERDP_SPI_MODE_SLAVE)
    {
        gpio_mode_set(spi_gpio_cfg->cs_port, GPIO_MODE_AF, GPIO_PUPD_NONE, spi_gpio_cfg->cs_pin);
        gpio_af_set(spi_gpio_cfg->cs_port, spi_gpio_cfg->cs_af, spi_gpio_cfg->cs_pin);
    }
}

void erdp_if_spi_init(ERDP_Spi_t spi, ERDP_SpiCfg_t *spi_cfg)

{
    spi_parameter_struct spi_init_struct;
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;

    if (spi_cfg->mode == ERDP_SPI_MODE_MASTER)
    {
        spi_init_struct.device_mode = SPI_MASTER;
    }
    else if (spi_cfg->mode == ERDP_SPI_MODE_SLAVE)
    {
        spi_init_struct.device_mode = SPI_SLAVE;
        nvic_irq_enable(spi_irq_id[spi], spi_cfg->priority, 0);
        spi_i2s_interrupt_enable(spi_instance[spi], SPI_I2S_INT_RBNE);
    }

    if (spi_cfg->endian == ERDP_SPI_ENDIAN_MSB)
    {
        spi_init_struct.endian = SPI_ENDIAN_MSB;
    }
    else if (spi_cfg->endian == ERDP_SPI_ENDIAN_LSB)
    {
        spi_init_struct.endian = SPI_ENDIAN_LSB;
    }

    if (spi_cfg->data_size == ERDP_SPI_DATASIZE_8BIT)
    {
        spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    }
    else if (spi_cfg->data_size == ERDP_SPI_DATASIZE_16BIT)
    {
        spi_init_struct.frame_size = SPI_FRAMESIZE_16BIT;
    }

    if (spi_cfg->clk_mode == ERDP_SPI_CLKMODE_0)
    {
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    }
    else if (spi_cfg->clk_mode == ERDP_SPI_CLKMODE_1)
    {
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    }
    else if (spi_cfg->clk_mode == ERDP_SPI_CLKMODE_2)
    {
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
    }
    else if (spi_cfg->clk_mode == ERDP_SPI_CLKMODE_3)
    {
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    }

    if(spi_cfg->endian == ERDP_SPI_ENDIAN_MSB)
    {
        spi_init_struct.endian = SPI_ENDIAN_MSB;
    }
    else if (spi_cfg->endian == ERDP_SPI_ENDIAN_LSB)
    {
        spi_init_struct.endian = SPI_ENDIAN_LSB;
    }

    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = spi_cfg->prescale;

    rcu_periph_clock_enable(erdp_if_spi_get_PCLK(spi));
    spi_init(spi_instance[spi], &spi_init_struct);
    spi_enable(spi_instance[spi]);
}

void erdp_if_spi_deinit(ERDP_Spi_t spi)
{
    spi_disable(spi_instance[spi]);
    spi_i2s_interrupt_disable(spi_instance[spi], SPI_I2S_INT_RBNE);
    nvic_irq_disable(spi_irq_id[spi]);
    rcu_periph_clock_disable(erdp_if_spi_get_PCLK(spi));
}

void erdp_if_spi_enable(ERDP_Spi_t spi,bool enable)
{
    if(enable)
    {
        spi_enable(spi_instance[spi]);
    }
    else
    {
        spi_disable(spi_instance[spi]);
    }
}
void erdp_if_spi_send(ERDP_Spi_t spi,uint16_t data)
{
    spi_i2s_data_transmit(spi_instance[spi], data);
}

void erdp_if_spi_recv(ERDP_Spi_t spi,uint16_t *data)
{
    *data = spi_i2s_data_receive(spi_instance[spi]);
}

bool erdp_if_spi_transfer_complete(ERDP_Spi_t spi)
{
    return !spi_i2s_flag_get(spi_instance[spi], SPI_STAT_TRANS);
}

bool erdp_if_spi_transmit_buffer_empty(ERDP_Spi_t spi)
{
    return spi_i2s_flag_get(spi_instance[spi], SPI_STAT_TBE);
}

bool erdp_if_spi_receive_buffer_not_empty(ERDP_Spi_t spi)
{
    return spi_i2s_flag_get(spi_instance[spi], SPI_STAT_RBNE);
}


void SPI0_IRQHandler(void)
{
    erdp_spi_irq_handler(ERDP_SPI0);
}

void SPI1_IRQHandler(void)
{
    erdp_spi_irq_handler(ERDP_SPI1);
}

void SPI2_IRQHandler(void)
{
    erdp_spi_irq_handler(ERDP_SPI2);
}

void SPI3_IRQHandler(void)
{
    erdp_spi_irq_handler(ERDP_SPI3);
}
