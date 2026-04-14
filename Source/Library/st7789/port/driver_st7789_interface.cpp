/**
 *Copyright (c) 2015 - present LibDriver All rights reserved
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      driver_st7789_interface_template.c
 * @brief     driver st7789 interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2023-04-15
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2023/04/15  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_st7789_interface.h"

#include <stdarg.h>

#include "board.h"
#include "erdp_hal_gpio.hpp"
#include "erdp_hal_spi.hpp"
#include "log_service.hpp"
using namespace erdp;
static SpiDev<ERDP_SPI_MODE_MASTER>* spiCOM = nullptr;
static GpioDev* dc = nullptr;
GpioDev* rst = nullptr;
extern "C" {

    /**
     * @brief  interface spi bus init
     * @return status code
     *         - 0 success
     *         - 1 spi init failed
     * @note   none
     */
    uint8_t st7789_interface_spi_init(void) {
        spiCOM = new SpiDev<ERDP_SPI_MODE_MASTER>(SPI_MASTER_INFO, SPI_MASTER_CONFIG, 256);
        if (spiCOM == nullptr) {
            return 1;
        }
        spiCOM->csHigh();
        return 0;
    }

    /**
     * @brief  interface spi bus deinit
     * @return status code
     *         - 0 success
     *         - 1 spi deinit failed
     * @note   none
     */
    uint8_t st7789_interface_spi_deinit(void) { return 1; }

    /**
     * @brief     interface spi bus write
     * @param[in] *buf pointer to a data buffer
     * @param[in] len length of data buffer
     * @return    status code
     *            - 0 success
     *            - 1 write failed
     * @note      none
     */
    uint8_t st7789_interface_spi_write_cmd(uint8_t* buf, uint16_t len) {
        if (spiCOM == nullptr) {
            return 1;
        }
        spiCOM->csLow();
        spiCOM->send(buf, len);
        spiCOM->csHigh();
        return 0;
    }

    /**
     * @brief     interface delay ms
     * @param[in] ms time
     * @note      none
     */
    void st7789_interface_delay_ms(uint32_t ms) { Thread::sleep(ms); }

    /**
     * @brief     interface print format data
     * @param[in] fmt format data
     * @note      none
     */
    void st7789_interface_debug_print(const char* const fmt, ...) {
        char buf[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        Debug("st7789", buf);
    }

    /**
     * @brief  interface command && data gpio init
     * @return status code
     *         - 0 success
     *         - 1 gpio init failed
     * @note   none
     */
    uint8_t st7789_interface_cmd_data_gpio_init(void) {
        dc = new GpioDev(LCD_DC_PORT, LCD_DC_PIN, ERDP_GPIO_PIN_MODE_OUTPUT);
        if (dc == nullptr) {
            return 1;
        }
        return 0;
    }

    /**
     * @brief  interface command && data gpio deinit
     * @return status code
     *         - 0 success
     *         - 1 gpio deinit failed
     * @note   none
     */
    uint8_t st7789_interface_cmd_data_gpio_deinit(void) { return 1; }

    /**
     * @brief     interface command && data gpio write
     * @param[in] value written value
     * @return    status code
     *            - 0 success
     *            - 1 gpio write failed
     * @note      none
     */
    uint8_t st7789_interface_cmd_data_gpio_write(uint8_t value) { 
        if (dc == nullptr) {
            return 1;
        }
        dc->write((ERDP_Status_t)value);
        return 0; }

    /**
     * @brief  interface reset gpio init
     * @return status code
     *         - 0 success
     *         - 1 gpio init failed
     * @note   none
     */
    uint8_t st7789_interface_reset_gpio_init(void) {
        rst = new GpioDev(LCD_RST_PORT, LCD_RST_PIN, ERDP_GPIO_PIN_MODE_OUTPUT);
        if (rst == nullptr) {
            return 1;
        }
        return 0;
    }

    /**
     * @brief  interface reset gpio deinit
     * @return status code
     *         - 0 success
     *         - 1 gpio deinit failed
     * @note   none
     */
    uint8_t st7789_interface_reset_gpio_deinit(void) { return 0; }

    /**
     * @brief     interface reset gpio write
     * @param[in] value written value
     * @return    status code
     *            - 0 success
     *            - 1 gpio write failed
     * @note      none
     */
    uint8_t st7789_interface_reset_gpio_write(uint8_t value) { 
        if (rst == nullptr) {
            return 1;
        }
        rst->write((ERDP_Status_t)value);
        return 0; }
}