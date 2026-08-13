#include "Timestamp.h"
#include <chrono>
#include <cstdio>
#include <ctime>

Timestamp::Timestamp():microSecondsSinceEpoch_(0) {}

Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {}

Timestamp Timestamp::now()
{
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    return Timestamp(microseconds);
}

std::string Timestamp::toString() const
{
    char buf[128] = {0};
    const time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / 1000000);
    tm localTime;
    if (localtime_r(&seconds, &localTime) == nullptr)
    {
        snprintf(buf, 128, "invalid time");
        return buf;
    }
    snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d",
        localTime.tm_year + 1900,
        localTime.tm_mon + 1,
        localTime.tm_mday,
        localTime.tm_hour,
        localTime.tm_min,
        localTime.tm_sec);
    return buf;
}

// #include <iostream>
// int main()
// {
//     std::cout << Timestamp::now().toString() << std::endl;
//     return 0;
// }
