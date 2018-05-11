#include "LogStream.h"
#include <cstring>
#include <algorithm>
#include <iostream>

using namespace std;
char intToChar[]="9876543210123456789";

template <class T>
void LogStream::transform(T value)
{
    if(buf_.avali()<32)return;
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
    *cur = 0;
    std::reverse(buf_.begin(),cur);
    buf_.add(static_cast<int>(cur - buf_.begin()));
}


// template <class T>
// void transform(char* buf,T value)
// {
//     if(buf_.avali() > 32)
//     {
//         int len = sprintf(buf,"")
//     }
// }








// int main()
// {
//     char dig[10]="";
//     transform(dig,-123);
//     cout<<dig<<endl;


//     return 0;
// }