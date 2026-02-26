
/* erdp include */
#include "erdp_if_gpio.h"

/* platform include */
#include "gd32f4xx_gpio.h"
#include "gd32f4xx_rcu.h"

const static uint32_t port_instance[ERDP_GPIO_MAX] = {
    (uint32_t)GPIOA,
    (uint32_t)GPIOB,
    (uint32_t)GPIOC,
    (uint32_t)GPIOD,
    (uint32_t)GPIOE,
    (uint32_t)GPIOF,
    (uint32_t)GPIOG,
    (uint32_t)GPIOH,
    (uint32_t)GPIOI,
};

const static uint32_t pin_instance[ERDP_GPIO_PIN_MAX] = {
    (uint32_t)GPIO_PIN_0,
    (uint32_t)GPIO_PIN_1,
    (uint32_t)GPIO_PIN_2,
    (uint32_t)GPIO_PIN_3,
    (uint32_t)GPIO_PIN_4,
    (uint32_t)GPIO_PIN_5,
    (uint32_t)GPIO_PIN_6,
    (uint32_t)GPIO_PIN_7,
    (uint32_t)GPIO_PIN_8,
    (uint32_t)GPIO_PIN_9,
    (uint32_t)GPIO_PIN_10,
    (uint32_t)GPIO_PIN_11,
    (uint32_t)GPIO_PIN_12,
    (uint32_t)GPIO_PIN_13,
    (uint32_t)GPIO_PIN_14,
    (uint32_t)GPIO_PIN_15,
};

const static uint32_t port_clk[ERDP_GPIO_MAX] = {
    (uint32_t)RCU_GPIOA,
    (uint32_t)RCU_GPIOB,
    (uint32_t)RCU_GPIOC,
    (uint32_t)RCU_GPIOD,
    (uint32_t)RCU_GPIOE,
    (uint32_t)RCU_GPIOF,
    (uint32_t)RCU_GPIOG,
    (uint32_t)RCU_GPIOH,
    (uint32_t)RCU_GPIOI,
};

uint32_t erdp_if_gpio_get_port(ERDP_GpioPort_t port)
{
    return port_instance[port];
}

uint32_t erdp_if_gpio_get_pin(ERDP_GpioPin_t pin)
{
    return pin_instance[pin];
}

uint32_t erdp_if_gpio_get_PCLK(ERDP_GpioPort_t port)
{
    return port_clk[port];
}

void erdp_if_gpio_init(ERDP_GpioPort_t port, ERDP_GpioPin_t pin, ERDP_GpioPinMode_t mode, ERDP_GpioPinPull_t pull, ERDP_GpioSpeed_t speed)
{

    // Initialize the GPIO pin
    uint32_t gpio_periph = erdp_if_gpio_get_port(port);
    uint32_t gpio_pin = erdp_if_gpio_get_pin(pin);
    uint32_t gpio_pclk = erdp_if_gpio_get_PCLK(port);
    uint32_t gpio_mode = 0;
    uint32_t gpio_pull = 0;
    uint32_t gpio_speed = 0;

    switch (mode)
    {
    case ERDP_GPIO_PIN_MODE_INPUT:
        gpio_mode = GPIO_MODE_INPUT;
        break;
    case ERDP_GPIO_PIN_MODE_OUTPUT:
        gpio_mode = GPIO_MODE_OUTPUT;
        break;
    case ERDP_GPIO_PIN_MODE_ALTERNATE:
        gpio_mode = GPIO_MODE_AF;
        break;
    case ERDP_GPIO_PIN_MODE_ANALOG:
        gpio_mode = GPIO_MODE_ANALOG;
        break;
    default:
        break;
    }

    switch (pull)
    {
    case ERDP_GPIO_PIN_PULL_NONE:
        gpio_pull = GPIO_PUPD_NONE;
        break;
    case ERDP_GPIO_PIN_PULLUP:
        gpio_pull = GPIO_PUPD_PULLUP;
        break;
    case ERDP_GPIO_PIN_PULLDOWN:
        gpio_pull = GPIO_PUPD_PULLDOWN;
        break;
    default:
        break;
    }

    switch (speed)
    {
    case ERDP_GPIO_SPEED_LOW:
        gpio_speed = GPIO_OSPEED_2MHZ;
        break;
    case ERDP_GPIO_SPEED_MEDIUM:
        gpio_speed = GPIO_OSPEED_25MHZ;
        break;
    case ERDP_GPIO_SPEED_HIGH:
        gpio_speed = GPIO_OSPEED_50MHZ;
        break;
    case ERDP_GPIO_SPEED_MAX:
        gpio_speed = GPIO_OSPEED_MAX;
        break;
    }

    rcu_periph_clock_enable(gpio_pclk);                         // Enable the GPIO port clock
    gpio_mode_set(gpio_periph, gpio_mode, gpio_pull, gpio_pin); // Set the GPIO mode and pull-up/pull-down
    // if(mode == ERDP_GPIO_PIN_MODE_OUTPUT || mode == ERDP_GPIO_PIN_MODE_ALTERNATE)
    gpio_output_options_set(gpio_periph, GPIO_OTYPE_PP, gpio_speed, gpio_pin); // Set the GPIO output type and speed
}

void erdp_if_gpio_af_cfg(ERDP_GpioPort_t port, ERDP_GpioPin_t pin, uint32_t af)
{
    uint32_t gpio_periph = erdp_if_gpio_get_port(port);
    uint32_t gpio_pin = erdp_if_gpio_get_pin(pin);

    // Set the alternate function for the GPIO pin
    gpio_af_set(gpio_periph, af, gpio_pin);
}

void erdp_if_gpio_write(ERDP_GpioPort_t port, ERDP_GpioPin_t pin, ERDP_Status_t value)
{
    uint32_t gpio_periph = erdp_if_gpio_get_port(port);
    uint32_t gpio_pin = erdp_if_gpio_get_pin(pin);

    if (value == ERDP_SET)
    {
        gpio_bit_set(gpio_periph, gpio_pin); // Set the GPIO pin
    }
    else
    {
        gpio_bit_reset(gpio_periph, gpio_pin); // Reset the GPIO pin
    }
}

ERDP_Status_t erdp_if_gpio_read(ERDP_GpioPort_t port, ERDP_GpioPin_t pin)
{
    uint32_t gpio_periph = erdp_if_gpio_get_port(port);
    uint32_t gpio_pin = erdp_if_gpio_get_pin(pin);

    if (gpio_input_bit_get(gpio_periph, gpio_pin) == SET)
    {
        return ERDP_SET; // Pin is set
    }
    else
    {
        return ERDP_RESET; // Pin is reset
    }
}
