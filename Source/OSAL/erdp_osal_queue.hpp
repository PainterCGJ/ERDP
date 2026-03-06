#ifndef __ERDP_OSAL_QUEUE_HPP__
#define __ERDP_OSAL_QUEUE_HPP__
#include "erdp_assert.h"
#include "erdp_config.h"
#include "erdp_osal_fifo.hpp"

#ifdef ERDP_ENABLE_RTOS
#include "erdp_if_rtos.h"
namespace erdp {
    template <typename _Type>
    class Queue : public FifoBase<_Type> {
       public:
        Queue() {}
        Queue(uint32_t queue_length) { init(queue_length); }

        ~Queue()
        {
            erdp_if_rtos_queue_delet(m_handler);
        }

        bool init(size_t queue_length)
        {
            m_handler = erdp_if_rtos_queue_create(queue_length, sizeof(_Type));
            if (m_handler == nullptr)
            {
                return false;
            }
            m_queueLength = queue_length;
            m_queueSize = 0;
            return true;
        }

        bool push(const _Type &elm_to_push, uint32_t ticks_to_wait = 0) override
        {
            erdp_assert(m_handler != nullptr);
            if (erdp_if_rtos_queue_send(m_handler, (uint8_t *)(&elm_to_push), ticks_to_wait))
            {
                m_queueSize++;
                return true;
            }
            return false;
        }

        bool pop(_Type &elm_recv, uint32_t ticks_to_wait = 0) override
        {
            erdp_assert(m_handler != nullptr);
            if (erdp_if_rtos_queue_recv(m_handler, (uint8_t *)(&elm_recv), ticks_to_wait))
            {
                m_queueSize--;
                return true;
            }
            return false;
        }

        bool empty() const noexcept
        {
            erdp_assert(m_handler != nullptr);
            return m_queueSize == 0;
        }

        bool full() const noexcept
        {
            erdp_assert(m_handler != nullptr);
            return m_queueSize == m_queueLength;
        }

        uint32_t size() const noexcept
        {
            erdp_assert(m_handler != nullptr);
            return m_queueSize;
        }

       private:
        OS_Queue m_handler;
        uint32_t m_queueLength;
        uint32_t m_queueSize;
    };
}    // namespace erdp
#endif    // ERDP_ENABLE_RTOS

#endif
