#ifndef __ERDP_IF_EXTI_H__
#define __ERDP_IF_EXTI_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "erdp_interface.h"
#include "erdp_if_gpio.h"

    typedef enum
    {
        ERDP_EXTI_RAISING_EDGE = 0, // Rising edge detection mode
        ERDP_EXTI_FALLING_EDGE,     // Falling edge detection mode
        ERDP_EXTI_BOTH_EDGE,        // Both edge detection mode
    } ERDP_ExtiEdage_t;

    void erdp_if_exti_init(ERDP_GpioPort_t port, ERDP_GpioPin_t pin, ERDP_ExtiEdage_t edge, uint8_t priority);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ERDP_IF_EXTI_H__
