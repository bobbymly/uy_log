#include "noncopyable.h"
#include <iostream>
#include <string>


class AppendFile:noncopyable
{
public:
    AppendFile(std::string filename);
    ~AppendFile();
    void append(const char* log,size_t len);
    void flush();

private:
    FILE* fp_;

    char buf[64*1024];

    size_t write(const char* log,size_t len);
};
