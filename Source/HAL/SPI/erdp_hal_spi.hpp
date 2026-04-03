#ifndef __ERDP_HAL_SPI_HPP__
#define __ERDP_HAL_SPI_HPP__
#include <vector>

#include "erdp_if_spi.h"
#include "erdp_osal.hpp"


namespace erdp {
    extern "C" {
        void erdp_spi_irq_handler(ERDP_Spi_t spi);
    }
    using SpiConfig_t = ERDP_SpiCfg_t;
    using SpiInfo_t = ERDP_SpiInfo_t;

    class SpiBase {
       public:
        SpiBase() {}
        const SpiInfo_t &getSpiInfo() const { return m_spiInfo; }

       protected:
        SpiInfo_t m_spiInfo;
        SpiConfig_t m_spiCfg;
        static SpiBase *m_spiInstance[ERDP_SPI_NUM];

       private:
        friend void erdp_spi_irq_handler(ERDP_Spi_t spi);
        virtual void irqHandler() {}
    };

    template <ERDP_SpiDataSize_t DATA_SIZE>
    class SpiDevBase : public SpiBase {
       public:
        using DataType = typename std::conditional<DATA_SIZE == ERDP_SPI_DATASIZE_8BIT, uint8_t, uint16_t>::type;

        RingBuffer<DataType> &rx_buffer = m_rxBuffer;

        SpiDevBase() {}
        SpiDevBase(const SpiInfo_t &spiInfo, ERDP_SpiMode_t mode, const SpiConfig_t &spiCfg, uint32_t rxBufferSize) {
            init(spiInfo, mode, spiCfg, rxBufferSize);
        }

        void enable() { erdp_if_spi_enable(m_spiInfo.spi, true); }

        void disable() { erdp_if_spi_enable(m_spiInfo.spi, false); }

        virtual bool send(DataType *pData, uint32_t len) = 0;
        virtual bool recv(uint32_t &&len) = 0;

       protected:
        void init(const SpiInfo_t &spiInfo, ERDP_SpiMode_t mode, const SpiConfig_t &spiCfg, uint32_t rxBufferSize) {
            m_spiInfo = spiInfo;
            m_spiCfg = spiCfg;
            m_rxBuffer.init(rxBufferSize);
            m_spiInstance[m_spiInfo.spi] = this;
            erdp_if_spi_init(m_spiInfo.spi, mode, &m_spiCfg, DATA_SIZE);
            erdp_if_spi_gpio_init(&m_spiInfo, mode);
        }
        RingBuffer<DataType> m_rxBuffer;

       private:
    };

    template <ERDP_SpiDataSize_t DATA_SIZE = ERDP_SPI_DATASIZE_8BIT>
    class SpiMasterBase : public SpiDevBase<DATA_SIZE> {
       public:
        SpiMasterBase() = default;    // Add default constructor
        SpiMasterBase(const SpiInfo_t &spiInfo, const SpiConfig_t &spiCfg, uint32_t rxBufferSize)
            : SpiDevBase<DATA_SIZE>(spiInfo, ERDP_SPI_MODE_MASTER, spiCfg, rxBufferSize) {}

        void csHigh() { erdp_if_gpio_write(SpiBase::m_spiInfo.cs_port, SpiBase::m_spiInfo.cs_pin, ERDP_SET); }
        void csLow() { erdp_if_gpio_write(SpiBase::m_spiInfo.cs_port, SpiBase::m_spiInfo.cs_pin, ERDP_RESET); }

        bool send(typename SpiDevBase<DATA_SIZE>::DataType *pData, uint32_t len) override {
            if (SpiBase::m_spiCfg.enable_tx_dma == true) {
                erdp_if_spi_send_dma(SpiBase::m_spiInfo.spi, (uint8_t *)pData, len);
                while (!erdp_if_spi_dma_transfer_complete(SpiBase::m_spiInfo.spi));
            } else {
                uint32_t txCount = 0;
                while (txCount < len) {
                    while (!erdp_if_spi_transmit_buffer_empty(SpiBase::m_spiInfo.spi));
                    erdp_if_spi_send(SpiBase::m_spiInfo.spi, pData[txCount++]);
                    // SPI 全双工: 每发 1 帧都会收到 1 帧，及时读出可避免 RXNE/OVR 影响尾帧完成判定
                    while (!erdp_if_spi_receive_buffer_not_empty(SpiBase::m_spiInfo.spi));
                    (void)erdp_if_spi_recv(SpiBase::m_spiInfo.spi);
                }
            }
            while (!erdp_if_spi_transmit_buffer_empty(SpiBase::m_spiInfo.spi));
            while (!erdp_if_spi_transfer_complete(SpiBase::m_spiInfo.spi));

            return true;
        }

        bool recv(uint32_t &&len) override {
            uint32_t rxCount = 0;
            typename SpiDevBase<DATA_SIZE>::DataType data = 0;
            erdp_if_spi_recv(SpiBase::m_spiInfo.spi);
            while (rxCount < len) {
                while (!erdp_if_spi_transmit_buffer_empty(SpiBase::m_spiInfo.spi));
                erdp_if_spi_send(SpiBase::m_spiInfo.spi, 0x00);
                while (!erdp_if_spi_receive_buffer_not_empty(SpiBase::m_spiInfo.spi));
                data = static_cast<typename SpiDevBase<DATA_SIZE>::DataType>(erdp_if_spi_recv(SpiBase::m_spiInfo.spi));
                SpiDevBase<DATA_SIZE>::m_rxBuffer.push(data);
                rxCount++;
            }
            while (!erdp_if_spi_transfer_complete(SpiBase::m_spiInfo.spi));
            return true;
        }

        bool recv(typename SpiDevBase<DATA_SIZE>::DataType *buffer, uint32_t len) {
            uint32_t rxCount = 0;
            typename SpiDevBase<DATA_SIZE>::DataType data = 0;
            while (rxCount < len) {
                while (!erdp_if_spi_transmit_buffer_empty(SpiBase::m_spiInfo.spi));
                erdp_if_spi_send(SpiBase::m_spiInfo.spi, 0x00);
                while (!erdp_if_spi_receive_buffer_not_empty(SpiBase::m_spiInfo.spi));
                data = static_cast<typename SpiDevBase<DATA_SIZE>::DataType>(erdp_if_spi_recv(SpiBase::m_spiInfo.spi));
                buffer[rxCount++] = data;
            }
            while (!erdp_if_spi_transfer_complete(SpiBase::m_spiInfo.spi));
            return true;
        }

        bool sendRecv(typename SpiDevBase<DATA_SIZE>::DataType *pTxData, uint32_t txLen, uint32_t rxLen) {
            uint32_t txSize = txLen > rxLen ? txLen : rxLen;
            uint32_t txCount = 0;
            uint32_t rxCount = 0;
            uint8_t txAllowed = 1;
            typename SpiDevBase<DATA_SIZE>::DataType data = 0;
            while (txCount < txSize || rxCount < rxLen) {
                if (txAllowed) {
                    txAllowed = 0;
                    while (!erdp_if_spi_transmit_buffer_empty(SpiBase::m_spiInfo.spi));
                    if (txCount < txLen) {
                        erdp_if_spi_send(SpiBase::m_spiInfo.spi, pTxData[txCount]);
                    } else {
                        erdp_if_spi_send(SpiBase::m_spiInfo.spi, 0x00);
                    }
                    txCount++;
                }
                if (rxCount < rxLen) {
                    while (!erdp_if_spi_receive_buffer_not_empty(SpiBase::m_spiInfo.spi));
                    data =
                        static_cast<typename SpiDevBase<DATA_SIZE>::DataType>(erdp_if_spi_recv(SpiBase::m_spiInfo.spi));
                    SpiDevBase<DATA_SIZE>::m_rxBuffer.push(data);
                    rxCount++;
                    txAllowed = 1;
                } else {
                    txAllowed = 1;
                }
            }
            while (!erdp_if_spi_transfer_complete(SpiBase::m_spiInfo.spi));
            erdp_if_spi_recv(SpiBase::m_spiInfo.spi);
            return true;
        }

       protected:
        void init(const SpiInfo_t &spiInfo, const SpiConfig_t &spiCfg, uint32_t rxBufferSize) {
            SpiDevBase<DATA_SIZE>::init(spiInfo, ERDP_SPI_MODE_MASTER, spiCfg, rxBufferSize);
        }
    };

    template <ERDP_SpiDataSize_t DATA_SIZE = ERDP_SPI_DATASIZE_8BIT>
    class SpiSlaveBase : public SpiDevBase<DATA_SIZE> {
       public:
        SpiSlaveBase() = default;    // Add default constructor
        SpiSlaveBase(const SpiInfo_t &spiInfo, const SpiConfig_t &spiCfg, uint32_t rxBufferSize, uint32_t txBufferSize)
            : SpiDevBase<DATA_SIZE>(spiInfo, ERDP_SPI_MODE_SLAVE, spiCfg, rxBufferSize) {
            m_txBuffer.init(txBufferSize);
        }

        bool send(typename SpiDevBase<DATA_SIZE>::DataType *data, uint32_t len) override {
            loadTxBuffer(data, len);
            return true;
        }

        bool recv(uint32_t &&len) override { return true; }

        bool isSendComplete() { return m_txBuffer.empty(); }

        void setUsrRxIrqFunc(std::function<void(typename SpiDevBase<DATA_SIZE>::DataType)> handler) {
            m_usrRxIrqFunc = handler;
        }

        void clearUsrRxIrqFunc() { m_usrRxIrqFunc = nullptr; }

       protected:
        void init(const SpiInfo_t &spiInfo, const SpiConfig_t &spiCfg, uint32_t rxBufferSize, uint32_t txBufferSize) {
            SpiDevBase<DATA_SIZE>::init(spiInfo, ERDP_SPI_MODE_SLAVE, spiCfg, rxBufferSize);
            m_txBuffer.init(txBufferSize);
        }

       private:
        uint32_t m_txCount = 0;
        typename SpiDevBase<DATA_SIZE>::DataType m_data;
        RingBuffer<typename SpiDevBase<DATA_SIZE>::DataType> m_txBuffer;
        std::function<void(typename SpiDevBase<DATA_SIZE>::DataType)> m_usrRxIrqFunc = nullptr;
        bool loadTxBuffer(typename SpiDevBase<DATA_SIZE>::DataType *pData, uint32_t len) {
            m_txCount = 0;
            while (m_txCount < len) {
                if (m_txCount == 0 && erdp_if_spi_transmit_buffer_empty(SpiBase::m_spiInfo.spi)) {
                    erdp_if_spi_send(SpiBase::m_spiInfo.spi, pData[m_txCount++]);
                } else {
                    if (!m_txBuffer.push(pData[m_txCount++])) {
                        return false;
                    }
                }
            }
            return true;
        }
        void irqHandler() override {
            if (erdp_if_spi_transmit_buffer_empty(SpiBase::m_spiInfo.spi)) {
                if (m_txBuffer.pop(m_data)) {
                    erdp_if_spi_send(SpiBase::m_spiInfo.spi, m_data);
                }
            }
            if (erdp_if_spi_receive_buffer_not_empty(SpiBase::m_spiInfo.spi)) {
                m_data =
                    static_cast<typename SpiDevBase<DATA_SIZE>::DataType>(erdp_if_spi_recv(SpiBase::m_spiInfo.spi));
                SpiDevBase<DATA_SIZE>::m_rxBuffer.push(m_data);
                if (m_usrRxIrqFunc) {
                    m_usrRxIrqFunc(m_data);
                }
            }
        }
    };
    template <ERDP_SpiMode_t MODE, ERDP_SpiDataSize_t DATA_SIZE = ERDP_SPI_DATASIZE_8BIT>
    class SpiDev
        : public std::conditional_t<MODE == ERDP_SPI_MODE_MASTER, SpiMasterBase<DATA_SIZE>, SpiSlaveBase<DATA_SIZE>> {
       public:
        // Default constructor
        SpiDev() = default;

        // Master mode constructor
        template <ERDP_SpiMode_t M = MODE, typename = std::enable_if_t<M == ERDP_SPI_MODE_MASTER>>
        SpiDev(const SpiInfo_t &spiInfo, const SpiConfig_t &spiCfg, uint32_t rxBufferSize)
            : SpiMasterBase<DATA_SIZE>(spiInfo, spiCfg, rxBufferSize) {}

        // Slave mode constructor
        template <ERDP_SpiMode_t M = MODE, typename = std::enable_if_t<M == ERDP_SPI_MODE_SLAVE>>
        SpiDev(const SpiInfo_t &spiInfo, const SpiConfig_t &spiCfg, uint32_t rxBufferSize, uint32_t txBufferSize)
            : SpiSlaveBase<DATA_SIZE>(spiInfo, spiCfg, rxBufferSize, txBufferSize) {}

        // Master mode init
        template <ERDP_SpiMode_t M = MODE, typename = std::enable_if_t<M == ERDP_SPI_MODE_MASTER>>
        void init(const SpiInfo_t &spiInfo, const SpiConfig_t &spiCfg, uint32_t rxBufferSize) {
            SpiMasterBase<DATA_SIZE>::init(spiInfo, spiCfg, rxBufferSize);
        }

        // Slave mode init
        template <ERDP_SpiMode_t M = MODE, typename = std::enable_if_t<M == ERDP_SPI_MODE_SLAVE>>
        void init(const SpiInfo_t &spiInfo, const SpiConfig_t &spiCfg, uint32_t rxBufferSize, uint32_t txBufferSize) {
            SpiSlaveBase<DATA_SIZE>::init(spiInfo, spiCfg, rxBufferSize, txBufferSize);
        }
    };
}    // namespace erdp
#endif
