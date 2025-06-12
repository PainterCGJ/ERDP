#ifndef ERDP_OSAL_HPP
#define ERDP_OSAL_HPP
#include "erdp_if_rtos.h"
#include "string.h"

extern "C"
{
    void task_run(void *parm);
}

class Task
{
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
     *      Task TASK(task, "task", 1);
     *      task.join();
     *      while(1){}
     * }
     */
    Task(void (*task_code)(void *p_arg), const char *name, uint32_t priority, size_t starck_size = DEFAULT_STACK_SIZE)
        : __task_code(task_code), __starck_size(starck_size)
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
     *      Task TASK(task, (void*)(&a), "task", 1);
     *      task.join();
     *      while(1){}
     * }
     */
    Task(void (*task_code)(void *p_arg), void *p_arg, const char *name, uint32_t priority, size_t starck_size = DEFAULT_STACK_SIZE)
        : __task_code(task_code), __p_arg(p_arg), __starck_size(starck_size)
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
     * class TASK: public Task
     * {
     *  public:
     *  TASK()：Thread("task",1,64){join();}
     *  virtual void task_code() override
     *  {
     *      printf("hello world\r\n");
     *  }
     * }
     */
    Task(const char *name, uint32_t priority, size_t starck_size) : __priority(priority), __starck_size(starck_size)
    {
        strcpy(__name, name);
    }

    ~Task(){}

    void join()
    {
        if (!__join_flag)
        {
            __join_flag = 1;
            __handler = erdp_if_rtos_task_create(task_run, __name, __starck_size, this, __priority);
        }
    }

    void suspend(Task* task)
    {
        erdp_if_rtos_task_suspend(task->get_task_handler());
    }

    void suspend()
    {
        erdp_if_rtos_task_suspend(__handler);
    }

    void resume(Task* task)
    {
        erdp_if_rtos_task_resume(task->get_task_handler());
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
        erdp_if_rtos_task_delete(__handler);
    }

    OS_TaskHandle get_task_handler()
    {
        return __handler;
    }

    virtual void task_code()
    {
        __task_code(__p_arg);
    }

private:
    void (*__task_code)(void *p_arg);
    void *__p_arg;
    char __name[configMAX_TASK_NAME_LEN + 1];
    uint32_t __priority;
    size_t __starck_size;
    OS_TaskHandle __handler;
    uint8_t __join_flag = 0;
};

#endif
