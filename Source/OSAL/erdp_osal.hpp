#ifndef ERDP_OSAL_HPP
#define ERDP_OSAL_HPP
#include "erdp_config.h"
#include "erdp_assert.h"
#include "erdp_if_rtos.h"
#include "string.h"

#include <cstddef>
#include <functional>
#include <queue>
#include <map>
namespace erdp
{
    extern "C"
    {
        void erdp_task_run(void *parm);
    }

    template <typename _Type>
    class ContainerBase
    {
    public:
        ContainerBase() {}
        ~ContainerBase() {}

        virtual bool initialize(size_t size) = 0;
        virtual bool push(const _Type &elm) = 0;
        virtual bool pop(_Type &elm) = 0;
        virtual uint32_t size() const noexcept = 0;
        virtual bool empty() const noexcept = 0;
        virtual bool full() const noexcept = 0;
    };

#ifdef ERDP_ENABLE_RTOS
    class Thread
    {
        friend void create_main_task();

    public:
        /**
         * @brief 创建线程，线程无传入参数，可用于直接创建线程对象
         * @param[in] task_code 线程代码
         * @param[in] name 线程名称
         * @param[in] priority 线程优先值
         * @param[in] starck_size 堆栈大小，默认值为Default_Starck_size
         * @example
         * void task(void* parg)
         * {
         *      printf("hello world\r\n");
         *      //运行结束，自动删除线程
         * }
         *
         * void rtos_main(void)
         * {
         *      Thread TASK(task, "task", 1);
         *      task.join();
         *      while(1){}
         * }
         */
        Thread(void (*task_code)(void *p_arg), const char *name, uint32_t priority, size_t starck_size = DEFAULT_STACK_SIZE)
            : __thread_code(task_code), __starck_size(starck_size)
        {
            strcpy(__name, name);
        }

        /**
         * @brief 创建线程，线程有传入参数，可用于直接创建线程对象
         * @param[in] task_code 线程代码
         * @param[in] p_arg 线程传入参数
         * @param[in] name 线程名称
         * @param[in] priority 线程优先值
         * @param[in] starck_size 堆栈大小，默认值为Default_Starck_size
         * @example
         * void task(void* parg)
         * {
         *      printf("a = %d", *((int*)(parg)));
         *      //运行结束，自动删除线程
         * }
         *
         * void rtos_main(void)
         * {
         *      int a = 10;
         *      Thread TASK(task, (void*)(&a), "task", 1);
         *      task.join();
         *      while(1){}
         * }
         */
        Thread(void (*task_code)(void *p_arg), void *p_arg, const char *name, uint32_t priority, size_t starck_size = DEFAULT_STACK_SIZE)
            : __thread_code(task_code), __p_arg(p_arg), __starck_size(starck_size)
        {
            strcpy(__name, name);
        }

        /**
         * @brief 当选择使用Thread作为基类定义派生类时，无需传入task_code线程代码
         * 需要在派生类中定义虚函数task_code，作为线程运行的主体，并在调用join后才
         * 真正的创建该线程，执行task_code里的内容。当task_code结束后，该线程将会被
         * 自动删除，即FreeRTOS删除任务
         * @param[in] name 线程名称
         * @param[in] starck_size 栈大小
         * @param[in] priority 线程优先值
         * @example
         * class TASK: public Thread
         * {
         *  public:
         *  TASK()：Thread("task",1,64){join();}
         *  virtual void task_code() override
         *  {
         *      printf("hello world\r\n");
         *  }
         * }
         */
        Thread(const char *name, uint32_t priority, size_t starck_size) : __priority(priority), __starck_size(starck_size)
        {
            strcpy(__name, name);
        }

        ~Thread() {}

        void join()
        {
            if (!__join_flag)
            {
                __join_flag = 1;
                __handler = erdp_if_rtos_task_create(erdp_task_run, __name, __starck_size, this, __priority);
            }
        }

        void suspend(Thread *task)
        {
            erdp_if_rtos_task_suspend(task->get_thread_handler());
        }

        void suspend()
        {
            erdp_if_rtos_task_suspend(__handler);
        }

        void resume(Thread *task)
        {
            erdp_if_rtos_task_resume(task->get_thread_handler());
        }

        void resume()
        {
            erdp_if_rtos_task_resume(__handler);
        }

        void kill(OS_TaskHandle handler)
        {
            erdp_if_rtos_task_delete(handler);
        }

        void kill()
        {
            erdp_assert(__handler != __main_task);
            erdp_if_rtos_task_delete(__handler);
        }

        OS_TaskHandle get_thread_handler()
        {
            return __handler;
        }

        static void delay_ms(uint32_t ms)
        {
            erdp_if_rtos_delay_ms(ms);
        }

        static void start_scheduler()
        {
            erdp_if_rtos_start_scheduler();
        }

        virtual void thread_code()
        {
            __thread_code(__p_arg);
        }

    private:
        void (*__thread_code)(void *p_arg);
        void *__p_arg;
        char __name[configMAX_TASK_NAME_LEN + 1];
        uint32_t __priority;
        size_t __starck_size;
        OS_TaskHandle __handler;
        uint8_t __join_flag = 0;

        static OS_TaskHandle __main_task;
        static void main_thread(void *parm);
    };

    template <typename _Type>
    class Queue : public ContainerBase<_Type>
    {
    public:
        Queue() {}
        Queue(uint32_t queue_length)
        {
            initialize(queue_length);
        }
        ~Queue() { erdp_if_rtos_queue_delet(__handler); }

        bool initialize(size_t queue_length)
        {
            __handler = erdp_if_rtos_queue_create(queue_length, sizeof(_Type));
            if (__handler == nullptr)
            {
                return false;
            }
            __queue_length = queue_length;
            __queue_size = 0;
            return true;
        }
        bool push(const _Type &elm_to_push, uint32_t ticks_to_wait)
        {
            erdp_assert(__handler != nullptr);
            if (erdp_if_rtos_queue_send(__handler, (uint8_t *)(&elm_to_push), ticks_to_wait))
            {
                __queue_size++;
                return true;
            }
            return false;
        }

        bool push(const _Type &elm_to_push)
        {
            erdp_assert(__handler != nullptr);
            if (erdp_if_rtos_queue_send(__handler, (uint8_t *)(&elm_to_push), 0))
            {
                __queue_size++;
                return true;
            }
            return false;
        }

        bool pop(_Type &elm_recv, uint32_t ticks_to_wait)
        {
            erdp_assert(__handler != nullptr);
            if (erdp_if_rtos_queue_recv(__handler, (uint8_t *)(&elm_recv), ticks_to_wait))
            {
                __queue_size--;
                return true;
            }
            return false;
        }

        bool pop(_Type &elm_recv)
        {
            erdp_assert(__handler != nullptr);
            if (erdp_if_rtos_queue_recv(__handler, (uint8_t *)(&elm_recv), 0))
            {
                __queue_size--;
                return true;
            }
            return false;
        }

        bool empty() const noexcept
        {
            erdp_assert(__handler != nullptr);
            if (__queue_size == 0)
            {
                return true;
            }
            return false;
        }

        bool full() const noexcept
        {
            erdp_assert(__handler != nullptr);
            if (__queue_size == __queue_length)
            {
                return true;
            }
            return false;
        }

        uint32_t size() const noexcept
        {
            erdp_assert(__handler != nullptr);
            return __queue_size;
        }

    private:
        OS_Queue __handler;
        uint32_t __queue_length;
        uint32_t __queue_size;
    };

template <Semaphore_tag T>
class Semaphore {
public:
    
    template <Semaphore_tag U = T, typename = std::enable_if_t<U != COUNT_TAG>>
    Semaphore()
    {
        if constexpr (U == BINARY_TAG) {
            __handler = erdp_if_rtos_semaphore_creat(BINARY_TAG);
        }
        else if constexpr (U == MUTEX_TAG) {
            __handler = erdp_if_rtos_semaphore_creat(MUTEX_TAG);
        }
        else if constexpr (U == RECURISIVE_TAG) {
            __handler = erdp_if_rtos_semaphore_creat(RECURISIVE_TAG);
        }
    }

    template <Semaphore_tag U = T, typename = std::enable_if_t<U == COUNT_TAG>>
    Semaphore(uint32_t max_count, uint32_t initial_count)
    {
        __handler = erdp_if_rtos_counting_semaphore_creat(max_count, initial_count);
    }

    // 获取信号量
    bool take(uint32_t ticks_to_wait = portMAX_DELAY)
    {
        if constexpr (T == RECURISIVE_TAG) {
            return erdp_if_rtos_recursive_semaphore_take(__handler, ticks_to_wait);
        } else {
            return erdp_if_rtos_semaphore_take(__handler, ticks_to_wait);
        }
    }

    // 释放信号量
    bool give()
    {
        if constexpr (T == RECURISIVE_TAG) {
            return erdp_if_rtos_recursive_semaphore_give(__handler);
        } else {
            return erdp_if_rtos_semaphore_give(__handler);
        }
    }

    ~Semaphore()
    {
        if(__handler != nullptr) {
            erdp_if_rtos_semaphore_delet(__handler);
        }
    }

    // 删除拷贝构造和赋值
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

private:
    OS_Semaphore __handler = nullptr;
};

class Mutex : public Semaphore<MUTEX_TAG>
{
public:
    Mutex() : Semaphore<MUTEX_TAG>() {}
};

class Event
    {

    public:
        Event() : __handler(erdp_if_rtos_event_create()) {}

        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;

        ~Event()
        {
            vEventGroupDelete(__handler);
        }
        OS_EventBits set(OS_EventBits bits_to_set)
        {
            return erdp_if_rtos_set_event_bits(__handler, bits_to_set);
        }

        OS_EventBits clear(OS_EventBits bits_to_clear)
        {
            return erdp_if_rtos_clear_event_bits(__handler, bits_to_clear);
        }

        OS_EventBits get()
        {
            return erdp_if_rtos_get_event_bits(__handler);
        }

        OS_EventBits wait(OS_EventBits bits_to_wait, uint32_t ticks_to_wait = OS_WAIT_FOREVER, bool wait_for_all = true)
        {
            return xEventGroupWaitBits(__handler, bits_to_wait, false, wait_for_all, ticks_to_wait);
        }

        OS_EventBits sync(OS_EventBits bits_to_set, OS_EventBits bits_wait_for, uint32_t ticks_to_wait = OS_WAIT_FOREVER)
        {
            return erdp_if_rtos_event_sync(__handler, bits_to_set, bits_wait_for, ticks_to_wait);
        }

    private:
        OS_Event __handler;
    };
    
#else // ERDP_ENABLE_RTOS
    class Heap4
    {
    public:
        // 内存统计信息
        struct Stats
        {
            size_t total_size;      // 内存池总大小
            size_t free_size;       // 当前空闲内存
            size_t min_free_size;   // 历史最小空闲内存
            size_t alloc_count;     // 当前分配块数
            size_t max_alloc_count; // 最大分配块数
        };

        // 初始化内存池
        Heap4() {}
        explicit Heap4(void *heap_area, size_t heap_size) noexcept;

        // 禁止拷贝和移动
        Heap4(const Heap4 &) = delete;
        Heap4 &operator=(const Heap4 &) = delete;

        void initialize(void *heap_area, size_t heap_size) noexcept;

        // 内存分配(无异常)
        void *allocate(size_t size) noexcept;

        // 内存释放
        void deallocate(void *ptr) noexcept;

        // 获取内存统计
        Stats get_stats() const noexcept;

        // 遍历所有内存块(用于调试)
        void walk_heap(const std::function<void(void *ptr, size_t size, bool is_free)> &visitor) const noexcept;

        // 验证堆完整性(调试用)
        bool validate() const noexcept;

        // C++标准Allocator接口(无异常)
        template <typename T>
        class Allocator
        {
        public:
            using value_type = T;

            explicit Allocator(Heap4 &heap) noexcept : heap_(heap) {}

            template <typename U>
            Allocator(const Allocator<U> &other) noexcept : heap_(other.heap_) {}

            T *allocate(size_t n) noexcept
            {
                return static_cast<T *>(heap_.allocate(n * sizeof(T)));
            }

            void deallocate(T *p, size_t) noexcept
            {
                heap_.deallocate(p);
            }

            Heap4 &heap_;
        };

    private:
        struct BlockHeader
        {
            BlockHeader *next; // 下一个空闲块
            size_t size;       // 块大小(包括头部)
        };

        // 内存对齐设置
        static constexpr size_t ALIGNMENT = 8;
        static constexpr size_t ALIGN_MASK = ALIGNMENT - 1;
        static constexpr size_t MIN_BLOCK_SIZE = sizeof(BlockHeader) * 2;

        // 辅助函数
        static size_t align_size(size_t size) noexcept;
        void insert_free_block(BlockHeader *block) noexcept;
        void coalesce_free_blocks(BlockHeader *prev, BlockHeader *block) noexcept;
        BlockHeader *find_best_fit(size_t required_size) noexcept;

        // 堆管理变量
        BlockHeader *heap_start_;
        BlockHeader *heap_end_;
        BlockHeader *free_list_;
        size_t free_size_;
        size_t min_free_size_;
        size_t alloc_count_;
        size_t max_alloc_count_;
    };

    // 全局默认堆(需先初始化)
    extern Heap4 *default_heap;

#endif // ERDP_ENABLE_RTOS

    template <typename T>
    class RingBuffer : public ContainerBase<T>
    {
    public:
        RingBuffer(const RingBuffer &) = delete;
        RingBuffer &operator=(const RingBuffer &) = delete;

        RingBuffer() {}

        RingBuffer(uint8_t *mempool, size_t mempool_size) noexcept
        {
            initialize(mempool, mempool_size);
        }

        RingBuffer(size_t size) noexcept
        {
            initialize(size);
        }

        bool initialize(uint8_t *mempool, size_t mempool_size)
        {
            erdp_assert(mempool != nullptr);
            erdp_assert(mempool_size % sizeof(T) == 0);
            __buffer = reinterpret_cast<T *>(mempool);
            __size = mempool_size / sizeof(T);
            __head = 0;
            __tail = 0;
            return true;
        }

        bool initialize(uint32_t size) noexcept
        {
            __buffer = new T[size];
            if (__buffer == nullptr)
            {
                return false;
            }
            __size = size;
            __head = 0;
            __tail = 0;
            return true;
        }

        bool full() const noexcept { return (__tail + 1) % __size == __head; }

        bool empty() const noexcept { return __head == __tail; }

        bool push(const T &item) noexcept
        {
            if (full())
            {
                return false;
            }
            __buffer[__tail] = item;
            __tail = (__tail + 1) % __size;
            return true;
        }

        bool pop(T &item) noexcept
        {
            if (empty())
            {
                return false;
            }
            item = __buffer[__head];
            __head = (__head + 1) % __size;
            return true;
        }

        uint32_t size() const noexcept
        {
            return (__tail - __head + __size) % __size;
        }

    private:
        T *__buffer;
        uint32_t __size;
        volatile uint32_t __head;
        volatile uint32_t __tail;

        T &operator[](uint32_t index)
        {
            erdp_assert(index < size());
            return *reinterpret_cast<T *>(__buffer + ((__head + index) % __size));
        }

        const T &operator[](uint32_t index) const
        {
            erdp_assert(index < size());
            return *reinterpret_cast<const T *>(__buffer + ((__head + index) % __size));
        }
    };

} // namespace erdp

#endif
