#pragma once

#include <cstdint>
#include <string>

// 时间类
class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch);
    static Timestamp now();
    std::string toString() const;
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
private:
    int64_t microSecondsSinceEpoch_;
};
