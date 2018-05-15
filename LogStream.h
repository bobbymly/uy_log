#pragma once
#include "LogFile.h"
#include "noncopyable.h"
#include <memory>
#include <cstring>
#include <algorithm>
#include <iostream>
#include "FixedBuffer.h"
#include <string>
using std::string;
const int kLargeBuffer = 4000*1000;
const int kSmallBuffer = 4000;







class LogStream: noncopyable
{
public:

    FixedBuffer<kSmallBuffer>& buffer(){  return buf_;}
    void apppend(const char* target_buf,int len){   buf_.append(target_buf,len);}

private:
    FixedBuffer<kSmallBuffer> buf_;

    template<class T>
    void transform(T);


public:
    
    LogStream& operator<<(bool value){ buf_.append(value == 1 ? "1" : "0",1);return *this;}
    LogStream& operator<<(short value){ transform(value);return *this;}
    LogStream& operator<<(unsigned short value){ transform(value);return *this;}
    LogStream& operator<<(int value){ transform(value);return *this;}
    LogStream& operator<<(unsigned int value){ transform(value);return *this;}
    LogStream& operator<<(long value){ transform(value);return *this;}
    LogStream& operator<<(unsigned long value){ transform(value);return *this;}
    LogStream& operator<<(long long value){ transform(value);return *this;}
    LogStream& operator<<(unsigned long long value){ transform(value);return *this;}
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
    LogStream& operator<<(char target_buf)
    {
        if(&target_buf)
        {
            buf_.append(&target_buf,1);
        }else{
            buf_.append("(NULL)",6);
        }
        return *this;
    }
    LogStream& operator<<(const string& target_buf)
    {    
        buf_.append(target_buf.c_str(),target_buf.size());
        return *this;
    }


};



using namespace std;
char intToChar[]="9876543210123456789";

template <class T>
void LogStream::transform(T value)
{
    if(buf_.avail()<32)return;
    T temp = value;
    char *cur = buf_.current();
    while(temp)
    {
        int key = temp % 10;
        *cur = intToChar[9 + key];
        ++cur;
        temp /= 10;
    }
    if(value < 0)
    {
        *cur='-';
        ++cur;
    }
    *cur = '\0';
    std::reverse(buf_.current(),cur);
    buf_.add(cur - buf_.current());
}