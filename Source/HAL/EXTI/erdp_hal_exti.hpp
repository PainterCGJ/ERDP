#ifndef __ERDP_HAL_EXTI_HPP__
#define __ERDP_HAL_EXTI_HPP__
#include "erdp_hal.hpp"
#include "erdp_if_exti.h"
namespace erdp
{
    extern "C"
    {
        void erdp_exti_irq_handler(ERDP_GpioPin_t pin);
    }
    class Exti
    {
    public:
        friend void erdp_exti_irq_handler(ERDP_GpioPin_t pin);
        Exti() = default;
        Exti(const Exti &) = delete;
        Exti &operator=(const Exti &) = delete;
        Exti(Exti &&) = delete;
        Exti &operator=(Exti &&) = delete;

        Exti(ERDP_GpioPort_t port, ERDP_GpioPin_t pin, ERDP_ExtiEdage_t edge, uint8_t priority)
        {
            erdp_if_exti_init(port, pin, edge, priority);
        }
        void init(ERDP_GpioPort_t port, ERDP_GpioPin_t pin, ERDP_ExtiEdage_t edge, uint8_t priority)
        {
            __exti_instance[pin] = this;
            erdp_if_exti_init(port, pin, edge, priority);
        }
        ~Exti() = default;

        void set_usr_irq_hendler(std::function<void()> usr_irq_hendler){
            __usr_irq_hendler = usr_irq_hendler;
        }

        void clear_usr_irq_hendler(){
            __usr_irq_hendler = nullptr;
        }

    private:
        static Exti *__exti_instance[ERDP_GPIO_PIN_MAX];
        std::function<void()> __usr_irq_hendler = nullptr;

        void __irq_handler()
        {
            if (__usr_irq_hendler != nullptr)
            {
                __usr_irq_hendler();
            }
        }
    };
} // namespace erdp
#endif // __ERDP_HAL_EXTI_HPP__