#include "erdp_osal.h"
#include "erdp_hal_gpio.hpp"
using namespace erdp;
using namespace std;
#define SYS_LED_PORT ERDP_GPIOC
#define SYS_LED_PIN ERDP_GPIO_PIN_13
class LED : private GpioDev {
   public:
    LED(ERDP_GpioPort_t port, ERDP_GpioPin_t pin,
        ERDP_Status_t on_level = ERDP_SET)
        : GpioDev(port, pin, ERDP_GPIO_PIN_MODE_OUTPUT, ERDP_GPIO_PIN_PULL_NONE,
                  ERDP_GPIO_SPEED_LOW),
          __on_level(on_level) {}

    void on() {
        write((ERDP_Status_t)__on_level);
        __status = (ERDP_Status_t)__on_level;
    }
    void off() {
        write((ERDP_Status_t)!__on_level);
        __status = (ERDP_Status_t)!__on_level;
    }

    void toggle() {
        __status = (ERDP_Status_t)!__status;
        write(__status);
    }

   private:
    ERDP_Status_t __on_level;
    ERDP_Status_t __status;
};
void Thread::main_thread(void *parm)
{
	// UartDev uart(uart_backon_config,100);
	// uart.set_as_debug_com();
	// printf("gd32 hello word\n");
    LED sys_led(SYS_LED_PORT, SYS_LED_PIN, ERDP_RESET);
	erdp::Thread LED_thread(
        [&sys_led]() {
            while (1) {
                sys_led.toggle();    // Toggle the system LED to indicate the
                                     // system is
                Thread::delay_ms(1000);
            }
        },
        "LED", 6, 128);
    LED_thread.join();

    while (1)
    {
        Thread::delay_ms(10);
    }
}