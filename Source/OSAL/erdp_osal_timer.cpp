#include "erdp_osal_timer.hpp"

#ifdef ERDP_ENABLE_RTOS
namespace erdp {
    Timer::Timer(std::function<void()> callback, const char *name, uint32_t period_ms, bool auto_reload)
        : __usr_timer_func(callback) {
        __handler = erdp_if_rtos_timer_create(name, period_ms, auto_reload, __timer_callback, this);
    }

    Timer::~Timer() { erdp_if_rtos_timer_delete(__handler); }

    bool Timer::start() { return erdp_if_rtos_timer_start(__handler); }

    bool Timer::stop() { return erdp_if_rtos_timer_stop(__handler); }

    bool Timer::set_period(uint32_t period_ms) { return erdp_if_rtos_timer_set_period(__handler, period_ms); }

    void Timer::__timer_callback(void *parm) {
        Timer *timer = static_cast<Timer *>(parm);
        timer->__usr_timer_func();
    }
}    // namespace erdp
#endif    // ERDP_ENABLE_RTOS
