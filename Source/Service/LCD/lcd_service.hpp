#ifndef LCD_SERVICE_HPP
#define LCD_SERVICE_HPP

#include <memory>

#include "erdp_hal_gpio.hpp"
#include "erdp_hal_spi.hpp"
#include "erdp_if_spi.h"
#include "erdp_osal.hpp"
#include "lvgl.h"

using namespace erdp;
class LCDService {
   public:
    LCDService();
    ~LCDService();

    void start();

   private:
    std::unique_ptr<Thread> m_pThread;
    std::unique_ptr<Thread> m_pLvglThread;
    GpioDev m_blacklight;
    GpioDev m_rst;
    lv_display_t * m_disp;
    static GpioDev m_dc;
    static OS_Hook m_tickHook;
    static SpiDev<ERDP_SPI_MODE_MASTER>* m_spi;

    void lcdThreadFunc();

    static void tickHook(void* param);
    static void lvglSendCmd(lv_display_t* disp, const uint8_t* cmd, size_t cmd_size, const uint8_t* param,
                            size_t param_size);
    static void lvglSendColor(lv_display_t* disp, const uint8_t* cmd, size_t cmd_size, uint8_t* param,
                              size_t param_size);
};

#endif    // LCD_SERVICE_HPP