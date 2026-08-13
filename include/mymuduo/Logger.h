#pragma once

#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>

#include "noncopyable.h"

// LOG_INFO("%s %d", arg1, arg2)
#define LOG_INFO(...) \
    do \
    { \
        Logger::instance().logf(INFO, __VA_ARGS__); \
    } while(0)

#define LOG_ERROR(...) \
    do \
    { \
        Logger::instance().logf(ERROR, __VA_ARGS__); \
    } while(0)

#define LOG_FATAL(...) \
    do \
    { \
        Logger::instance().logf(FATAL, __VA_ARGS__); \
        exit(-1); \
    } while(0)

#ifdef MUDEBUG
#define LOG_DEBUG(...) \
    do \
    { \
        Logger::instance().logf(DEBUG, __VA_ARGS__); \
    } while(0)
#else
    #define LOG_DEBUG(...)
#endif

// 定义日志的级别  INFO  ERROR  FATAL  DEBUG
enum LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
    FATAL, // core信息
    DEBUG, // 调试信息
};

// 输出一个日志类
class Logger : noncopyable
{
public:
    using OutputFunc = std::function<void(const char* msg, int len)>;
    using FlushFunc = std::function<void()>;

    // 获取日志唯一的实例对象
    static Logger& instance();
    // 设置日志级别
    void setLogLevel(int level);
    // 写日志
    void log(std::string msg);
    void log(LogLevel level, std::string msg);

    void logf(LogLevel level, const char* message)
    {
        log(level, message);
    }

    template<typename Arg, typename... Args>
    void logf(LogLevel level, const char* format, Arg arg, Args... args)
    {
        char buffer[1024] = {0};
        std::snprintf(buffer, sizeof buffer, format, arg, args...);
        log(level, buffer);
    }

    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

};
