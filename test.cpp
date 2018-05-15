#include <iostream>
#include <string>
#include "log/Logging.h"
#include <unistd.h>
#include <vector>
#include "log/Thread.h"
using namespace std;
void type_test();
void threadFunc();
void stressing_single_thread();
void stressing_multi_threads(int threadNum = 4);

int main()
{   
    cout<<"------------the default log path=\"/uy_log.log\"----------"<<endl;
    cout<<"------------maybe you need the Administrator permission to run it------------"<<endl;
    cout<<"------------begin test------------"<<endl;
    type_test();
    stressing_single_thread();
    stressing_multi_threads();
    cout<<"------------sleep for 5 seconds------------"<<endl;
    sleep(5);
    cout << "------------test finished------------" << endl;
    return 0;
}

void type_test()
{
    cout << "------------type test------------" << endl;
    LOG <<"hello world";
    LOG << 0;
    LOG << 1234567890123;
    LOG << 1.0f;
    LOG << 3.1415926;
    LOG << (short) 1;
    LOG << (long long) 1;
    LOG << (unsigned int) 1;
    LOG << (unsigned long) 1;
    LOG << (unsigned long long) 1;
    LOG << 'c';
    LOG << "abcdefg";
    LOG << string("This is a string");
    cout << "------------type test finished------------" << endl;
}

void stressing_single_thread()
{
    cout << "------------stressing test single_thread------------" << endl;
    threadFunc();
    cout << "------------stressing test single_thread finished------------" << endl;
}

void threadFunc()
{
    for(int i=0;i<100000;i++)LOG<<i;
}

void stressing_multi_threads(int threadNum)
{
    cout << "------------stressing test multi_(4)threads------------" << endl;
    cout << "------------out put 4*100000 logs------------" << endl;
    vector<shared_ptr<Thread>> vsp;
    char threadName[13]="threadFunc:";
    for (int i = 0; i < threadNum; ++i)
    {
        threadName[12]=i+'0';
        shared_ptr<Thread> tmp(new Thread(threadFunc, threadName));
        vsp.push_back(tmp);
    }
    for (int i = 0; i < threadNum; ++i)
    {
        vsp[i]->start();
    }
}



