#include  <functional>
#include <string>
#include "CountDownLatch.h"

class Thread
{
    Thread(std::function<void()>func,std::string name = std::string()):
        name_(name),
        started_(false),
        joined_(false),
        func_(func)    
    {   }
    ~Thread()
    {   }
    void start();
    int join();
    pid_t get_tid(){    return tid_;}
    bool started(){ return started_;}
    std::string name(){ return name_;}



private:
    pid_t tid_;
    pthread_t pthreadid_;
    std::string name_;
    CountDownLatch count_;
    bool started_;
    bool joined_;
    std::function<void()> func_;
}