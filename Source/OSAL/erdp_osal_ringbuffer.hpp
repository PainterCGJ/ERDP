#ifndef __ERDP_OSAL_RINGBUFFER_HPP__
#define __ERDP_OSAL_RINGBUFFER_HPP__
#include "erdp_assert.h"
#include "erdp_osal_fifo.hpp"
#include <cstddef>
#include <cstdint>

namespace erdp
{
    template <typename T>
    class RingBuffer : public FifoBase<T>
    {
    public:
        RingBuffer(const RingBuffer &) = delete;
        RingBuffer &operator=(const RingBuffer &) = delete;

        RingBuffer() {}

        RingBuffer(uint8_t *mempool, size_t mempool_size) noexcept
        {
            init(mempool, mempool_size);
        }

        RingBuffer(size_t size) noexcept
        {
            init(size);
        }

        bool init(uint8_t *mempool, size_t mempool_size)
        {
            erdp_assert(mempool != nullptr);
            erdp_assert(mempool_size % sizeof(T) == 0);
            m_buffer = reinterpret_cast<T *>(mempool);
            m_size = mempool_size / sizeof(T);
            m_head = 0;
            m_tail = 0;
            return true;
        }

        bool init(uint32_t size) noexcept
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

        bool full() const noexcept { return (m_tail + 1) % m_size == m_head; }

        bool empty() const noexcept { return m_head == m_tail; }

        bool push(const T &item) noexcept
        {
            if (full())
            {
                return false;
            }
            m_buffer[m_tail] = item;
            m_tail = (m_tail + 1) % m_size;
            return true;
        }

        bool pop(T &item) noexcept
        {
            if (empty())
            {
                return false;
            }
            item = m_buffer[m_head];
            m_head = (m_head + 1) % m_size;
            return true;
        }

        uint32_t size() const noexcept
        {
            return (m_tail - m_head + m_size) % m_size;
        }

    private:
        T *m_buffer;
        uint32_t m_size;
        volatile uint32_t m_head;
        volatile uint32_t m_tail;

        T &operator[](uint32_t index)
        {
            erdp_assert(index < size());
            return *reinterpret_cast<T *>(m_buffer + ((m_head + index) % m_size));
        }

        const T &operator[](uint32_t index) const
        {
            erdp_assert(index < size());
            return *reinterpret_cast<const T *>(m_buffer + ((m_head + index) % m_size));
        }
    };
} // namespace erdp

#endif

