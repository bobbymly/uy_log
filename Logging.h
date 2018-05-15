#include "LogFile.h"
#include "AsyncLogging.h"
#include <pthread.h>
#include <string>
#include <sys/time.h>
#include "LogStream.h"
#include <stdio.h>
#include <string.h>
using std::string;
//class Logger;


static void output(const char* buf,int len);

class Logger:noncopyable
{
public:

    Logger(const char* basename,int line):basename_(basename),line_(line)
    {   addTimeInfo();}
    ~Logger()
    {    
        stream_<< " -- " << basename_ << ':' << line_ << '\n';
        output(stream_.buffer().begin(),stream_.buffer().lenth());
    }




    LogStream& stream()
    {
        return stream_;
    }

    static void setFileName(string& filename)
    {
        Logger::logFileName_ = filename;
    }

    static const string& getFileName()
    {
        return Logger::logFileName_;
    }







private:
    LogStream stream_;
    string basename_;
    int line_;
    static string logFileName_;

    void addTimeInfo()
    {
        struct timeval tv;
        time_t time;
        char str_t[26] = {0};
        gettimeofday (&tv, NULL);
        time = tv.tv_sec;
        struct tm* p_time = localtime(&time);   
        strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
        stream_ << str_t;
    }
};



static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging *AsyncLogger_;
std::string Logger::logFileName_ = "/uy_log.log";


static void once_init()
{
    AsyncLogger_ = new AsyncLogging(Logger::getFileName());
    AsyncLogger_->start();
}
static void output(const char* buf,int len)
{
    pthread_once(&once_control_,once_init);
    AsyncLogger_->append(buf,len);
    std::cout<<"test msg"<<buf;
}


#define LOG Logger(__FILE__, __LINE__).stream() //宏命令替换  方便写代码时直接以 LOG<< 的形式