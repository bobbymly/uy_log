#pragma once
#include <string>
#include <memory>
#include <string.h>
#include "noncopyable.h"

template <int SIZE>
class FixedBuffer: noncopyable
{
public:
    FixedBuffer():cur_(buf_){   }
    ~FixedBuffer() = default;
    char* current(){  return cur_;}
    void add(int n){    cur_ += n;}
    char* begin(){  return buf_;}
    void reset(){   cur_ = buf_;}
    int avail(){   return static_cast<int>(end() - cur_);}
    int lenth(){   return static_cast<int>(cur_ - buf_);}
    void bzero(){   memset(buf_,0,sizeof(buf_));}
    void append(const char* target_buf,int len)
    {
        if(avail() >= len)
        {
            memcpy(buf_,target_buf,len);
            cur_ += len;
        }
    }

private:
    char buf_[SIZE];
    char* cur_;
    const char* end(){  return buf_ + sizeof buf_;}

};
