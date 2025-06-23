#include "erdp_hal_uart.hpp"
namespace erdp
{
    UartDev *UartDev::__instance[ERDP_UART_NUM];
    UartDev *UartDev::__debug_com = nullptr;

    extern "C"
    {
        void erdp_uart_irq_handler(ERDP_Uart_t uart)
        {
            UartDev::__instance[uart]->__irq_handler();
        }

        void _putchar(char character)
        {
            if(UartDev::__debug_com != nullptr)
            {
                UartDev::__debug_com->send((uint8_t *)&character, 1);
            }
        }
    }
}