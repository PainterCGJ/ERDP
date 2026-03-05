#ifndef __ERDP_OSAL_RINGBUFFER_HPP__
#define __ERDP_OSAL_RINGBUFFER_HPP__
#include "erdp_assert.h"
#include "erdp_osal_fifo.hpp"
#include <cstddef>
#include <cstdint>
#ifdef ERDP_ENABLE_RTOS
#include "erdp_if_rtos.h"
#endif

namespace erdp
{
#ifndef ERDP_ENABLE_RTOS
    // 无 RTOS 版本的 RingBuffer
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

        bool init(uint8_t *mempool, size_t mempool_size);
        bool init(uint32_t size) noexcept;
        bool full() const noexcept;
        bool empty() const noexcept;
        bool push(const T &item, uint32_t ticks_to_wait = 0) noexcept override;
        bool pop(T &item, uint32_t ticks_to_wait = 0) noexcept override;
        uint32_t size() const noexcept;

    private:
        T *m_buffer;
        uint32_t m_size;
        volatile uint32_t m_head;
        volatile uint32_t m_tail;

        T &operator[](uint32_t index);
        const T &operator[](uint32_t index) const;
    };

#else
    // RTOS 版本的 RingBuffer，使用队列接口实现
    template <typename T>
    class RingBuffer : public FifoBase<T>
    {
    public:
        RingBuffer(const RingBuffer &) = delete;
        RingBuffer &operator=(const RingBuffer &) = delete;

        RingBuffer() {}

        RingBuffer(uint32_t queue_length)
        {
            init(queue_length);
        }

        ~RingBuffer();

        bool init(size_t queue_length);
        bool full() const noexcept;
        bool empty() const noexcept;
        bool push(const T &item, uint32_t ticks_to_wait = 0) noexcept override;
        bool pop(T &item, uint32_t ticks_to_wait = 0) noexcept override;
        uint32_t size() const noexcept;

    private:
        OS_Queue m_handler;
        uint32_t m_queueLength;
        uint32_t m_queueSize;
    };
#endif
} // namespace erdp

#endif

