#include "erdp_hal_spi.hpp"
namespace erdp
{
    SpiBase *SpiBase::m_spiInstance[ERDP_SPI_NUM];
    extern "C"
    {
        void erdp_spi_irq_handler(ERDP_Spi_t spi)
        {
            if (SpiBase::m_spiInstance[spi] != nullptr)
            {
                SpiBase::m_spiInstance[spi]->irqHandler();
            }
        }
    }
}
