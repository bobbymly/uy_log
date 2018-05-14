#pragma once
#include <pthread.h>
#include "MutexLock.h"
#include <time.h>
#include <errno.h>
class Condition
{
public:
    Condition(MutexLock& mutex):mutex_(mutex)
    {  
        pthread_cond_init(&cond_,NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond_);
    }
    void wait()
    {
        pthread_cond_wait(&cond_,mutex_.get());
    }
    void notify()
    {
        pthread_cond_signal(&cond_);
    }
    void notifyall()
    {
        pthread_cond_broadcast(&cond_);
    }
    bool waitForSeconds(int sec)
    {
        struct timespec nowtime;
        clock_gettime(CLOCK_REALTIME,&nowtime);
        nowtime.tv_sec += static_cast<time_t>(sec);
        return  ETIMEDOUT == pthread_cond_timedwait(&cond_,mutex_.get(),&nowtime);
    }




private:
    MutexLock& mutex_;
    pthread_cond_t cond_;


};
