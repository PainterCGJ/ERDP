#ifndef __LOG_ADAPTER_HPP__
#define __LOG_ADAPTER_HPP__
#include "log.hpp"
#include "erdp_hal_uart.hpp"
#include "erdp_osal.hpp"

class Logger : public LogInterface
{
public:
    Logger() : log_thread([this]()
                          { this->log_thread_code(); }, "LoggerThread", 1, 512)
    {
        set_pattern("[%L][%M]: %m\n");
        set_tag(LogLevel::ERROR, "\033[31mERROR\033[0m");
        set_tag(LogLevel::WARN, "\033[33mWARN\033[0m");
        set_tag(LogLevel::DEBUG, "\033[37mDEBUG\033[0m");
        set_tag(LogLevel::INFO, "\033[94mINFO\033[0m");
        set_tag(LogLevel::TRACE, "\033[90mTRACE\033[0m");

        logger.set_level(LogLevel::INFO);
    };
    ~Logger() = default;
    static void t(const char *module, const char *format, ...);
    static void i(const char *module, const char *format, ...);
    static void d(const char *module, const char *format, ...);
    static void w(const char *module, const char *format, ...);
    static void e(const char *module, const char *format, ...);
    static void set_pattern(const std::string &pattern)
    {
        logger.set_pattern(pattern);
    }
    void log_output(const uint8_t *message, uint32_t len) override;
    void start()
    {
        log_thread.join();
    }

    static void set_tag(LogLevel level, const std::string &tag)
    {
        logger.set_tag(level, tag);
    }

private:
    static erdp::Queue<uint8_t> log_queue;
    static Log<Logger> logger;
    void log_thread_code();
    erdp::Thread log_thread;
};

#endif
