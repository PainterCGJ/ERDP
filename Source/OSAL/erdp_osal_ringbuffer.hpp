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

        bool init(uint8_t *mempool, size_t mempool_size);
        bool init(uint32_t size) noexcept;
        bool full() const noexcept;
        bool empty() const noexcept;
        bool push(const T &item) noexcept;
        bool pop(T &item) noexcept;
        uint32_t size() const noexcept;

    private:
        T *m_buffer;
        uint32_t m_size;
        volatile uint32_t m_head;
        volatile uint32_t m_tail;

        T &operator[](uint32_t index);
        const T &operator[](uint32_t index) const;
    };
} // namespace erdp

#endif

