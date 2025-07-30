/*

*/
#include "erdp_hal_uart.hpp"
#include "erdp_hal_gpio.hpp"
#include "erdp_hal_spi.hpp"
#include "erdp_osal.hpp"
#include "gd32f4xx.h"
#include "systick.h"
#include "erdp_assert.h"
#include "erdp_hal_exti.hpp"
// #include "printf.h"
#include <vector>
#include "log_adapter.hpp"
#include "log_adapter.hpp"

using namespace erdp;
using namespace std;
UartConfig_t uart_config{
    .uart = ERDP_UART2,
    .baudrate = 115200,
    .mode = ERDP_UART_TX_RX,
    .tx_port = ERDP_GPIOB,
    .tx_pin = ERDP_GPIO_PIN_10,
    .tx_af = GPIO_AF_7,
    .rx_port = ERDP_GPIOB,
    .rx_pin = ERDP_GPIO_PIN_11,
    .rx_af = GPIO_AF_7,
    .priority = 6};

SpiInfo_t spi3_info = {
    .spi = ERDP_SPI3,

    .sck_port = ERDP_GPIOE,
    .sck_pin = ERDP_GPIO_PIN_2,
    .sck_af = GPIO_AF_5,

    .mosi_port = ERDP_GPIOE,
    .mosi_pin = ERDP_GPIO_PIN_6,
    .mosi_af = GPIO_AF_5,

    .miso_port = ERDP_GPIOE,
    .miso_pin = ERDP_GPIO_PIN_5,
    .miso_af = GPIO_AF_5,

    .cs_port = ERDP_GPIOE,
    .cs_pin = ERDP_GPIO_PIN_4,
    .cs_af = GPIO_AF_5,
};

SpiInfo_t spi1_info = {
    .spi = ERDP_SPI1,

    .sck_port = ERDP_GPIOC,
    .sck_pin = ERDP_GPIO_PIN_7,
    .sck_af = GPIO_AF_5,

    .mosi_port = ERDP_GPIOC,
    .mosi_pin = ERDP_GPIO_PIN_1,
    .mosi_af = GPIO_AF_7,

    .miso_port = ERDP_GPIOC,
    .miso_pin = ERDP_GPIO_PIN_2,
    .miso_af = GPIO_AF_5,

    .cs_port = ERDP_GPIOB,
    .cs_pin = ERDP_GPIO_PIN_12,
    .cs_af = GPIO_AF_5,
};

SpiConfig_t spi_config = {
    .clk_mode = ERDP_SPI_CLKMODE_0,
    .endian = ERDP_SPI_ENDIAN_MSB,
    .prescale = SPI_PSC_16,
    .priority = 7,
};

#ifdef ERDP_ENABLE_RTOS
class UartTask : public Thread
{
public:
    UartTask() : Thread("UartTask", 2, 512)
    {
    }
    Semaphore<BINARY_TAG> sem;
    UartDev uart_dev;
    void uart_irq_handler()
    {
        sem.give();
    }
    void thread_code() override
    {
        uart_dev.init(uart_config, 20);
        uart_dev.set_usr_irq_handler([this]()
        {
            uart_irq_handler();
        });
        uart_dev.set_as_debug_com();
        vector<uint8_t> rx_buffer;
        // rx_buffer.reserve(10);
        printf("UartTask constructor\n");
        while (1)
        {
            uint8_t data;
            if (uart_dev.recv(data))
            {
                printf("data = %x\n", data);
            }
            if (sem.take(0))
            {
                Logger::t("uart","uart sem take ok\n");
            }
            delay_ms(2);
        }
    }
};

class SpiTask : public Thread
{
public:
    SpiTask() : Thread("SpiTask", 3, 512),
                sub_thread([this]()
                           {
                    printf("sub_thread constructor\n");
                    while(1) {
                        Logger::t("sub_thread","sub_thread running");
                        delay_ms(1000);
                    } }, "sub_thread", 4, 512)
    {
        sub_thread.join();
    }

    Semaphore<BINARY_TAG> sem;
    const char str[14] = "hello world\n\0";
    Thread sub_thread; // Declaration only
    void thread_code() override
    {
        printf("SpiTask constructor\n");

        SpiDev<ERDP_SPI_MODE_MASTER> spi_master(spi1_info, spi_config, 50);
        SpiDev<ERDP_SPI_MODE_SLAVE> spi_slave(spi3_info, spi_config, 50, 50);
        spi_slave.set_usr_rx_irq_handler([this](uint8_t data)
                                         { sem.give(); });

        std::vector<uint8_t> master_tx_buffer8 = {0x01, 0x02, 0x03};
        std::vector<uint8_t> slave_tx_buffer8 = {0x0A, 0x0B, 0x0C, 0x0D};
        uint8_t data;

        spi_master.cs_low();
        while (1)
        {
            if (spi_slave.is_send_complete())
            {
                spi_slave.send(slave_tx_buffer8.data(), slave_tx_buffer8.size());
            }
            if (spi_master.send_recv(master_tx_buffer8.data(), master_tx_buffer8.size(), slave_tx_buffer8.size()))
            {
                while (spi_master.rx_buffer.pop(data))
                {
                    printf("%x ", data);
                }
                printf("\n");
            }
            while (spi_slave.rx_buffer.pop(data))
            {
                printf("slave data = %x\n", data);
            }
            if (sem.take(0))
            {
                printf("spi sem take ok\n");
            }

            master_tx_buffer8[0] += 1;
            master_tx_buffer8[1] += 1;
            master_tx_buffer8[2] += 1;
            slave_tx_buffer8[0] += 1;
            slave_tx_buffer8[1] += 1;
            slave_tx_buffer8[2] += 1;
            slave_tx_buffer8[3] += 1;
            Thread::delay_ms(500);
        }
    }
};
void printRunningTasks()
{
    // 需要确保FreeRTOSConfig.h中配置了:
    // #define configUSE_TRACE_FACILITY 1
    // #define configUSE_STATS_FORMATTING_FUNCTIONS 1

    // char *pcWriteBuffer = (char *)pvPortMalloc(512); // 分配缓冲区
    // if (pcWriteBuffer != NULL)
    // {
    //     printf("Task Name\tStatus\tPrio\tStack\tTask#\n");
    //     printf("****************************************\n");
    //     vTaskList(pcWriteBuffer);      // 获取任务列表
    //     printf("%s\n", pcWriteBuffer); // 打印任务信息
    //     vPortFree(pcWriteBuffer);      // 释放缓冲区
    // }
}
extern "C"
{
    void vApplicationMallocFailedHook(void)
    {
        // 内存分配失败处理
        printf("Error: Malloc Failed!\n");
        taskDISABLE_INTERRUPTS();
        for (;;)
            ; // 死循环，可根据需要改为其他错误处理
    }
}

void Thread::main_thread(void *parm)
{

    Logger log_service;
    log_service.start();

    GpioDev Led(ERDP_GPIOC, ERDP_GPIO_PIN_6, ERDP_GPIO_PIN_MODE_OUTPUT, ERDP_GPIO_PIN_PULL_NONE, ERDP_GPIO_SPEED_HIGH);
    UartTask uart_task;
    uart_task.join();

    SpiTask spi_task;
    // spi_task.join();
    while (UartDev::get_debug_com() == nullptr)
    {
        Thread::delay_ms(100);
    }

    Semaphore<BINARY_TAG> sem;
    Exti exti;
    exti.init(ERDP_GPIOA, ERDP_GPIO_PIN_8, ERDP_EXTI_FALLING_EDGE, 6);
    exti.set_usr_irq_hendler([&sem]()
                             { sem.give(); });

    

    Logger::i("main","Logger started");
    Logger::t("main","Logger started");
    Logger::d("main","Logger started");
    Logger::w("main","Logger started");
    Logger::e("main","Logger started");

    while (1)
    {
        Led.write(ERDP_SET);
        Thread::delay_ms(500);
        Led.write(ERDP_RESET);
        Thread::delay_ms(500);
        if (sem.take(0))
        {
            Logger::t("main","exti sem take ok\n");
        }
        // printRunningTasks();
    }
}
#else

SpiInfo_t _info;
#include "erdp_heap.hpp"
int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    systick_config();
    SpiMasterBase<ERDP_SPI_DATASIZE_8BIT> *spi_master;
    SpiSlaveBase<ERDP_SPI_DATASIZE_8BIT> *spi_slave;
    UartDev<> uart_dev;
    uint8_t data;
    uart_dev.init(uart_config, 20);
    uart_dev.set_as_debug_com();
    printf("UartTask constructor\n");
    LED Led(ERDP_GPIOC, ERDP_GPIO_PIN_6);

    GpioDev PA8(ERDP_GPIOA, ERDP_GPIO_PIN_8, ERDP_GPIO_PIN_MODE_INPUT, ERDP_GPIO_PIN_PULLUP);
    bool is_master = PA8.read();
    if (is_master)
    {
        printf("is_master\n");
        spi_master = new SpiMasterBase<ERDP_SPI_DATASIZE_8BIT>(spi1_info, spi_config, 50);
        if (spi_master == nullptr)
        {
            printf("spi_master is nullptr\n");
        }
        spi_slave = new SpiSlaveBase<ERDP_SPI_DATASIZE_8BIT>(spi3_info, spi_config, 50, 50);
        if (spi_slave == nullptr)
        {
            printf("spi_slave is nullptr\n");
        }
    }
    else
    {
        printf("is_slave\n");
        spi_slave = new SpiSlaveBase<ERDP_SPI_DATASIZE_8BIT>(spi3_info, spi_config, 50, 50);
        if (spi_slave == nullptr)
        {
            printf("spi_slave is nullptr\n");
        }
    }
    std::vector<uint8_t> master_tx_buffer8 = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    std::vector<uint8_t> slave_tx_buffer8 = {0x0A, 0x0B, 0x0C, 0x0D};
    // std::function<void()> __usr_rx_irq_handler;

    // tx_buffer.push_back(0x0A);

    // while (1)
    while (1)
    {
        if (is_master)
        {
            if (spi_slave->is_send_complete())
            {
                spi_slave->send(slave_tx_buffer8.data(), slave_tx_buffer8.size());
            }
            if (spi_master->recv(slave_tx_buffer8.size()))
            {
                while (spi_master->rx_buffer.pop(data))
                {
                    printf("%x ", data);
                }
                printf("\n");
            }
            // spi_master->cs_low();
            // spi_master->send(master_tx_buffer8.data(), master_tx_buffer8.size());
            // spi_master->cs_high();
            Led.on();
            delay_1ms(200);
            Led.off();
            delay_1ms(200);
            // while (spi_slave->rx_buffer.pop(data))
            // {
            //     printf("%x ", data);
            // }
        }
        else
        {
            while (spi_slave->rx_buffer.pop(data))
            {
                printf("%x ", data);
            }
        }
        while (uart_dev.recv(data))
        {
            printf("%x ", data);
        }
    }
}

#endif
