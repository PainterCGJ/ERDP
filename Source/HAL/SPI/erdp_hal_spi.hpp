#ifndef __ERDP_HAL_SPI_HPP__
#define __ERDP_HAL_SPI_HPP__
#include "erdp_if_spi.h"
#include "erdp_osal.hpp"
#include "erdp_config.h"

#include <vector>

namespace erdp
{
    using SpiConfig_t = ERDP_SpiCfg_t;
    using SpiInfo_t = ERDP_SpiInfo_t;

    class SpiBase
    {
    public:
        SpiBase() {}
        const SpiInfo_t& get_spi_info() const { return __spi_info; }
    protected:
        SpiInfo_t __spi_info;
        SpiConfig_t __spi_cfg;
    };

    template <ERDP_SpiDataSize_t DATA_SIZE>
    class SpiDevBase : public SpiBase
    {
    public:
        using DataType = typename std::conditional<DATA_SIZE == ERDP_SPI_DATASIZE_8BIT,
                                                   uint8_t,
                                                   uint16_t>::type;
#ifdef ERDP_ENABLE_RTOS
        using Buffer = Queue<DataType>;
#else
        using Buffer = RingBuffer<DataType>;
#endif

        const Buffer &rx_buffer = __rx_buffer;

        SpiDevBase() {}
        SpiDevBase(SpiInfo_t &spi_info, SpiConfig_t &spi_cfg)
        {
            init(spi_info, spi_cfg);
        }
        void init(SpiInfo_t &spi_info, SpiConfig_t &spi_cfg)
        {
            __spi_info = spi_info;
            __spi_cfg = spi_cfg;
            erdp_if_spi_init(__spi_info.spi, &__spi_cfg, DATA_SIZE);
            erdp_if_spi_gpio_init(&__spi_info, __spi_cfg.mode);
        }

        void enable()
        {
            erdp_if_spi_enable(__spi_info.spi, true);
        }

        void disable()
        {
            erdp_if_spi_enable(__spi_info.spi, false);
        }

        virtual bool send(uint8_t *data, uint32_t len) = 0;
        virtual bool recv(uint32_t len) = 0;
        virtual bool send_recv(uint8_t *tx_data, uint8_t *rx_data, uint32_t tx_len, uint32_t rx_len) = 0;

    protected:
        Buffer __rx_buffer;

    private:
    };

    template <ERDP_SpiDataSize_t DATA_SIZE = ERDP_SPI_DATASIZE_8BIT>
    class SpiMasterBase : public SpiDevBase<DATA_SIZE>
    {
    public:
        SpiMasterBase() {}
        SpiMasterBase(SpiInfo_t &spi_info, SpiConfig_t &spi_cfg) : SpiDevBase<DATA_SIZE>(spi_info, spi_cfg)
        {
        }

        void cs_high()
        {
            erdp_if_gpio_write(SpiBase::__spi_info.cs_port, SpiBase::__spi_info.cs_pin, ERDP_SET);
        }
        void cs_low()
        {
            erdp_if_gpio_write(SpiBase::__spi_info.cs_port, SpiBase::__spi_info.cs_pin, ERDP_RESET);
        }

        bool send(typename SpiDevBase<DATA_SIZE>::DataType *data, uint32_t len) override
        {
            uint32_t tx_count = 0;
            while (tx_count < len)
            {
                while (!erdp_if_spi_transmit_buffer_empty(SpiBase::__spi_info.spi))
                    ;
                erdp_if_spi_send(SpiBase::__spi_info.spi, data[tx_count++]);
            }
            while (!erdp_if_spi_transfer_complete(SpiBase::__spi_info.spi))
                ;
            erdp_if_spi_recv(SpiBase::__spi_info.spi);

            return true;
        }

        bool recv(uint32_t len) override
        {
            uint32_t rx_count = 0;
            typename SpiDevBase<DATA_SIZE>::DataType data = 0;
            while (rx_count < len)
            {
                while (!erdp_if_spi_transmit_buffer_empty(SpiBase::__spi_info.spi))
                    ;
                erdp_if_spi_send(SpiBase::__spi_info.spi, 0x00);
                while (!erdp_if_spi_receive_buffer_not_empty(SpiBase::__spi_info.spi))
                    ;
                data = static_cast<typename SpiDevBase<DATA_SIZE>::DataType>(
                    erdp_if_spi_recv(SpiBase::__spi_info.spi));
                SpiDevBase<DATA_SIZE>::__rx_buffer.push(data);
                rx_count++;
            }
            return true;
        }

        bool send_recv(typename SpiDevBase<DATA_SIZE>::DataType *tx_data, typename SpiDevBase<DATA_SIZE>::DataType *rx_data, uint32_t tx_len, uint32_t rx_len) override
        {

            return true;
        }
    };
}
#endif
