#include "log_adapter.hpp"
#include "erdp_hal_uart.hpp"

erdp::Queue<uint8_t> Logger::log_queue(1024);
Log<Logger> Logger::logger;

void Logger::i(const char *module, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger.vinfo(module, format, args);
    va_end(args);

}
void Logger::d(const char *module, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger.vdebug(module, format, args);
    va_end(args);
}
void Logger::e(const char *module, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger.verror(module, format, args);
    va_end(args);
}
void Logger::w(const char *module, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger.vwarn(module, format, args);
    va_end(args);
}
void Logger::t(const char *module, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger.vtrace(module, format, args);
    va_end(args);
}
void Logger::log_output(const uint8_t *message, uint32_t len)
{
    // 将日志信息输出到控制台
    if (message != nullptr && len > 0)
    {
        while (len--)
        {
            log_queue.push(*message++);
        }
    }
}
void Logger::log_thread_code()
{
    uint8_t data;
    const erdp::UartDev *const &uart_dev = erdp::UartDev::get_debug_com();
    while (!uart_dev)
        ;
    while (true)
    {
        while (log_queue.pop(data))
        {
            uart_dev->send(&data, 1);
        }
        erdp::Thread::delay_ms(10); // 10ms
    }
}