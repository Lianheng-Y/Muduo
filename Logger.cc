#include "Logger.h"
#include "Timestamp.h"

#include <iostream>

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

// 获取日志唯一的实例对象
Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::setLogLevel(int level)
{
    logLevel_ = level;
}

void Logger::setOutput(OutputFunc out)
{
    g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
    g_flush = flush;
}

// 写日志  [级别信息] time : msg
void Logger::log(std::string msg)
{
    std::string buf;
    switch (logLevel_)
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

    g_output(buf.c_str(), buf.size());
    if (logLevel_ == FATAL)
    {
        g_flush();
    }
}