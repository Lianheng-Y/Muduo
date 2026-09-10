#include "TimerQueue.h"
#include "EventLoop.h"
#include "Channel.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <time.h>
#include <algorithm>
static int64_t nowms(){timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec*1000LL+t.tv_nsec/1000000;}
TimerQueue::TimerQueue(EventLoop*l):l_(l),fd_(timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC)),ch_(new Channel(l,fd_)),next_(1){ch_->setReadCallback(std::bind(&TimerQueue::read,this,std::placeholders::_1));ch_->enableReading();}
TimerQueue::~TimerQueue(){ch_->disableAll();ch_->remove();close(fd_);}
TimerQueue::TimerId TimerQueue::addTimer(TimerCallback cb,int64_t d,int64_t i){auto f=[&](){auto t=std::make_shared<T>();t->id=next_++;t->when=nowms()+std::max<int64_t>(1,d);t->interval=i;t->cb=cb;q_.emplace(t->when,t);reset();return t->id;};if(l_->isInLoopThread())return f();l_->runInLoop(f);return 0;}
void TimerQueue::cancel(TimerId id){l_->runInLoop([this,id]{for(auto i=q_.begin();i!=q_.end();++i)if(i->second->id==id){q_.erase(i);break;}reset();});}
void TimerQueue::read(Timestamp){uint64_t x;::read(fd_,&x,8);auto n=nowms();while(!q_.empty()&&q_.begin()->first<=n){auto t=q_.begin()->second;q_.erase(q_.begin());t->cb();if(t->interval){t->when=n+t->interval;q_.emplace(t->when,t);}}reset();}
void TimerQueue::reset(){itimerspec s{};if(!q_.empty()){auto d=std::max<int64_t>(1,q_.begin()->first-nowms());s.it_value.tv_sec=d/1000;s.it_value.tv_nsec=d%1000*1000000;}timerfd_settime(fd_,0,&s,nullptr);}
