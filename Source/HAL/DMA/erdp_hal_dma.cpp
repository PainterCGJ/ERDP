#include "erdp_hal_dma.hpp"

namespace erdp {
    DmaDev *DmaDev::m_instance[ERDP_DMA_NUM][ERDP_DMA_STREAM_NUM] = {{nullptr}};
    DmaDev::DmaDev(const DmaConfig_t& config) : m_config(config) {
        m_instance[config.dma][config.stream] = this;
        erdp_if_dma_init(config);
    }
    DmaDev::~DmaDev() {}
    void DmaDev::setUsrIrqFunc(std::function<void()> usrIrqFunc) { m_usrIrqFunc = usrIrqFunc; }
    extern "C" {
        void erdp_dma_irq_handler(ERDP_Dma_t dma, ERDP_DmaStream_t stream) {
            if (DmaDev::m_instance[dma][stream] != nullptr) {
                if (DmaDev::m_instance[dma][stream]->m_usrIrqFunc != nullptr) {
                    DmaDev::m_instance[dma][stream]->m_usrIrqFunc();
                }
            }
        }
    }
}    // namespace erdp
