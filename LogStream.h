#include "LogFile.h"
//#include "noncopyable.h"
#include <memory>
#include <cstring>
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



class LogStream: noncopyable
{
public:
    LogStream();
    ~LogStream();
    void apppend(const char* target_buf,int len){   buf_.append(target_buf,len);}

private:
    FixedBuffer<4000> buf_;

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
    LogStream& operator<<(char* target_buf)
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
    {    buf_.append(target_buf.c_str(),target_buf.size());
        return *this;
    }
};