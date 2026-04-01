#ifndef __ERDP_OSAL_TIMER_HPP__
#define __ERDP_OSAL_TIMER_HPP__
#include "erdp_config.h"
#ifdef ERDP_ENABLE_RTOS
#include <functional>    // NOLINT
#include "erdp_if_rtos.h"
namespace erdp {

    class Timer {
       public:
        Timer(std::function<void()> callback, const char *name, uint32_t periodMs, bool autoReload = true);

        ~Timer();

        bool start();

        bool stop();

        bool setPeriod(uint32_t periodMs);

       private:
        static void timerCallback(void *parm);
        std::function<void()> m_usrTimerFunc;
        OS_Timer m_handler;
    };

}    // namespace erdp
#endif    // ERDP_ENABLE_RTOS

#endif
