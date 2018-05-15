#pragma once
#include "noncopyable.h"

#include "MutexLock.h"
#include "FileUtil.h"
#include <iostream>
#include <memory>
#include <string>
#include "FixedBuffer.h"


using std::string;

class LogFile:noncopyable
{
public:
    LogFile(string& filename,const int FlushEveryN = 1024):
        file_(new AppendFile(filename)),
        FlushEveryN_(FlushEveryN),
        mutex_(new MutexLock),
        count_(0)
    {   }
    ~LogFile()
    {    }

    void flush()
    {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    }

    void append(const char* log,size_t len)
    {
        MutexLockGuard lock(*mutex_);
        file_->append(log,len);
        ++count_;
        if(count_ >= FlushEveryN_)
        {
            file_->flush();
        }
    }

private:
    const string filename_;
    const int FlushEveryN_;
    std::unique_ptr<AppendFile> file_;
    std::unique_ptr<MutexLock> mutex_;
    int count_;



};