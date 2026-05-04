#ifndef __ERDP_HAL_DMA_H__
#define __ERDP_HAL_DMA_H__
#include "erdp_if_dma.h"
#include "erdp_osal.hpp"
namespace erdp {
    using DmaConfig_t = ERDP_DmaCfg_t;
    extern "C" {
        void erdp_dma_irq_handler(ERDP_Dma_t dma, ERDP_DmaStream_t stream);
    }
    class DmaDev {
       public:
        friend void erdp_dma_irq_handler(ERDP_Dma_t dma, ERDP_DmaStream_t stream);

        DmaDev(const DmaConfig_t& config);
        ~DmaDev();
        void setUsrIrqFunc(std::function<void()> usrIrqFunc);

       private:
        static DmaDev* m_instance[ERDP_DMA_NUM][ERDP_DMA_STREAM_NUM];
        std::function<void()> m_usrIrqFunc = nullptr;
        DmaConfig_t m_config;
    };
}    // namespace erdp
#endif    // __ERDP_HAL_DMA_H__