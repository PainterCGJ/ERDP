#include "erdp_hal_uart.hpp"
namespace erdp
{
    UartBase *UartBase::__instance[ERDP_UART_MAX];
    UartBase *UartBase::__debug_com = nullptr;

    extern "C"
    {
        void erdp_uart_irq_handler(ERDP_Uart_t uart)
        {
            UartBase::__instance[uart]->__irq_handler();
        }

        void _putchar(char character)
        {
            if(UartBase::__debug_com != nullptr)
            {
                UartBase::__debug_com->send((uint8_t *)&character, 1);
            }
        }
    }
}