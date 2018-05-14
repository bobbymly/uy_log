#pragma once
#include "LogFile.h"
#include "noncopyable.h"
#include <memory>
#include <cstring>
#include "FixedBuffer.h"
const int kLargeBuffer = 4000*1000;
const int kSmallBuffer = 4000;


class LogStream: noncopyable
{
public:
    LogStream();
    ~LogStream();
    FixedBuffer<kSmallBuffer>& buffer(){  return buf_;}
    void apppend(const char* target_buf,int len){   buf_.append(target_buf,len);}

private:
    FixedBuffer<kSmallBuffer> buf_;

    template<class T>
    void transform(T);


public:
    
    LogStream& operator<<(bool value){ buf_.append(value == 1 ? "1" : "0",1);return *this;}
    LogStream& operator<<(short value){ transform(value);return *this;}
    LogStream& operator<<(int value){ transform(value);return *this;}
    LogStream& operator<<(long value){ transform(value);return *this;}
    LogStream& operator<<(long long value){ transform(value);return *this;}
    LogStream& operator<<(float value)
    { 
        if(buf_.avail() < 32)return *this;
        int len = sprintf(buf_.current(),"%f",value);    
        buf_.add(len);
        return *this;
    }
    LogStream& operator<<(double value)
    { 
        if(buf_.avail() < 32)return *this;
        int len = sprintf(buf_.current(),"%lf",value);    
        buf_.add(len);
        return *this;
    }
    LogStream& operator<<(const char* target_buf)
    {    
        if(target_buf)
        {
            buf_.append(target_buf,strlen(target_buf));
        }else{
            buf_.append("(NULL)",7);
        }
        return *this;
    }
    LogStream& operator<<(string& target_buf)
    {    
        buf_.append(target_buf.c_str(),target_buf.size());
        return *this;
    }

};