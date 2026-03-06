#include "erdp_hal_gpio.hpp"
#include "erdp_if_uart.h"
#include "erdp_osal.hpp"
#include "erdp_test_cfg.h"
extern "C" {
    void erdp_uart_irq_handler(ERDP_Uart_t uart) { (void)uart; }
}
class LED : private erdp::GpioDev {
   public:
    LED(ERDP_GpioPort_t port, ERDP_GpioPin_t pin, ERDP_Status_t on_level = ERDP_SET)
        : GpioDev(port, pin, ERDP_GPIO_PIN_MODE_OUTPUT, ERDP_GPIO_PIN_PULL_NONE, ERDP_GPIO_SPEED_LOW),
          m_on_level(on_level),
          m_status((ERDP_Status_t)!on_level) {}

    void on() {
        write((ERDP_Status_t)m_on_level);
        m_status = (ERDP_Status_t)m_on_level;
    }
    void off() {
        write((ERDP_Status_t)!m_on_level);
        m_status = (ERDP_Status_t)!m_on_level;
    }

    void toggle() {
        m_status = (ERDP_Status_t)!m_status;
        write(m_status);
    }

   private:
    ERDP_Status_t m_on_level;
    ERDP_Status_t m_status;
};
void erdp::Thread::mainThread(void *parm) {
    Thread ledThread([](){
        LED led(SYS_LED_PORT, SYS_LED_PIN);
        while (true) {
            led.toggle();
            sleep(500);
        }
    }, "ledThread",5);
    ledThread.join();
    while (true) {
        sleep(1000);
    }
}