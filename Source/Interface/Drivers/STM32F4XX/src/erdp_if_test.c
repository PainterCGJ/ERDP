#include "erdp_if_gpio.h"
#include "stm32f4xx.h"
void SystemInit(void);

void Delay(uint32_t count);

int main(void)
{
    erdp_if_gpio_init(ERDP_GPIOC, ERDP_GPIO_PIN_0, ERDP_GPIO_PIN_MODE_OUTPUT, ERDP_GPIO_PIN_PULL_NONE,
                      ERDP_GPIO_SPEED_MAX);

    while (1)
    {
        erdp_if_gpio_write(ERDP_GPIOC, ERDP_GPIO_PIN_0, ERDP_SET);
        for (volatile uint32_t i = 0; i < 1000000; i++)
        {
            __NOP();
        }
        erdp_if_gpio_write(ERDP_GPIOC, ERDP_GPIO_PIN_0, ERDP_RESET);
        for (volatile uint32_t i = 0; i < 1000000; i++)
        {
            __NOP();
        }
    }
}
