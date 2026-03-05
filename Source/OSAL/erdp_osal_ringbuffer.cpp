#include "erdp_osal_ringbuffer.hpp"

namespace erdp
{
#ifndef ERDP_ENABLE_RTOS
    // 无 RTOS 版本的 RingBuffer 实现
    template <typename T>
    bool RingBuffer<T>::init(uint8_t *mempool, size_t mempool_size)
    {
        erdp_assert(mempool != nullptr);
        erdp_assert(mempool_size % sizeof(T) == 0);
        m_buffer = reinterpret_cast<T *>(mempool);
        m_size = mempool_size / sizeof(T);
        m_head = 0;
        m_tail = 0;
        return true;
    }

    template <typename T>
    bool RingBuffer<T>::init(uint32_t size) noexcept
    {
        m_buffer = new T[size];
        if (m_buffer == nullptr)
        {
            return false;
        }
        m_size = size;
        m_head = 0;
        m_tail = 0;
        return true;
    }

    template <typename T>
    bool RingBuffer<T>::full() const noexcept { return (m_tail + 1) % m_size == m_head; }

    template <typename T>
    bool RingBuffer<T>::empty() const noexcept { return m_head == m_tail; }

    template <typename T>
    bool RingBuffer<T>::push(const T &item, uint32_t ticks_to_wait) noexcept
    {
        (void)ticks_to_wait; // 忽略参数，无 RTOS 版本不使用超时机制
        if (full())
        {
            return false;
        }
        m_buffer[m_tail] = item;
        m_tail = (m_tail + 1) % m_size;
        return true;
    }

    template <typename T>
    bool RingBuffer<T>::pop(T &item, uint32_t ticks_to_wait) noexcept
    {
        (void)ticks_to_wait; // 忽略参数，无 RTOS 版本不使用超时机制
        if (empty())
        {
            return false;
        }
        item = m_buffer[m_head];
        m_head = (m_head + 1) % m_size;
        return true;
    }

    template <typename T>
    uint32_t RingBuffer<T>::size() const noexcept
    {
        return (m_tail - m_head + m_size) % m_size;
    }

    template <typename T>
    T &RingBuffer<T>::operator[](uint32_t index)
    {
        erdp_assert(index < size());
        return *reinterpret_cast<T *>(m_buffer + ((m_head + index) % m_size));
    }

    template <typename T>
    const T &RingBuffer<T>::operator[](uint32_t index) const
    {
        erdp_assert(index < size());
        return *reinterpret_cast<const T *>(m_buffer + ((m_head + index) % m_size));
    }

#else
    // RTOS 版本的 RingBuffer 实现，使用队列接口
    template <typename T>
    RingBuffer<T>::~RingBuffer() { 
        if (m_handler != nullptr) {
            erdp_if_rtos_queue_delet(m_handler);
        }
    }

    template <typename T>
    bool RingBuffer<T>::init(size_t queue_length)
    {
        m_handler = erdp_if_rtos_queue_create(queue_length, sizeof(T));
        if (m_handler == nullptr)
        {
            return false;
        }
        m_queueLength = queue_length;
        m_queueSize = 0;
        return true;
    }

    template <typename T>
    bool RingBuffer<T>::full() const noexcept
    {
        return m_queueSize == m_queueLength;
    }

    template <typename T>
    bool RingBuffer<T>::empty() const noexcept
    {
        return m_queueSize == 0;
    }

    template <typename T>
    bool RingBuffer<T>::push(const T &item, uint32_t ticks_to_wait) noexcept
    {
        erdp_assert(m_handler != nullptr);
        if (erdp_if_rtos_queue_send(m_handler, (uint8_t *)(&item), ticks_to_wait))
        {
            m_queueSize++;
            return true;
        }
        return false;
    }

    template <typename T>
    bool RingBuffer<T>::pop(T &item, uint32_t ticks_to_wait) noexcept
    {
        erdp_assert(m_handler != nullptr);
        if (erdp_if_rtos_queue_recv(m_handler, (uint8_t *)(&item), ticks_to_wait))
        {
            m_queueSize--;
            return true;
        }
        return false;
    }

    template <typename T>
    uint32_t RingBuffer<T>::size() const noexcept
    {
        return m_queueSize;
    }
#endif

    // 显式实例化常用的类型
    template class RingBuffer<uint8_t>;
    template class RingBuffer<uint16_t>;
    template class RingBuffer<uint32_t>;
    template class RingBuffer<int>;
    template class RingBuffer<float>;
    template class RingBuffer<double>;
} // namespace erdp