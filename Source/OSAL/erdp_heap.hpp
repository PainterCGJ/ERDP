#ifndef __ERDP_HEAP_HPP__
#define __ERDP_HEAP_HPP__
#include <cstddef>
#include <stdint.h>
#include <cstdio>
#include "erdp_assert.h"
#include "erdp_config.h"
#define HEAP4_DEBUG 0

#ifndef ERDP_ENABLE_RTOS
class Heap4
{
public:
    struct Stats
    {
        size_t total_size;
        size_t free_size;
        size_t min_free_size;
        size_t alloc_count;
        size_t max_alloc_count;
    };

    Heap4() = default;
    explicit Heap4(void *heap_area, size_t heap_size) noexcept;
    Heap4(const Heap4 &) = delete;
    Heap4 &operator=(const Heap4 &) = delete;

    void init(void *heap_area, size_t heap_size) noexcept;
    void *allocate(size_t size) noexcept;
    void deallocate(void *ptr) noexcept;
    Stats get_stats() const noexcept;
    bool validate() noexcept;

private:
    struct BlockHeader
    {
        BlockHeader *next;
        size_t size;
#ifdef HEAP4_DEBUG
        bool in_use;
#endif
    };
    static constexpr size_t ALIGNMENT = 8;
    static constexpr size_t MIN_BLOCK_SIZE = sizeof(BlockHeader) * 2;
    static size_t align_up(size_t sz) noexcept { return (sz + ALIGNMENT - 1) & ~(ALIGNMENT - 1); }

    void insert_and_coalesce(BlockHeader *block) noexcept;

    BlockHeader *heap_start_ = nullptr;
    BlockHeader *heap_end_ = nullptr;
    BlockHeader *free_list_ = nullptr;
    size_t free_size_ = 0;
    size_t min_free_size_ = 0;
    size_t alloc_count_ = 0;
    size_t max_alloc_count_ = 0;
};
#endif // !ERDP_ENABLE_RTOS
#endif // __ERDP_HEAP_HPP__

