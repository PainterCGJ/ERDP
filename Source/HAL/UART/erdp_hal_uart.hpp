#ifndef __ERDP_HAL_UART_GPP__
#define __ERDP_HAL_UART_GPP__

#include <type_traits>
#include <vector>
#include "erdp_if_gpio.h"
#include "erdp_if_uart.h"
#include "erdp_osal.hpp"

namespace erdp {
    extern "C" {
        void erdp_uart_irq_handler(ERDP_Uart_t uart);
    }

    typedef struct {
        ERDP_Uart_t uart;        // UART number
        uint32_t baudrate;       // Baudrate for the UART
        ERDP_UartMode_t mode;    // Mode for the UART (TX/RX, TX only, RX only)

        ERDP_GpioPort_t txPort;    // GPIO port for TX pin
        ERDP_GpioPin_t txPin;      // GPIO pin for TX pin
        uint32_t txAf;             // Alternate function for TX pin

        ERDP_GpioPort_t rxPort;    // GPIO port for RX pin
        ERDP_GpioPin_t rxPin;      // GPIO pin for RX pin
        uint32_t rxAf;             // Alternate function for RX pin

        uint8_t priority;    // Priority for the UART receive interrupt

    } UartConfig_t;

    class UartDev {
        friend void erdp_uart_irq_handler(ERDP_Uart_t uart);
#define GET_SYS_TICK() erdp_if_rtos_get_1ms_timestamp()

       public:
        UartDev() {}
        UartDev(const UartConfig_t &config, size_t recvBufferSize) { devInit(config, recvBufferSize); }

        void init(const UartConfig_t &config, size_t recvBufferSize) { devInit(config, recvBufferSize); }

        void send(const uint8_t *data, uint32_t len) const { erdp_if_uart_send_bytes(m_uart, data, len); }

        void send(const std::vector<uint8_t> &data) const { erdp_if_uart_send_bytes(m_uart, data.data(), data.size()); }

        bool recv(std::vector<uint8_t> &buffer, uint32_t timeout = 5) {
            bool ret = false;
            uint8_t data;
            buffer.clear();
            uint32_t startTime = GET_SYS_TICK();

            while (GET_SYS_TICK() - startTime < timeout) {
                if (m_recvBuffer.pop(data)) {
                    buffer.push_back(data);
                    ret = true;
                    startTime = GET_SYS_TICK();    // Reset the timer on successful receive
                }
            }

            return ret;
        }
        bool recv(uint8_t &data) { return m_recvBuffer.pop(data); }

        void setUsrIrqFunc(std::function<void()> usrIrqFunc) { m_usrIrqFunc = usrIrqFunc; }

        void setAsDebugCom() { erdp_if_uart_set_putchar_com(m_uart); }

        static void (*putChar)(char c);
       private:
        ERDP_Uart_t m_uart = ERDP_UART0;              // Default to UART0
        static UartDev *m_instance[ERDP_UART_NUM];    // Array to hold instances for each UART
        uint8_t m_data;
        RingBuffer<uint8_t> m_recvBuffer;
        std::function<void()> m_usrIrqFunc = nullptr;

        void devInit(const UartConfig_t &config, size_t recvBufferSize) {
            if (!m_recvBuffer.init(recvBufferSize)) {
                erdp_assert(false);
                return;
            }
            ERDP_UartGpioCfg_t gpioCfg = { config.txPort,
                                           config.txPin,
                                           config.txAf,
                                           config.rxPort,
                                           config.rxPin,
                                           config.rxAf};
            m_uart = config.uart;

            m_instance[m_uart] = this;    // Store the instance for the IRQ handler

            erdp_if_uart_init(config.uart, config.baudrate, config.mode, config.priority);
            erdp_if_uart_gpio_init(&gpioCfg);
        }

        void irqHandler() {
            erdp_if_uart_read_byte(m_uart, &m_data);
            if (m_recvBuffer.push(m_data)) {
            }

            if (m_usrIrqFunc != nullptr) {
                m_usrIrqFunc();    // Call the user-defined function
            }
        }
    };
}    // namespace erdp

#endif    // ERDP_HAL_UART_GPP
