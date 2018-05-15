#pragma once
#include  <functional>
#include <string>
#include "CountDownLatch.h"
#include <iostream>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <unistd.h>
using std::cout;
using std::endl;
using std::string;
namespace CurrentThread
{
    //使用__thread 关键字用以记录线程信息
	__thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "default";
	void cacheTid();
    inline int tid()
	{
		if(t_cachedTid == 0)cacheTid();

		return t_cachedTid;
	}
	inline const char* tidString()
	{
		return t_tidString;
	}

	inline int tidStringLength()
	{
		return t_tidStringLength;
	}

	inline const char* name()
	{
		return t_threadName;
	}
}


class Thread
{
public:
	Thread(std::function<void()>func,std::string name = std::string()):
        name_(name),
        started_(false),
        joined_(false),
        func_(func),
        count_(1),
        pthreadId_(0),
        tid_(0)    
    {   name_ = "Thread";   }
    ~Thread()
    {
        if(started_ && !joined_)
        {
            pthread_detach(pthreadId_);//若未 join 则应 detach，防止产生僵尸线程
        }
    }
    void start();
    int join()
    {
        if(!started_)
        {
            cout<<"the pthread not start"<<endl;
            return 0;
        }
        joined_ = true;
        return pthread_join(pthreadId_,NULL);
    }
    pid_t get_tid(){    return tid_;}
    bool started(){ return started_;}
    std::string name(){ return name_;}

private:
    pid_t tid_;
    pthread_t pthreadId_;
    std::string name_;
    CountDownLatch count_;
    bool started_;
    bool joined_;
    std::function<void()> func_;
};






pid_t gettid()
{
	return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid()
{
	if(t_cachedTid == 0)
	{
		t_cachedTid = gettid();
		t_tidStringLength = snprintf(t_tidString,sizeof(t_tidString),"%5d",t_cachedTid);
	}
}

struct ThreadData
{
	typedef std::function<void()> ThreadFunc;
	ThreadFunc func_;
	string name_;
	pid_t* tid_;
	CountDownLatch* latch_;

	ThreadData(const ThreadFunc& func,const string& name,pid_t* tid,CountDownLatch* latch):
		func_(func),
		name_(name),
		tid_(tid),
		latch_(latch)
	{ }

	void runInThread()
	{
		*tid_=CurrentThread::tid();
		tid_=NULL;
		latch_->countdown();
		latch_=NULL;

		CurrentThread::t_threadName=name_.empty()?"Thread":name_.c_str();
		prctl(PR_SET_NAME,CurrentThread::t_threadName);
		func_();
		CurrentThread::t_threadName="finished";
	}


};

void *startThread(void *obj)
{
	ThreadData* data=static_cast<ThreadData*>(obj);
	data->runInThread();
	delete data;
		return NULL;
}



void Thread::start()
{
	started_=true;
	ThreadData* data=new ThreadData(func_,name_,&tid_,&count_);
	if(pthread_create(&pthreadId_,NULL,&startThread,data))
	{
		started_=false;
		delete data;
	}else{
		count_.wait();	//通过条件变量确定线程成功创建
	}
}