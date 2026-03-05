#include "erdp_osal_queue.hpp"

namespace erdp
{
#ifdef ERDP_ENABLE_RTOS
    template <typename _Type>
    Queue<_Type>::~Queue() { erdp_if_rtos_queue_delet(m_handler); }

    template <typename _Type>
    bool Queue<_Type>::init(size_t queue_length)
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

    template <typename _Type>
    bool Queue<_Type>::push(const _Type &elm_to_push, uint32_t ticks_to_wait)
    {
        erdp_assert(m_handler != nullptr);
        if (erdp_if_rtos_queue_send(m_handler, (uint8_t *)(&elm_to_push), ticks_to_wait))
        {
            m_queueSize++;
            return true;
        }
        return false;
    }

    template <typename _Type>
    bool Queue<_Type>::pop(_Type &elm_recv, uint32_t ticks_to_wait)
    {
        erdp_assert(m_handler != nullptr);
        if (erdp_if_rtos_queue_recv(m_handler, (uint8_t *)(&elm_recv), ticks_to_wait))
        {
            m_queueSize--;
            return true;
        }
        return false;
    }

    template <typename _Type>
    bool Queue<_Type>::empty() const noexcept
    {
        erdp_assert(m_handler != nullptr);
        if (m_queueSize == 0)
        {
            return true;
        }
        return false;
    }

    template <typename _Type>
    bool Queue<_Type>::full() const noexcept
    {
        erdp_assert(m_handler != nullptr);
        if (m_queueSize == m_queueLength)
        {
            return true;
        }
        return false;
    }

    template <typename _Type>
    uint32_t Queue<_Type>::size() const noexcept
    {
        erdp_assert(m_handler != nullptr);
        return m_queueSize;
    }

    // 显式实例化常用的类型
    template class Queue<uint8_t>;
    template class Queue<uint16_t>;
    template class Queue<uint32_t>;
    template class Queue<int>;
    template class Queue<float>;
    template class Queue<double>;
#endif // ERDP_ENABLE_RTOS
} // namespace erdp