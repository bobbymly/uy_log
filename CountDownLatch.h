#include "Condition.h"
//#include  "MutexLock.h"





class CountDownLatch
{
public:
    CountDownLatch(int count):
        mutex_(),
        cond_(mutex_),
        count_(count)
    {   }

    ~CountDownLatch() = default;
    void wait()
    {
        MutexLockGuard lock(mutex_);
        while(count_ > 0)cond_.wait();
    }
    void countdown()
    {
        MutexLockGuard lock(mutex_);
        --count_;
        if(count_ == 0)
        {
            cond_.notifyall();
        }
    }




private:
    MutexLock mutex_;
    Condition cond_;
    int count_;

};