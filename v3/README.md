## myReactorServer v3特性概述

### (已完成)

* 基于对象编程，暴露一个具体类，以注册回调函数实现控制反转。
避免面向对象方式暴露一个抽象基类，用户需以继承方式承担虚指针与虚表开销，以及面对多重继承、可扩展性不足等问题。
* 事件驱动架构(one event loop per thread) + Thread Pool。
* 主eventloop线程 + I/O worker eventloop线程 + 基于阻塞队列的任务队列计算线程 + 四缓冲异步日志线程。
单进程多线程设计，提升并发性。发挥多核性能，降低延迟。

* 内存分配
    * 对资源使用预分配策略，进行池化处理，设计连接池(参考Nginx连接池设计)，避免因频繁内存分配影响性能。
* 数据拷贝
    * Buffer的特别设计，参照Redis的SDS字符串设计，使用Buffer chain，并对其使用scatter I/O，减少系统调用(read(), size(), realloc()等)次数。
    * 使用C++ 11的移动语义，避免拷贝。
    * 使用epoll LT，较selet与poll减少一次由用户空间向内核空间的拷贝。

* 上下文切换
    * 分工明确的多线程设计 + EventLoop::runInLoop() + EventLoop::queueInLoop()避免线程上下文切换开销。
* 锁竞争
    * 通过逻辑与设计(master + workers + others线程)减少锁的使用，且仅使用NPTL的互斥锁mutex。
        对于需要磁盘写入的日志线程，采取异步形式，减少对计算及I/O线程的影响。
    * 使用Copy-on-Write等手法尽可能的缩小临界区(EventLoop::doPendingFunctors()等)。


### (待完成)

* 多阶段请求异步处理。(类Nginx实现)
* 内存池的使用。(类Nginx实现)
* 使用更多针对性能优化的数据结构。(类Redis实现)

* 模块化设计。(类Nginx实现)
* 网络库与应用逻辑分离设计，提高可重用性。


### (已放弃方案)
* Epoll ET的使用。(性能原因，见[v3性能评估.md](https://github.com/ChyauAng/myReactorServer/blob/master/v3/v3性能评估.md))
