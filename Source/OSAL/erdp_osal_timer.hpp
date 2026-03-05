#ifndef __ERDP_OSAL_TIMER_HPP__
#define __ERDP_OSAL_TIMER_HPP__
#include "erdp_if_rtos.h"
#include <functional>

namespace erdp {
#ifdef ERDP_ENABLE_RTOS
    class Timer {
       public:
        Timer(std::function<void()> callback, const char *name, uint32_t period_ms, bool auto_reload = true);

        ~Timer();

        bool start();

        bool stop();

        bool set_period(uint32_t period_ms);

       private:
        static void m_timerCallback(void *parm);
        std::function<void()> m_usrTimerFunc;
        OS_Timer m_handler;
    };
#endif    // ERDP_ENABLE_RTOS
}    // namespace erdp

#endif

