/*
//D:\Users\painter\AppData\Local\Keil_v5\ARM\ARMCLANG\bin\fromelf.exe --bin --output ./Build/erdp.bin ./Build/erdp.axf
*/
#include <vector>

#include "board.h"
#include "erdp_hal_gpio.hpp"
#include "erdp_hal_spi.hpp"
#include "erdp_hal_uart.hpp"
#include "erdp_osal.hpp"
#include "log_service.hpp"
#include "lcd_service.hpp"


// #include "log_adapter.hpp"

using namespace erdp;
using namespace std;

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

void Thread::mainThread(void *parm) {
    UartDev com;
    com.init(SYS_COM_CFG, 128);
    com.setAsDebugCom();
    LoggerBase::start();
    LED sys_led(SYS_LED_PORT, SYS_LED_PIN, ERDP_RESET);
    erdp::Thread LED_thread(
        [&sys_led]() {
            while (1) {
                sys_led.toggle();    // Toggle the system LED to indicate the
                                     // system is
                Thread::sleep(500);
            }
        },
        "LED", 6, 512);
    LED_thread.join();
    Info("test", "info message");
    LCDService lcdService;
    lcdService.start();
    while (1) {
        Debug("test", "run...");
       
        Thread::sleep(1000);
    }
}
