#include "erdp_osal.hpp"
#include <new>
#include <cstring>

#ifdef ERDP_ENABLE_RTOS
namespace erdp
{
    void erdp_task_run(void *parm)
    {
        Thread *thead = static_cast<Thread *>(parm);
        thead->thead_code();
        thead->kill();
    }
}

void *operator new(size_t size)
{
    return pvPortMalloc(size);
}

void *operator new[](size_t size)
{
    return pvPortMalloc(size);
}

void operator delete(void *pointer) noexcept
{
    vPortFree(pointer);
}

void operator delete[](void *pointer) noexcept
{
    vPortFree(pointer);
}

#else  // ERDP_ENABLE_RTOS
namespace erdp
{

    Heap4 *default_heap = nullptr;
    static uint8_t memory_pool[ERDP_CONFIG_HEAP_SIZE];
    Heap4 heap;

    // 内存对齐计算
    size_t Heap4::align_size(size_t size) noexcept
    {
        return (size + ALIGN_MASK) & ~ALIGN_MASK;
    }

    Heap4::Heap4(void *heap_area, size_t heap_size) noexcept
    {
        initialize(heap_area, heap_size);
    }

    void Heap4::initialize(void *heap_area, size_t heap_size) noexcept
    {
        std::fill(static_cast<uint8_t *>(heap_area), static_cast<uint8_t *>(heap_area) + heap_size, 0);
        // 确保堆大小足够并正确对齐
        uint8_t *heap_start = static_cast<uint8_t *>(heap_area);
        size_t aligned_size = align_size(heap_size) - ALIGNMENT;

        // 初始化堆起始和结束块
        heap_start_ = reinterpret_cast<BlockHeader *>(heap_start);
        heap_start_->size = aligned_size;
        heap_start_->next = nullptr;

        heap_end_ = reinterpret_cast<BlockHeader *>(heap_start + aligned_size);
        heap_end_->size = 0;
        heap_end_->next = nullptr;

        // 初始化空闲链表
        free_list_ = heap_start_;
        free_size_ = aligned_size;
        min_free_size_ = aligned_size;
        alloc_count_ = 0;
        max_alloc_count_ = 0;
    }

    void *Heap4::allocate(size_t size) noexcept
    {
        if (size == 0)
        {
            return nullptr;
        }

        // 计算需要的总大小(包括头部和对齐)
        size_t required_size = align_size(size + sizeof(BlockHeader));
        if (required_size < size)
        {
            return nullptr; // 溢出检测
        }

        // 确保块不小于最小值
        if (required_size < MIN_BLOCK_SIZE)
        {
            required_size = MIN_BLOCK_SIZE;
        }

        // 寻找最佳匹配块
        BlockHeader *block = find_best_fit(required_size);
        if (!block)
        {
            return nullptr; // 内存不足
        }

        // 如果剩余空间足够大，则分割块
        if (block->size >= required_size + MIN_BLOCK_SIZE)
        {
            BlockHeader *new_block = reinterpret_cast<BlockHeader *>(
                reinterpret_cast<uint8_t *>(block) + required_size);
            new_block->size = block->size - required_size;
            new_block->next = block->next;

            insert_free_block(new_block);

            block->size = required_size;
        }
        else
        {
            // 整块分配，从空闲链表移除
            BlockHeader **prev_next = &free_list_;
            while (*prev_next != block)
            {
                prev_next = &(*prev_next)->next;
            }
            *prev_next = block->next;
        }

        free_size_ -= block->size;
        if (free_size_ < min_free_size_)
        {
            min_free_size_ = free_size_;
        }

        alloc_count_++;
        if (alloc_count_ > max_alloc_count_)
        {
            max_alloc_count_ = alloc_count_;
        }

        // 返回用户数据区(跳过头部)
        return reinterpret_cast<void *>(block + 1);
    }

    void Heap4::deallocate(void *ptr) noexcept
    {
        if (!ptr)
        {
            return;
        }

        // 获取块头部
        BlockHeader *block = reinterpret_cast<BlockHeader *>(ptr) - 1;

        // 更新统计信息
        free_size_ += block->size;
        alloc_count_--;

        // 将块插入空闲链表并尝试合并相邻块
        insert_free_block(block);
    }

    Heap4::Stats Heap4::get_stats() const noexcept
    {
        return {
            .total_size = static_cast<size_t>(reinterpret_cast<uint8_t *>(heap_end_) - reinterpret_cast<uint8_t *>(heap_start_)),
            .free_size = free_size_,
            .min_free_size = min_free_size_,
            .alloc_count = alloc_count_,
            .max_alloc_count = max_alloc_count_};
    }

    void Heap4::walk_heap(const std::function<void(void *ptr, size_t size, bool is_free)> &visitor) const noexcept
    {
        BlockHeader *block = heap_start_;
        while (block != heap_end_)
        {
            bool is_free = false;
            BlockHeader *free_block = free_list_;
            while (free_block)
            {
                if (free_block == block)
                {
                    is_free = true;
                    break;
                }
                free_block = free_block->next;
            }

            visitor(block + 1, block->size - sizeof(BlockHeader), is_free);

            block = reinterpret_cast<BlockHeader *>(reinterpret_cast<uint8_t *>(block) + block->size);
        }
    }

    bool Heap4::validate() const noexcept
    {
        // 检查空闲链表完整性
        size_t free_size_check = 0;
        BlockHeader *prev_free = nullptr;
        BlockHeader *free_block = free_list_;
        while (free_block)
        {
            if (free_block < heap_start_ || free_block >= heap_end_)
            {
                return false;
            }
            if (prev_free && prev_free >= free_block)
            {
                return false;
            }
            free_size_check += free_block->size;
            prev_free = free_block;
            free_block = free_block->next;
        }

        if (free_size_check != free_size_)
        {
            return false;
        }

        // 遍历整个堆
        BlockHeader *block = heap_start_;
        while (block < heap_end_)
        {
            if (block->size < sizeof(BlockHeader))
            {
                return false;
            }
            if (reinterpret_cast<uint8_t *>(block) + block->size > reinterpret_cast<uint8_t *>(heap_end_))
            {
                return false;
            }
            block = reinterpret_cast<BlockHeader *>(reinterpret_cast<uint8_t *>(block) + block->size);
        }

        return block == heap_end_;
    }

    void Heap4::insert_free_block(BlockHeader *block) noexcept
    {
        BlockHeader *iterator = free_list_;
        BlockHeader *prev = nullptr;

        // 查找插入位置(按地址排序)
        while (iterator && iterator < block)
        {
            prev = iterator;
            iterator = iterator->next;
        }

        // 尝试与前一个块合并
        if (prev && reinterpret_cast<uint8_t *>(prev) + prev->size == reinterpret_cast<uint8_t *>(block))
        {
            prev->size += block->size;
            block = prev;
        }
        else
        {
            // 不能合并，直接插入
            block->next = iterator;
            if (prev)
            {
                prev->next = block;
            }
            else
            {
                free_list_ = block;
            }
        }

        // 尝试与后一个块合并
        if (iterator && reinterpret_cast<uint8_t *>(block) + block->size == reinterpret_cast<uint8_t *>(iterator))
        {
            block->size += iterator->size;
            block->next = iterator->next;
        }
    }

    Heap4::BlockHeader *Heap4::find_best_fit(size_t required_size) noexcept
    {
        BlockHeader *best_block = nullptr;
        BlockHeader *best_prev = nullptr;
        BlockHeader *current = free_list_;
        BlockHeader *prev = nullptr;

        // 最佳适应算法: 寻找最小足够大的块
        while (current)
        {
            if (current->size >= required_size)
            {
                if (!best_block || current->size < best_block->size)
                {
                    best_block = current;
                    best_prev = prev;

                    // 找到完美匹配，立即返回
                    if (best_block->size == required_size)
                    {
                        break;
                    }
                }
            }
            prev = current;
            current = current->next;
        }

        return best_block;
    }

} // namespace erdp

// C++操作符重载实现(无异常)
void *operator new(size_t size)
{
    if (!erdp::default_heap)
    {
        erdp::heap.initialize(erdp::memory_pool, sizeof(erdp::memory_pool));
        erdp::default_heap = &erdp::heap;
    }
    return erdp::default_heap->allocate(size);
}

void *operator new[](size_t size)
{
    return operator new(size);
}

void operator delete(void *ptr) noexcept
{
    if (ptr && erdp::default_heap)
    {
        erdp::default_heap->deallocate(ptr);
    }
}

void operator delete[](void *ptr) noexcept
{
    operator delete(ptr);
}

void operator delete(void *ptr, size_t) noexcept
{
    operator delete(ptr);
}

void operator delete[](void *ptr, size_t) noexcept
{
    operator delete(ptr);
}
#endif // ERDP_ENABLE_RTOS
