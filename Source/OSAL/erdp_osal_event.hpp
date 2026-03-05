#ifndef __ERDP_OSAL_EVENT_HPP__
#define __ERDP_OSAL_EVENT_HPP__
#include "erdp_if_rtos.h"

namespace erdp
{
#ifdef ERDP_ENABLE_RTOS
    class Event
    {

    public:
        Event();

        Event(const Event &) = delete;
        Event &operator=(const Event &) = delete;

        ~Event();
        OS_EventBits set(OS_EventBits bits_to_set);

        OS_EventBits clear(OS_EventBits bits_to_clear);

        OS_EventBits get();

        OS_EventBits wait(OS_EventBits bits_to_wait, uint32_t ticks_to_wait = OS_WAIT_FOREVER, bool wait_for_all = true);

        OS_EventBits sync(OS_EventBits bits_to_set, OS_EventBits bits_wait_for, uint32_t ticks_to_wait = OS_WAIT_FOREVER);

    private:
        OS_Event __handler;
    };
#endif // ERDP_ENABLE_RTOS
} // namespace erdp

#endif

