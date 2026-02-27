#include "erdp_if_gpio.h"
#include "gd32f4xx.h"

int main(void)
{
    erdp_if_gpio_init(ERDP_GPIOC, ERDP_GPIO_PIN_13, ERDP_GPIO_PIN_MODE_OUTPUT, ERDP_GPIO_PIN_PULL_NONE,
                      ERDP_GPIO_SPEED_MAX);

    while (1)
    {
        erdp_if_gpio_write(ERDP_GPIOC, ERDP_GPIO_PIN_13, ERDP_RESET);
        for (volatile uint32_t i = 0; i < 6000000; i++)
        {
        }
        erdp_if_gpio_write(ERDP_GPIOC, ERDP_GPIO_PIN_13, ERDP_SET);
        for (volatile uint32_t i = 0; i < 6000000; i++)
        {
        }
    }
}
