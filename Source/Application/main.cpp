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
SpiInfo_t info{
    ERDP_SPI1,
    ERDP_GPIOA,
    ERDP_GPIO_PIN_5,
    GPIO_AF_SPI1,
    ERDP_GPIOA,
    ERDP_GPIO_PIN_7,
    GPIO_AF_SPI1,
    ERDP_GPIOA,
    ERDP_GPIO_PIN_6,
    GPIO_AF_SPI1,
    ERDP_GPIOA,
    ERDP_GPIO_PIN_4,
    GPIO_AF_SPI1,
};

void Thread::mainThread(void *parm) {
    UartDev com;
    com.init(SYS_COM_CFG, 128);
    com.setAsDebugCom();
    SpiDev<ERDP_SPI_MODE_MASTER,ERDP_SPI_DATASIZE_8BIT> spi;
    spi.init(info, SPI_CONFIG,128);
    
    LoggerBase::start();
    printf("Hello World!\n");
    LED sys_led(SYS_LED_PORT, SYS_LED_PIN, ERDP_RESET);
    erdp::Thread LED_thread(
        [&sys_led]() {
            while (1) {
                sys_led.toggle();    // Toggle the system LED to indicate the
                                     // system is
                Thread::sleep(500);
                Debug("LED", "LED toggle");
            }
        },
        "LED", 6, 512);
    LED_thread.join();
    erdp::Thread testThread([]() { printf("testThread run\n"); }, "testThread", 6, 256);
    testThread.join();
    Thread::setKillThreadHook([](Thread *task) {
        Debug("OSAL", "killThreadHook %s", task->getName());
        // printf("killThreadHook %s\n", task->getName());
    });
    Debug("test", "debug message");
    Info("test", "info message");
    Warn("test", "warn message");
    Error("test", "error message");
    Fatal("test", "fatal message");
    Debug("size", "size %d", sizeof(unsigned int));
    uint8_t data[5] = {0x01,0x02,0x03,0x04,0x05};
    while (1) {
        Debug("test", "run...");
        spi.csLow();
        spi.send(data,5);
        spi.csHigh();
        Thread::sleep(1000);
    }
}
