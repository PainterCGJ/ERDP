
#include "erdp_if_sys.h"
#include "gd32f4xx.h"
void erdp_if_sys_init(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    systick_config();
}
