
#include "erdp_if_sys.h"
#include "stm32f4xx.h"
void erdp_if_sys_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Config(SystemCoreClock / 1000);
}
