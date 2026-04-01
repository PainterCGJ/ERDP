#ifndef LOG_SERVICE_HPP
#define LOG_SERVICE_HPP

#include "erdp_config.h"
#include "erdp_osal.hpp"

#define LOG_MAX_LEN      (128)
#define LOG_BUFFER_SIZE  (64)
#define LOG_BUFFER_COUNT (16)

#define LOGBUFFER_EVENT_IDLE  (uint32_t(1) << 0)
#define LOGBUFFER_EVENT_WRITE (uint32_t(1) << 1)
#define LOGBUFFER_EVENT_READY (uint32_t(1) << 2)
// 使用空参数时调用无参版本
#define Debug(module, fmt, ...) LoggerBase::message(LoggerBase::DEBUG, module, fmt, ##__VA_ARGS__)
#define Info(module, fmt, ...) LoggerBase::message(LoggerBase::INFO, module, fmt, ##__VA_ARGS__)
#define Warn(module, fmt, ...) LoggerBase::message(LoggerBase::WARN, module, fmt, ##__VA_ARGS__)
#define Error(module, fmt, ...) LoggerBase::message(LoggerBase::ERROR, module, fmt, ##__VA_ARGS__)
#define Fatal(module, fmt, ...) LoggerBase::message(LoggerBase::FATAL, module, fmt, ##__VA_ARGS__)

namespace erdp {
    class LoggerBase {
       public:
        LoggerBase() = default;
        ~LoggerBase();
        enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL };

        static void start();
        static void debug(const char* module, const char* fmt, ...);
        static void message(LogLevel level, const char* module, const char* fmt, ...);

       private:
        typedef struct sLogBlock {
            char logBuffer[LOG_BUFFER_SIZE];
            erdp::Event* pEvent;
            uint32_t bufferLen;
        } LogBlock;
        static LogBlock m_logBlock[LOG_BUFFER_COUNT];
        static RingBuffer<uint8_t> m_bufferOrder;
        static char LOG_LEVEL_STR[5][2];
        static char COLOR_ANSI[5][6];
        constexpr static char FORMAT[] = ERDP_SET_LOGGER_FORMAT;
        constexpr static uint32_t FORMAT_SIZE = sizeof(FORMAT) + 12;
        static Mutex* m_pMutex;
        static Thread* m_pLogThread;
        static void logThreadFunc(void* arg);
        static int findIdleBlock();
        static bool isBufferEnough(uint8_t blockIndex, uint8_t expectLen);
    };
    class Logger {
       public:
        Logger();
        ~Logger();

        static Logger& getInstance();

       private:
    };
}    // namespace erdp
#endif
