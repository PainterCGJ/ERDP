#include "erdp_hal_uart.hpp"
namespace erdp
{
    UartBase *UartBase::__instance[ERDP_UART_MAX];

    extern "C"
    {
        void erdp_irq_handler(ERDP_Uart_t uart)
        {
            UartBase::__instance[uart]->__irq_handler();
        }
    }
}