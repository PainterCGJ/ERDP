#ifndef __THREAD_CONFIG_H__
#define __THREAD_CONFIG_H__
#include "erdp_if_rtos.h"

enum ThreadPriority {
    MINIMAL_PRIO = 0,
/* ================================< user config >================================ */

    LOG_SERVICE_PRIO,

/* =============================< end of user config >============================ */
    MAX_PRIO = OS_MAX_PRIORITIES - 1,

};

#define LOG_THREAD_STACK_SIZE 512



#endif