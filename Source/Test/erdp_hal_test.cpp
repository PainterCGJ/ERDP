#include "erdp_hal_gpio.hpp"
#include "erdp_hal_uart.hpp"
#include "erdp_if_uart.h"
#include "erdp_osal.hpp"
#include "erdp_test_cfg.h"
using namespace erdp;
class LED : private GpioDev {
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
    UartDev com;
    UartConfig_t cfg;
    cfg.uart = SYS_UART_ERDP;
    cfg.baudrate = 115200;
    cfg.mode = ERDP_UART_TX_RX;
    cfg.txPort = SYS_UART_TX_PORT;
    cfg.txPin = SYS_UART_TX_PIN;
    cfg.txAf = SYS_UART_TX_AF;
    cfg.rxPort = SYS_UART_RX_PORT;
    cfg.rxPin = SYS_UART_RX_PIN;
    cfg.rxAf = SYS_UART_RX_AF;
    cfg.priority = 10;
    com.init(cfg, 128);
    com.setAsDebugCom();
    printf("Hello World!\n");
    Thread ledThread([](){
        LED led(SYS_LED_PORT, SYS_LED_PIN);
        while (true) {
            led.toggle();
            sleep(500);
        }
    }, "ledThread",5);
    ledThread.join();
    std::vector<uint8_t> buffer;
    while (true) {
        sleep(1000);
        if (com.recv(buffer)) {
            for (auto &byte : buffer) {
                printf("%02X ", byte);
            }
            printf("\n");
        }
    }
}