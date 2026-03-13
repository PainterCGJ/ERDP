/*
//D:\Users\painter\AppData\Local\Keil_v5\ARM\ARMCLANG\bin\fromelf.exe --bin --output ./Build/erdp.bin ./Build/erdp.axf
*/
#include "erdp_hal_uart.hpp"
#include "erdp_hal_gpio.hpp"
#include "erdp_osal.hpp"
#include <vector>
#include "log_service.hpp"
// #include "log_adapter.hpp"

using namespace erdp;
using namespace std;
#define SYS_LED_PORT ERDP_GPIOC
#define SYS_LED_PIN ERDP_GPIO_PIN_0


class LED : private GpioDev {
   public:
    LED(ERDP_GpioPort_t port, ERDP_GpioPin_t pin, ERDP_Status_t on_level = ERDP_SET)
        : GpioDev(port, pin, ERDP_GPIO_PIN_MODE_OUTPUT, ERDP_GPIO_PIN_PULL_NONE, ERDP_GPIO_SPEED_LOW),
          m_onLevel(on_level),
          m_status((ERDP_Status_t)!on_level) {}

    void on() {
        write((ERDP_Status_t)m_onLevel);
        m_status = (ERDP_Status_t)m_onLevel;
    }
    void off() {
        write((ERDP_Status_t)!m_onLevel);
        m_status = (ERDP_Status_t)!m_onLevel;
    }

    void toggle() {
        m_status = (ERDP_Status_t)!m_status;
        write(m_status);
    }

   private:
    ERDP_Status_t m_onLevel;
    ERDP_Status_t m_status;
};

void Thread::mainThread(void *parm)
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
                Thread::sleep(500);
            }
        },
        "LED", 6, 128);
    LED_thread.join();

    while (1)
    {
        Thread::sleep(10);
    }
}
