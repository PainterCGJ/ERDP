#include "erdp_osal_timer.hpp"

#ifdef ERDP_ENABLE_RTOS
namespace erdp {
    Timer::Timer(std::function<void()> callback, const char *name, uint32_t period_ms, bool auto_reload)
        : m_usrTimerFunc(callback) {
        m_handler = erdp_if_rtos_timer_create(name, period_ms, auto_reload, m_timerCallback, this);
    }

    Timer::~Timer() { erdp_if_rtos_timer_delete(m_handler); }

    bool Timer::start() { return erdp_if_rtos_timer_start(m_handler); }

    bool Timer::stop() { return erdp_if_rtos_timer_stop(m_handler); }

    bool Timer::setPeriod(uint32_t period_ms) { return erdp_if_rtos_timer_set_period(m_handler, period_ms); }

    void Timer::m_timerCallback(void *parm) {
        Timer *timer = static_cast<Timer *>(parm);
        timer->m_usrTimerFunc();
    }
}    // namespace erdp
#endif    // ERDP_ENABLE_RTOS
