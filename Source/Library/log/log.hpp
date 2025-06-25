#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

class LogInterface
{
public:
    LogInterface() = default;
    ~LogInterface() = default;
    virtual void log_output(const uint8_t *message, uint32_t len) = 0;
};

template <typename Interface, size_t MSG_MAX_SIZE = 256>
class Log
{
public:
    Log() = default;
    ~Log() = default;

    // 设置日志输出格式的函数
    void set_pattern(const std::string &pattern)
    {
        log_pattern = pattern;
    }

    void trace(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        log_with_level("TRACE", module, format, args);
        va_end(args);
    }
    void debug(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        log_with_level("DEBUG", module, format, args);
        va_end(args);
    }
    void info(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        log_with_level("INFO", module, format, args);
        va_end(args);
    }
    void warn(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        log_with_level("WARN", module, format, args);
        va_end(args);
    }
    void error(const char *module, const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        log_with_level("ERROR", module, format, args);
        va_end(args);
    }

private:
    std::string log_pattern; // 存储日志输出格式
    char message[MSG_MAX_SIZE];
    char full_message[MSG_MAX_SIZE];

    void log_with_level(const char *level, const char *module, const char *format, va_list args)
    {
        memset(full_message, 0, sizeof(full_message));
        int len = vsnprintf(message, sizeof(message), format, args);
        if (len <= 0)
            return;

        const char *pos = log_pattern.c_str();
        while (*pos)
        {
            if (*pos == '\033') // 检测ANSI转义序列开始
            {
                // 处理ANSI转义序列
                const char *ansi_start = pos;
                while (*pos && (*pos != 'm' && *pos != 'H' && *pos != 'J' && *pos != 'K'))
                {
                    ++pos;
                }
                if (*pos)
                {
                    ++pos; // 包含结束字符
                    size_t ansi_len = pos - ansi_start;
                    if (strlen(full_message) + ansi_len >= MSG_MAX_SIZE - 1)
                    {
                        break;
                    }
                    strncat(full_message, ansi_start, ansi_len);
                    continue;
                }
            }
            else if (*pos == '%')
            {
                ++pos;
                if (*pos == 'L')
                {
                    if (strlen(full_message) + strlen(level) >= MSG_MAX_SIZE - 1)
                    {
                        break;
                    }
                    strcat(full_message, level);
                }
                else if (*pos == 'M')
                {
                    if (strlen(full_message) + strlen(module) >= MSG_MAX_SIZE - 1)
                    {
                        break;
                    }
                    strcat(full_message, module);
                }
                else if (*pos == 'm')
                {
                    if (strlen(full_message) + strlen(message) >= MSG_MAX_SIZE - 1)
                    {
                        break;
                    }
                    strcat(full_message, message);
                }
                else
                {
                    if (strlen(full_message) + 2 >= MSG_MAX_SIZE - 1)
                    {
                        break;
                    }
                    strcat(full_message, "%");
                    strncat(full_message, &pos[-1], 1);
                }
            }
            else
            {
                if (strlen(full_message) + 1 >= MSG_MAX_SIZE - 1)
                {
                    break;
                }
                strncat(full_message, pos, 1);
            }
            ++pos;
        }
        logger.log_output(reinterpret_cast<const uint8_t *>(full_message), strlen(full_message));
    }
    // 获取日志接口实例并输出日志
    Interface logger;
};

#endif
