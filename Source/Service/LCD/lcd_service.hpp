#ifndef LCD_SERVICE_HPP
#define LCD_SERVICE_HPP

#include <memory>
#include "driver_st7789_interface.h"
#include "erdp_osal.hpp"

using namespace erdp;
class LCDService {
public:
    LCDService();
    ~LCDService();

    void start();
    private:
    st7789_handle_t m_lcdHandle;
    Thread* m_pThread;
    void lcdThreadFunc();
    uint8_t st7789BasicInit(void);
};

#endif    // LCD_SERVICE_HPP