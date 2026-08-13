#include "Logger.h"
#include "Timestamp.h"

#include <iostream>
#include <mutex>

void defaultOutput(const char* msg, int len)
{
    fwrite(msg, 1, len, stdout);
}

void defaultFlush()
{
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;
std::mutex g_outputMutex;
thread_local LogLevel t_logLevel = INFO;

// 获取日志唯一的实例对象
Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::setLogLevel(int level)
{
    t_logLevel = static_cast<LogLevel>(level);
}

void Logger::setOutput(OutputFunc out)
{
    std::lock_guard<std::mutex> lock(g_outputMutex);
    g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
    std::lock_guard<std::mutex> lock(g_outputMutex);
    g_flush = flush;
}

// 写日志  [级别信息] time : msg
void Logger::log(std::string msg)
{
    log(t_logLevel, std::move(msg));
}

void Logger::log(LogLevel level, std::string msg)
{
    std::string buf;
    switch (level)
    {
    case INFO:
        buf += "[INFO]";
        break;
    case ERROR:
        buf += "[ERROR]";
        break;
    case FATAL:
        buf += "[FATAL]";
        break;
    case DEBUG:
        buf += "[DEBUG]";
        break;
    default:
        break;
    }

    // 打印时间和msg
    buf += Timestamp::now().toString();
    buf += " : ";
    buf += msg;
    buf += "\n";

    OutputFunc output;
    FlushFunc flush;
    {
        std::lock_guard<std::mutex> lock(g_outputMutex);
        output = g_output;
        flush = g_flush;
    }
    output(buf.c_str(), static_cast<int>(buf.size()));
    if (level == FATAL)
    {
        flush();
    }
}
