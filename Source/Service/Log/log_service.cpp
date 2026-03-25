#include "log_service.hpp"

#include <stdarg.h>
#include <string.h>

#include <cstdint>

#include "erdp_hal_uart.hpp"
#include "thread_config.h"

namespace erdp {
    Thread* LoggerBase::m_pLogThread = nullptr;
    char LoggerBase::LOG_LEVEL_STR[5][2];
    char LoggerBase::COLOR_ANSI[5][6];
    LoggerBase::LogBlock LoggerBase::m_logBlock[LOG_BUFFER_COUNT];
    RingBuffer<uint8_t> LoggerBase::m_bufferOrder;
    LoggerBase::~LoggerBase() {
        if (m_pLogThread) {
            m_pLogThread->kill();
            delete m_pLogThread;
            m_pLogThread = nullptr;
        }
    }

    void LoggerBase::start() {
        if (!m_pLogThread) {
            // 初始化日志缓冲区
            for (int i = 0; i < LOG_BUFFER_COUNT; i++) {
                m_logBlock[i].pEvent = new erdp::Event();
                m_logBlock[i].pEvent->set(LOGBUFFER_EVENT_IDLE);
                memset(m_logBlock[i].logBuffer, 0, sizeof(m_logBlock[i].logBuffer));
                m_logBlock[i].bufferLen = 0;
            }
            m_bufferOrder.init(LOG_BUFFER_COUNT);
            // 初始化日志等级字符串
            strcpy(LOG_LEVEL_STR[DEBUG], "D");
            strcpy(LOG_LEVEL_STR[INFO], "I");
            strcpy(LOG_LEVEL_STR[WARN], "W");
            strcpy(LOG_LEVEL_STR[ERROR], "E");
            strcpy(LOG_LEVEL_STR[FATAL], "F");
            // 初始化颜色ANSI
            strcpy(COLOR_ANSI[DEBUG], "\033[90m");    // 白色
            strcpy(COLOR_ANSI[INFO], "\033[32m");     // 翠绿色
            strcpy(COLOR_ANSI[WARN], "\033[33m");     // 黄色
            strcpy(COLOR_ANSI[ERROR], "\033[31m");    // 红色
            strcpy(COLOR_ANSI[FATAL], "\033[31m");    // 红色
            m_pLogThread = new Thread(logThreadFunc, "LogThread", LOG_SERVICE_PRIO, LOG_THREAD_STACK_SIZE);
            m_pLogThread->join();
        }
    }

    int LoggerBase::findIdleBlock() {
        for (int i = 0; i < LOG_BUFFER_COUNT; i++) {
            if (m_logBlock[i].pEvent->get() & LOGBUFFER_EVENT_IDLE) {
                m_logBlock[i].pEvent->clear(LOGBUFFER_EVENT_IDLE);
                m_logBlock[i].pEvent->set(LOGBUFFER_EVENT_WRITE);
                m_logBlock[i].bufferLen = 0;
                return i;
            }
        }
        return -1;
    }
    bool LoggerBase::isBufferEnough(uint8_t blockIndex, uint8_t expectLen) {
        return m_logBlock[blockIndex].bufferLen + expectLen <= LOG_BUFFER_SIZE;
    }

    void LoggerBase::message(LogLevel level, const char* module, const char* fmt, ...) {
        char fmtChar = '\0';
        int blockIndex = -1;
        char messageBuffer[LOG_MAX_LEN];
        char timeString[15];
        char* pStrToWrite = nullptr;

#ifdef ERDP_ENABLE_LOGGER_COLOR
        char format[sizeof(ERDP_SET_LOGGER_FORMAT) + 12];
        strcpy(format, COLOR_ANSI[level]);
        strcat(format, ERDP_SET_LOGGER_FORMAT);
        strcat(format, "\033[0m");
#else
        char format[sizeof(ERDP_SET_LOGGER_FORMAT)];
        strcpy(format, ERDP_SET_LOGGER_FORMAT);
#endif
        uint8_t expectLen = 0;
        va_list args;
        va_start(args, fmt);
        vsnprintf(messageBuffer, sizeof(messageBuffer), fmt, args);
        va_end(args);
        for (uint32_t fmtPos = 0; fmtPos < sizeof(format); fmtPos++) {
            fmtChar = format[fmtPos];
            if (fmtChar == '%') {
                fmtPos++;
                switch (format[fmtPos]) {
                    case 'l':
                        expectLen = strlen(LOG_LEVEL_STR[level]);
                        pStrToWrite = LOG_LEVEL_STR[level];
                        break;
                    case 'm':
                        expectLen = strlen(module);
                        pStrToWrite = (char*)module;
                        break;
                    case 's':
                        expectLen = strlen(messageBuffer);
                        pStrToWrite = messageBuffer;
                        break;
                    case 't': {
                        uint32_t ticks = Thread::getSystem1msTicks();
                        uint32_t totalSeconds = ticks / 1000;
                        uint32_t hours = totalSeconds / 3600;
                        uint32_t minutes = (totalSeconds / 60) % 60;
                        uint32_t seconds = totalSeconds % 60;
                        uint32_t milliseconds = ticks % 1000;
                        snprintf(timeString, 15, "%02lu:%02lu:%02lu.%03lu", hours, minutes, seconds, milliseconds);
                        expectLen = 12;
                        pStrToWrite = timeString;
                        break;
                    }

                    default:

                        break;
                }
            } else {
                expectLen = 1;
                pStrToWrite = &fmtChar;
            }
            while (expectLen > 0) {
                if (blockIndex == -1) {
                    int res = findIdleBlock();
                    if (res == -1) {
                        printf("Log buffer is full, drop message.\n");
                        return;
                    }
                    blockIndex = res;
                }
                if (m_logBlock[blockIndex].bufferLen + expectLen <= LOG_BUFFER_SIZE) {
                    // 缓冲区有足够的空间，直接写入
                    memcpy(m_logBlock[blockIndex].logBuffer + m_logBlock[blockIndex].bufferLen, pStrToWrite, expectLen);
                    m_logBlock[blockIndex].bufferLen += expectLen;
                    expectLen = 0;
                } else {
                    // 缓冲区空间不足，写入部分数据
                    uint32_t writeLen = LOG_BUFFER_SIZE - m_logBlock[blockIndex].bufferLen;
                    memcpy(m_logBlock[blockIndex].logBuffer + m_logBlock[blockIndex].bufferLen, pStrToWrite, writeLen);
                    m_logBlock[blockIndex].bufferLen += writeLen;
                    pStrToWrite += writeLen;
                    expectLen -= writeLen;
                    if (!m_bufferOrder.push(blockIndex)) {
                        printf("Log buffer order is full, drop message.\n");
                        return;
                    }
                    blockIndex = -1;
                }
            }
        }
        if (blockIndex != -1) {
            if (!m_bufferOrder.push(blockIndex)) {
                printf("Log buffer order is full, drop message.\n");
                return;
            }
        }
    }

    void LoggerBase::logThreadFunc(void* arg) {
        uint8_t index;
        uint8_t charIndex = 0;
        while (1) {
            if (m_bufferOrder.pop(index, OS_WAIT_FOREVER)) {
                charIndex = 0;
                while (m_logBlock[index].bufferLen > 0) {
                    UartDev::putChar(m_logBlock[index].logBuffer[charIndex]);
                    charIndex++;
                    m_logBlock[index].bufferLen--;
                }
                m_logBlock[index].pEvent->clear(LOGBUFFER_EVENT_WRITE);
                m_logBlock[index].pEvent->set(LOGBUFFER_EVENT_IDLE);
            }
        }
    }

}    // namespace erdp
