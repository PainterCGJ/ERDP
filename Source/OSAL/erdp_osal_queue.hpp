#ifndef __ERDP_OSAL_QUEUE_HPP__
#define __ERDP_OSAL_QUEUE_HPP__
#include "erdp_assert.h"
#include "erdp_if_rtos.h"
#include "erdp_osal_fifo.hpp"

namespace erdp
{
#ifdef ERDP_ENABLE_RTOS
    template <typename _Type>
    class Queue : public FifoBase<_Type>
    {
    public:
        Queue() {}
        Queue(uint32_t queue_length)
        {
            init(queue_length);
        }
        ~Queue();

        bool init(size_t queue_length);
        bool push(const _Type &elm_to_push, uint32_t ticks_to_wait = 0) override;
        bool pop(_Type &elm_recv, uint32_t ticks_to_wait = 0) override;
        bool empty() const noexcept;
        bool full() const noexcept;
        uint32_t size() const noexcept;

    private:
        OS_Queue m_handler;
        uint32_t m_queueLength;
        uint32_t m_queueSize;
    };
#endif // ERDP_ENABLE_RTOS
} // namespace erdp

#endif

