#include "erdp_osal.h"
#include "erdp_if_gpio.h"
using namespace erdp;
#define SYS_LED_PORT ERDP_GPIOC
#define SYS_LED_PIN ERDP_GPIO_PIN_0

void Thread::main_thread(void *parm)
{
	// UartDev uart(uart_backon_config,100);
	// uart.set_as_debug_com();
	// printf("gd32 hello word\n");
    // 使用interface初始化LED
    erdp_if_gpio_init(SYS_LED_PORT, SYS_LED_PIN, ERDP_GPIO_PIN_MODE_OUTPUT, ERDP_GPIO_PIN_PULL_NONE, ERDP_GPIO_SPEED_LOW);
	// 定义LED线程
	erdp::Thread LED_thread(
        []() {
            while (1) {
                erdp_if_gpio_write(SYS_LED_PORT, SYS_LED_PIN, ERDP_SET);   
                Thread::delay_ms(500);
                erdp_if_gpio_write(SYS_LED_PORT, SYS_LED_PIN, ERDP_RESET);   
                Thread::delay_ms(500);
            }
        },
        "LED", 6, 128);
    LED_thread.join();

    while (1)
    {
        Thread::delay_ms(10);
    }
}