#include "erdp_osal_event.hpp"

#ifdef ERDP_ENABLE_RTOS
namespace erdp
{
    Event::Event() : __handler(erdp_if_rtos_event_create()) {}

    Event::~Event()
    {
        vEventGroupDelete(__handler);
    }

    OS_EventBits Event::set(OS_EventBits bits_to_set)
    {
        return erdp_if_rtos_set_event_bits(__handler, bits_to_set);
    }

    OS_EventBits Event::clear(OS_EventBits bits_to_clear)
    {
        return erdp_if_rtos_clear_event_bits(__handler, bits_to_clear);
    }

    OS_EventBits Event::get()
    {
        return erdp_if_rtos_get_event_bits(__handler);
    }

    OS_EventBits Event::wait(OS_EventBits bits_to_wait, uint32_t ticks_to_wait, bool wait_for_all)
    {
        return xEventGroupWaitBits(__handler, bits_to_wait, false, wait_for_all, ticks_to_wait);
    }

    OS_EventBits Event::sync(OS_EventBits bits_to_set, OS_EventBits bits_wait_for, uint32_t ticks_to_wait)
    {
        return erdp_if_rtos_event_sync(__handler, bits_to_set, bits_wait_for, ticks_to_wait);
    }
} // namespace erdp
#endif // ERDP_ENABLE_RTOS
