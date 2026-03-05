#include "erdp_if_gpio.h"
#include "erdp_if_uart.h"
#include "erdp_if_rtos.h"
#include "erdp_if_sys.h"
#include "stdio.h"
#include "erdp_test_cfg.h"


static ERDP_UartGpioCfg_t uart_gpio_cfg = {
    .tx_port = SYS_UART_TX_PORT,
    .tx_pin = SYS_UART_TX_PIN,
    .tx_af = SYS_UART_TX_AF,
    .rx_port = SYS_UART_RX_PORT,
    .rx_pin = SYS_UART_RX_PIN,
    .rx_af = SYS_UART_RX_AF
};

void erdp_uart_irq_handler(ERDP_Uart_t uart) {
    (void)uart;
}

void less_task(void *param) {
    erdp_if_gpio_init(SYS_LED_PORT, SYS_LED_PIN, ERDP_GPIO_PIN_MODE_OUTPUT, ERDP_GPIO_PIN_PULL_NONE, ERDP_GPIO_SPEED_LOW);
    while (1) {
        erdp_if_rtos_delay_ms(1000);
        erdp_if_gpio_write(SYS_LED_PORT, SYS_LED_PIN, ERDP_SET);
        erdp_if_rtos_delay_ms(1000);
        erdp_if_gpio_write(SYS_LED_PORT, SYS_LED_PIN, ERDP_RESET);
    }
}
int main(void) {
    erdp_if_sys_init();
    erdp_if_uart_gpio_init(&uart_gpio_cfg);
    erdp_if_uart_init(SYS_UART_ERDP, SYS_UART_BAUDRATE, ERDP_UART_TX_ONLY, 5);
    erdp_if_uart_set_putchar_com(SYS_UART_ERDP);
    printf("Hello from STM32!\r\n");

    // const uint8_t msg[] = "Hello from STM32!\r\n";
    // erdp_if_uart_send_bytes(SYS_UART_ERDP, msg, sizeof(msg) - 1);
    
    erdp_if_rtos_task_create(less_task, "LED", 256, NULL, 1);
    erdp_if_rtos_start_scheduler();

    while (1) {

    }
    return 0;
}
