#include "lcd_service.hpp"

#include "board.h"
#include "erdp_if_uart.h"
#include "erdp_interface.h"
#include "log_service.hpp"
#include "lvgl.h"
#include "thread_config.h"
#include "erdp_hal_uart.hpp"

using namespace erdp;

OS_Hook LCDService::m_tickHook = {.hook = tickHook, .param = nullptr};
SpiDev<ERDP_SPI_MODE_MASTER>* LCDService::m_spi = nullptr;
GpioDev LCDService::m_dc;

LCDService::LCDService() {
    m_pThread = std::make_unique<Thread>(
        Thread([this]() { lcdThreadFunc(); }, "lcd_thread", LCD_SERVICE_PRIO, LCD_THREAD_STACK_SIZE));
}

LCDService::~LCDService() {}

void LCDService::start() {
    m_pThread->join();
}

void LCDService::tickHook(void* param) { lv_tick_inc(1); }

void LCDService::lvglSendCmd(lv_display_t* disp, const uint8_t* cmd, size_t cmd_size, const uint8_t* param,
                             size_t param_size) {
    for (size_t i = 0; i < cmd_size; i++) {
        erdp_if_uart_putchar("Ccmd"[i % 4]);  // 调试标记
    }
    m_spi->csLow();
    m_dc.write(ERDP_RESET);
    m_spi->send(cmd, cmd_size);
    if (param_size > 0) {
        m_dc.write(ERDP_SET);
        m_spi->send(param, param_size);
    }
    m_spi->csHigh();
    for (size_t i = 0; i < 4; i++) {
        erdp_if_uart_putchar("Cend"[i]);  // 调试标记
    }
}

void LCDService::lvglSendColor(lv_display_t* disp, const uint8_t* cmd, size_t cmd_size, uint8_t* param,
                               size_t param_size) {
    m_spi->csLow();
    m_dc.write(ERDP_SET);
    m_spi->send(param, param_size);
    m_spi->csHigh();
}

void LCDService::lcdThreadFunc() {

    // SPI init
    m_spi = new SpiDev<ERDP_SPI_MODE_MASTER>(LCD_SPI_INFO, LCD_SPI_CONFIG, 128);

    // GPIO init
    m_blacklight.init(LCD_BL_PORT, LCD_BL_PIN, ERDP_GPIO_PIN_MODE_OUTPUT);
    m_blacklight.write(ERDP_RESET);
    m_dc.init(LCD_DC_PORT, LCD_DC_PIN, ERDP_GPIO_PIN_MODE_OUTPUT);
    m_dc.write(ERDP_RESET);
    m_rst.init(LCD_RST_PORT, LCD_RST_PIN, ERDP_GPIO_PIN_MODE_OUTPUT);
    m_rst.write(ERDP_RESET);
    Thread::sleep(100);
    m_rst.write(ERDP_SET);

    // Tick hook init
    Thread::setTickHook(&m_tickHook);
    lv_init();    
    lv_delay_set_cb([](uint32_t ms) { erdp::Thread::sleep(ms); });  // 使用 FreeRTOS 延迟
    UartDev::putChar('1');  // 调试
    m_disp = lv_st7789_create(240, 320,            // 分辨率
                              LV_LCD_FLAG_NONE,    // 标志（旋转、颜色顺序等）
                              lvglSendCmd,         // 命令回调
                              lvglSendColor        // 颜色数据回调
    );
    UartDev::putChar('2');  // 调试

    Debug("LCD", "LCD init success");

    m_pLvglThread = std::make_unique<Thread>(Thread(
        []() {
            while (true) {
                lv_timer_handler();
                Thread::sleep(20);
            }
        },
        "lvgl_thread", LVGL_SERVICE_PRIO, LVGL_THREAD_STACK_SIZE));
    m_pLvglThread->join();

    m_blacklight.write(ERDP_SET);
    // /* 1. 创建一个标签（Label）部件，父对象是当前活动屏幕 */
    // lv_obj_t* hello_label = lv_label_create(lv_scr_act());

    // /* 2. 设置标签的文本为 "Hello World" */
    // lv_label_set_text(hello_label, "Hello World");

    // /* 3. （可选）将标签在屏幕上居中显示 */
    // lv_obj_center(hello_label);
    while (true) {
        Thread::sleep(1000);
    }
}
