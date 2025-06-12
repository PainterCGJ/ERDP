#include "erdp_osal.hpp"

void task_run(void *parm)
{
    Task *task = static_cast<Task *>(parm);
    task->task_code(); 
    task->kill();        
}