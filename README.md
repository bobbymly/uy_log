# uy_log

由于在多线程环境下频繁的文件读写会造成很大的性能损失，故采用双缓冲区设计。
* 一个 Log 线程（单例）负责把各个线程的 log 信息写入文件，由于只有一个线程写入文件，可以使用无锁的 fwrite_unlocked(),收集满一定数量的信息（减少磁盘写频度）或到达一定的时间间隔（防止发生意外时 log 信息的丢失）才向文件中写入 log。
* 预先准备两个缓冲区 A B ,由各个产生 Log 信息的线程 向 A 缓冲区写入， 当 A 写满后 交换 A B 缓冲区(使用 std::move 和 swap 避免内存拷贝造成的性能损失)， Log 线程负责把缓冲区 B 中的 log 写入文件。

Log 的实现包括许多文件层层递进

FileUtil 包含了Log文件的 打开、关闭、写入，使用自定义缓冲区和无锁 fwrite_unlocked()。
LogFile 封装了FileUtil并设置了一个循环次数，每添加这么多次 Log 信息就向文件中写一次。
LogStream 重载各种<<运算符。
AsyncLogging 负责启动一个LOG线程，应用双缓冲区技术。
Logging 对外的接口，内含一个LogStream对象，在每次打Log时为其添加 文件名 和 行数 等格式化信息。

## 缓冲区组件 FixedBuffer

* 各文件中使用统一的缓冲区对象，方便编程。

## 互斥锁组件 MutexLock 和 MutexLockGuard

* MutexLock 封装 mutex 相关的操作。
* MutexLockGuard 使用 RAII 的方法封装 MutexLock ，方便使用。

## 条件变量 Condition 和 计数器 CountDownLatch

* Condition 封装了条件变量的相关操作。
* CountDownLatch 使用 Condition 实现计数器功能，用以做线程间的同步。

## 线程组件 Thread

* 封装线程的相关操作，通过条件变量组件(condition/CountDownLatch)保证线程在成功创建后本线程才继续执行。
* 使用__thread 关键字保存线程 PID 等线程信息，避免使用时的重复系统调用造成的性能损失。
* 为防止线程结束时资源未被回收变成僵尸线程，在析构时若未被 join 则应 detach。
