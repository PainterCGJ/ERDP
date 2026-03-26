#include "erdp_osal_event.hpp"

#ifdef ERDP_ENABLE_RTOS
namespace erdp
{
    Event::Event() : m_handler(erdp_if_rtos_event_create()) {}

    Event::~Event()
    {
        erdp_if_rtos_delete_event(m_handler);
    }

    OS_EventBits Event::set(OS_EventBits bits_to_set)
    {
        return erdp_if_rtos_set_event_bits(m_handler, bits_to_set);
    }

    OS_EventBits Event::clear(OS_EventBits bits_to_clear)
    {
        return erdp_if_rtos_clear_event_bits(m_handler, bits_to_clear);
    }

    OS_EventBits Event::get()
    {
        return erdp_if_rtos_get_event_bits(m_handler);
    }

    OS_EventBits Event::wait(OS_EventBits bits_to_wait, uint32_t ticks_to_wait, bool wait_for_all)
    {
        return erdp_if_rtos_wait_event_bits(m_handler, bits_to_wait, ticks_to_wait, wait_for_all);
    }

    OS_EventBits Event::sync(OS_EventBits bits_to_set, OS_EventBits bits_wait_for, uint32_t ticks_to_wait)
    {
        return erdp_if_rtos_event_sync(m_handler, bits_to_set, bits_wait_for, ticks_to_wait);
    }
} // namespace erdp
#endif // ERDP_ENABLE_RTOS
