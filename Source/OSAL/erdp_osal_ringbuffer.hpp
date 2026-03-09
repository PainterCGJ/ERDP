#ifndef __ERDP_OSAL_RINGBUFFER_HPP__
#define __ERDP_OSAL_RINGBUFFER_HPP__
#include "erdp_assert.h"
#include "erdp_osal_fifo.hpp"
#include <cstddef>
#include <cstdint>
#include "erdp_config.h"
#ifdef ERDP_ENABLE_RTOS
#include "erdp_if_rtos.h"
#endif

namespace erdp
{
#ifndef ERDP_ENABLE_RTOS
    template <typename Type>
    class RingBuffer : public FifoBase<Type>
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
            erdp_assert(mempool_size % sizeof(Type) == 0);
            m_buffer = reinterpret_cast<Type *>(mempool);
            m_size = mempool_size / sizeof(Type);
            m_head = 0;
            m_tail = 0;
            return true;
        }

        bool init(uint32_t size) noexcept
        {
            m_buffer = new Type[size];
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

        bool push(const Type &item, uint32_t ticks_to_wait = 0) noexcept override
        {
            (void)ticks_to_wait;
            if (full())
            {
                return false;
            }
            m_buffer[m_tail] = item;
            m_tail = (m_tail + 1) % m_size;
            return true;
        }

        bool pop(Type &item, uint32_t ticks_to_wait = 0) noexcept override
        {
            (void)ticks_to_wait;
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

        Type &operator[](uint32_t index)
        {
            erdp_assert(index < size());
            return *reinterpret_cast<Type *>(m_buffer + ((m_head + index) % m_size));
        }

        const Type &operator[](uint32_t index) const
        {
            erdp_assert(index < size());
            return *reinterpret_cast<const Type *>(m_buffer + ((m_head + index) % m_size));
        }

    private:
        Type *m_buffer;
        uint32_t m_size;
        volatile uint32_t m_head;
        volatile uint32_t m_tail;
    };

#else
    template <typename Type>
    class RingBuffer : public FifoBase<Type>
    {
    public:
        RingBuffer(const RingBuffer &) = delete;
        RingBuffer &operator=(const RingBuffer &) = delete;

        RingBuffer():m_handler(nullptr),m_queueLength(0),m_queueSize(0) {}

        RingBuffer(size_t size)
        {
            init(size);
        }

        ~RingBuffer()
        {
            if (m_handler != nullptr)
            {
                erdp_if_rtos_queue_delet(m_handler);
            }
        }

        bool init(size_t size) override
        {
            m_handler = erdp_if_rtos_queue_create(size, sizeof(Type));
            if (m_handler == nullptr)
            {
                return false;
            }
            m_queueLength = size;
            m_queueSize = 0;
            return true;
        }

        bool full() const noexcept
        {
            return m_queueSize == m_queueLength;
        }

        bool empty() const noexcept
        {
            return m_queueSize == 0;
        }

        bool push(const Type &item, uint32_t ticks_to_wait = 0) noexcept override
        {
            erdp_assert(m_handler != nullptr);
            if (erdp_if_rtos_queue_send(m_handler, (uint8_t *)(&item), ticks_to_wait))
            {
                m_queueSize++;
                return true;
            }
            return false;
        }

        bool pop(Type &item, uint32_t ticks_to_wait = 0) noexcept override
        {
            erdp_assert(m_handler != nullptr);
            if (erdp_if_rtos_queue_recv(m_handler, (uint8_t *)(&item), ticks_to_wait))
            {
                m_queueSize--;
                return true;
            }
            return false;
        }

        uint32_t size() const noexcept
        {
            return m_queueSize;
        }

    private:
        OS_Queue m_handler;
        uint32_t m_queueLength;
        uint32_t m_queueSize;
    };
#endif // ERDP_ENABLE_RTOS
} // namespace erdp

#endif
