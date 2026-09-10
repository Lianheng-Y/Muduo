#pragma once
#include "Timestamp.h"
#include "noncopyable.h"
#include <functional>
#include <map>
#include <memory>
class Channel; class EventLoop;
class TimerQueue: noncopyable { public: using TimerCallback=std::function<void()>; using TimerId=uint64_t; explicit TimerQueue(EventLoop*); ~TimerQueue(); TimerId addTimer(TimerCallback,int64_t,int64_t=0); void cancel(TimerId); private: struct T{TimerId id;int64_t when,interval;TimerCallback cb;}; void read(Timestamp); void reset(); EventLoop*l_;int fd_;std::unique_ptr<Channel> ch_;TimerId next_;std::multimap<int64_t,std::shared_ptr<T>> q_;};
