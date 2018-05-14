#pragma once
#include "Thread.h"
//#include "noncopyable.h"
//#include "MutexLock.h"
#include "CountDownLatch.h"
//#include <pthread.h>
//#include <string>
#include <functional>
#include <vector>
#include "FixedBuffer.h"
#include "LogFile.h"
#include "LogStream.h"
using std::string;
using std::vector;
using std::shared_ptr;



class AsyncLogging:public noncopyable
{



public:
    AsyncLogging(string basename,int flushInterval = 2):
        basename_(basename),
        flushInterval_(flushInterval),
        mutex_(),
        cond_(mutex_),
        running_(),
        count_(1),
        currentBuffer_(new Buffer),
        nextBuffer_(new Buffer),
        thread_(std::bind(&AsyncLogging::func_,this),"Logging")
    {   
        buffers_.reserve(16);
    }
    
    ~AsyncLogging()
    {    
        if(running_)stop();
    }

    void start()
    {
        running_ = true;
        thread_.start();
        count_.wait();
    }
    void stop()
    {
        running_ = false;
        cond_.notifyall();
        thread_.join();
    }

    void append(const char* logline,int len)
    {
        MutexLockGuard lock(mutex_);
        if(currentBuffer_->avail() > len)
        {
            currentBuffer_->append(logline,len);
        }else{
            buffers_.push_back(currentBuffer_);
	    	currentBuffer_.reset();
            if(nextBuffer_)
                currentBuffer_=std::move(nextBuffer_);
            else 
                currentBuffer_.reset(new Buffer);
            currentBuffer_->append(logline,len);
            cond_.notify();
        
        }
    }





private:
    
    MutexLock mutex_;
    string basename_;
    CountDownLatch count_;
    Thread  thread_;
    bool running_;
    Condition cond_;

    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef vector<shared_ptr<Buffer > >BufferVector;
    BufferVector buffers_;
    shared_ptr<Buffer > currentBuffer_;
	shared_ptr<Buffer > nextBuffer_;
    const int flushInterval_;

    void func_();


};

void AsyncLogging::func_()
{
    count_.countdown();
    LogFile output(basename_);
    shared_ptr<Buffer > newBuffer1(new Buffer);
    shared_ptr<Buffer > newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_)
    {
        {
            MutexLockGuard lock(mutex_);
            if(buffers_.empty())
                cond_.waitForSeconds(flushInterval_);
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();


            currentBuffer_=std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)
                nextBuffer_=std::move(newBuffer2);
        }

        if(buffersToWrite.size()>25)
            buffersToWrite.erase(buffersToWrite.begin()+2,buffersToWrite.end());
        for(size_t i=0;i<buffersToWrite.size();++i)
            output.append(buffersToWrite[i]->begin(),buffersToWrite[i]->lenth());
        if(buffersToWrite.size()>2)
            buffersToWrite.resize(2);
        if(!newBuffer1)
        {
            newBuffer1=buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if(!newBuffer2)
        {
            newBuffer2=buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}