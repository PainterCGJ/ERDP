#ifndef ERDP_HAL_UART_GPP
#define ERDP_HAL_UART_GPP

#include "erdp_hal.h"
#include "erdp_if_uart.h"
#include "erdp_if_gpio.h"

#include <type_traits>

namespace erdp
{
    extern "C"
    {
        void erdp_uart_irq_handler(ERDP_Uart_t uart);
    }
    typedef struct
    {
        ERDP_Uart_t uart;     // UART number
        uint32_t baudrate;    // Baudrate for the UART
        ERDP_UartMode_t mode; // Mode for the UART (TX/RX, TX only, RX only)

        ERDP_GpioPort_t tx_port; // GPIO port for TX pin
        ERDP_GpioPin_t tx_pin;   // GPIO pin for TX pin
        uint32_t tx_af;          // Alternate function for TX pin

        ERDP_GpioPort_t rx_port; // GPIO port for RX pin
        ERDP_GpioPin_t rx_pin;   // GPIO pin for RX pin
        uint32_t rx_af;          // Alternate function for RX pin

        uint8_t priority; // Priority for the UART receive interrupt

    } UartConfig_t;

    class VoidClass
    {
    };

    class UartBase
    {
        friend void erdp_uart_irq_handler(ERDP_Uart_t uart);

    public:
        UartBase() {}
        UartBase(UartConfig_t &config) {}

        ERDP_Uart_t __uart = ERDP_UART0;            // Default to UART0
        static UartBase *__instance[ERDP_UART_MAX]; // Array to hold instances for each UART
        uint8_t __data;

        void init(UartConfig_t &config)
        {
            __init(config);
        }

        void send(uint8_t *data, uint32_t len)
        {
            erdp_if_uart_send_bytes(__uart, data, len);
        }

    private:
        void __init(UartConfig_t &config)
        {
            ERDP_UartGpioCfg_t gpio_cfg = {
                .tx_port = config.tx_port,
                .tx_pin = config.tx_pin,
                .tx_af = config.tx_af,
                .rx_port = config.rx_port,
                .rx_pin = config.rx_pin,
                .rx_af = config.rx_af};
            __uart = config.uart;

            __instance[__uart] = this; // Store the instance for the IRQ handler

            erdp_if_uart_init(config.uart, config.baudrate, config.mode, config.priority);
            erdp_if_uart_gpio_init(&gpio_cfg);
        }

        void __irq_handler()
        {
            erdp_if_uart_read_byte(__uart, &__data);
            erdp_if_uart_send_bytes(__uart, &__data, 1);
            __usr_irq_service(); // Call the user-defined IRQ service
        }

        virtual void __usr_irq_service() = 0;
    };

    template <class T = VoidClass>
    class UartDev;

    template <class T>
    class UartDev : public UartBase
    {
    public:
        UartDev() : UartBase() {}
        UartDev(UartConfig_t &config) : UartBase(config)
        {
            init(config);
        }

        template <class U = T, class = std::enable_if_t<!std::is_same_v<U, VoidClass>>>
        void set_usr_irq_handler(T *obj, void (T::*func)(void))
        {
            __obj = obj;
            __usr_irq_handler = func;
        }

    private:
        template <class U = T>
        using obj_type = std::conditional_t<!std::is_same_v<U, VoidClass>, T *, void *>;
        obj_type<> __obj = nullptr;
        template <class U = T>
        using handler_type = std::conditional_t<!std::is_same_v<U, VoidClass>, void (T::*)(), void *>;
        handler_type<> __usr_irq_handler = nullptr;

        void __usr_irq_service() override
        {
            if constexpr (!std::is_same_v<T, VoidClass>)
            {
                if (__usr_irq_handler != nullptr)
                {
                    (__obj->*__usr_irq_handler)(); // Call the user-defined function
                }
            }
        }
    };
} // namespace erdp

#endif // ERDP_HAL_UART_GPP
