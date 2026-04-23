#include "erdp_hal_uart.hpp"
namespace erdp
{
    UartDev *UartDev::m_instance[ERDP_UART_NUM] = {nullptr};
    void (*UartDev::putChar)(char c) = erdp_if_uart_putchar;
    extern "C"
    {
        void erdp_uart_irq_handler(ERDP_Uart_t uart)
        {
            UartDev::m_instance[uart]->irqHandler();
        }
    }
}