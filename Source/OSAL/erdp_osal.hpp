#ifndef __ERDP_OSAL_HPP__
#define __ERDP_OSAL_HPP__
#include <functional> // NOLINT

#include "erdp_assert.h"
#include "erdp_config.h"
#include "erdp_heap.hpp"
#include "erdp_if_rtos.h"
#include "erdp_osal_event.hpp"
#include "erdp_osal_fifo.hpp"
#include "erdp_osal_queue.hpp"
#include "erdp_osal_ringbuffer.hpp"
#include "erdp_osal_semaphore.hpp"
#include "erdp_osal_timer.hpp"
#include "string.h"

namespace erdp {
    extern "C" {
        void erdp_task_run(void *parm);
    }

#ifdef ERDP_ENABLE_RTOS
    class Thread {
        friend void create_main_task();

       public:
        Thread(void (*task_code)(void *p_arg), const char *name, uint32_t priority,
               size_t starck_size = DEFAULT_STACK_SIZE);

        Thread(void (*task_code)(void *p_arg), void *p_arg, const char *name, uint32_t priority,
               size_t starck_size = DEFAULT_STACK_SIZE);

        Thread(const char *name, uint32_t priority, size_t starck_size);

        Thread(std::function<void()> handle, const char *name, uint32_t priority,
               size_t stack_size = DEFAULT_STACK_SIZE);

        virtual ~Thread();

        void join();

        void suspend(Thread *task);

        void suspend();

        void resume(Thread *task);

        void resume();

        void kill(OS_TaskHandle handler = nullptr);

        OS_TaskHandle getThreadHandler();

        static void sleep(uint32_t ms);

        static void startScheduler();

        static uint32_t getSystem1msTicks();

        virtual void threadCode();

       private:
        void (*m_threadCode)(void *p_arg) = nullptr;
        std::function<void()> m_threadCodeLambda = nullptr;
        void *m_pArg;
        char m_name[configMAX_TASK_NAME_LEN + 1];
        uint32_t m_priority;
        size_t m_stackSize;
        OS_TaskHandle m_handler;
        uint8_t m_joinFlag = 0;

        static OS_TaskHandle m_mainTask;
        static void mainThread(void *parm);
    };

#else    // ERDP_ENABLE_RTOS
    // 全局默认堆(需先初始化)
    extern Heap4 *default_heap;

#endif    // ERDP_ENABLE_RTOS

}    // namespace erdp

#endif
