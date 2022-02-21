## Pronto源代码笔记
* Savitar_thread_notify函数里的method_tag是每种structure单独编码的，比如map的insert_tag和vector的push_back_tag都是1 
* benchmark入口：./run-perf.sh -> benchmark(可执行文件) —> benchmark/main.cpp -> context.cpp/savitar_main -> thread.cpp/Savitar_thread_create
    * Savitar_thread_create里新建logger thread和main thread
    * 使用pthread_create创建，传入routine_wrapper 和 logger_cfg，真正的routine在logger_cfg -> routine，参数是logger_cfg -> argument
        * 其中routine_wrapper就是启动真正的routine并传入参数，并在做完后分不同routine进行clean up
            * 比如如果是logger thread对应的Savitar_persister_worker，要释放cpu core
            * 比如如果是main_thread对应的，要取消NVManager的thread register
        * main_thread的real routine是workload里面对应的对index的操作函数(就是context.cpp里面的main_wrapper，就是启动main.cpp里面的runBenchmark和runNvBenchmark)
        * log_thread的real routine是persister.cpp/Savitar_persister_worker，涉及很多与main_thread的交互，与main_thread的交互靠buffer和tx_buffer
        * Savitar_persister_worker具体内容：
            * 第一段 active_tx_id和tx_buffer[0]的比较那一段没看懂？感觉要结合main_thread发起的notify才能看懂
            * 如果当前method_tag == 0:
            * 如果当前method_tag != 0:（正式写log就是在这个位置）
                * 写当前log，用到的是当前nv_object->AppendLog() -> nv_log.cpp/Savitar_log_append()
                    * 其中用到__sync_fetch_and_add原子指令以及pmem_memcpy_nodrain、pmem_drain、pmem_persist等持久化操作
                    * 首先写入log entry的长度要与cache_line对齐
                    * 使用__sync_fetch_and_add更新log->tail
                    * 使用pmem_memcpy_nodrain()更新多个value，后跟drain()
                        * pmem_persist() =  pmem_flush + pmem_drain()
                        * flush是将cache里的脏数据flush到pmem的wpq（write pending queue）
                        * drain是阻塞等待buffer里的数据正式写入pmem
    * 真正的benchmark开始运行是runBenchmark里面的bench.run()，实现在base.cpp的Benchmark类里面
* 以上这些都是在本次benchmark（单一数据结构，但有很多次operation，参数一致）启动后建立main_thread和logger_thread并进行相关初始化，真正开始执行具体操作时，是在structure的具体operation实现方法中调用Savitar_thread_notify和Savitar_thread_wait（operation的实现是在main_thread中被执行的）使main_thread通知logger_thread做持久化log，并等待其完成操作。
* 而benchmark的一个个operation是怎么被激活开始run的呢，是benchmark/main.cpp里面的runBenchmark()，调用的是Benchmark基类的run()。在benchmark的work()里面有总benchmark的计时代码而不是对于单次operation的记时NDD
* Benchmark及其派生类解析：
    * 构造函数：
        * 清零计时器
        * 根据configuration里面的valueSize随机填充valueBuffer作为后续操作用到的操作数 
        * 解析trace文件存放在bench->traces里面并更新总operations数
    * run()
        * 新建线程开始worker然后join
    * worker() (这个worker不是派生类的worker,派生类的worker是纯虚函数，这个worker是static的）
        * 每一个worker会被传入一个threadID
        * 首先调用派生类的init()来创建数据结构本体，并加到bench的objetcs里面，threadID为标识，单线程模式下obhetcs里面就一个object
        * 对标识同步的sync做原子性加一__sync_add_and_fetch，如果sync等于threads number时，对t1计时（也就是说多线程的时候最后一个运行到这的thread负责对t1计时
        * 然后等待所有thread都就绪，开始做各自派生类的worker()，也就是具体的任务，比如vector就是把trace读出来不断的push_back(valueBuffer, trace[I])（注意这里用__sync_synchronize()等待所有thread准备就绪）
        * 做完后用原子性减一__sync_sub_and_fetch()，来实现最后一个昨晚的thread对t2计时
        * Clean up 
    * 派生类
        * 注意persist版本的派生类如vector头文件里除了benchmark的派生还有persistentObject的派生，重新实现operation并定制化log、replay等操作（因为log等具有数据结构差异性）
* Persister.cpp/Savitar_thread_notify具体内容：
    * 用va_list对可变参数进行解析
    * 先检查perisit_obejct是否处于recovery状态并进行相应操作
    * 依次将可变参数解析出来的本次operation参数填入sync_buffer的新元素内，并更新tx_buffer以及methods_tag等 NDD
* 区分一下NvVector:Benchmark和PersistentVector:PersistentObject NDD
* thread configuration里面的buffer和tx_buffer
    * Cfg->buffer是sync_buffer，就是记录persistentObject，operation methods和args的数组buffer
    * Cfg->tx_buffer用来index sync_buffer，tx_buffer[0]是当前线程活跃transaction的数量，tx_buffer[1+]是对应redo-log entry的offset
* 周期性实现snapshot的方法：
    * benchmark启动时启动Snapshot.sh而snapshot.sh在获取benchmark pid后按照设定的周期向pid发送SIGUSR1信号
    * context.cpp/savitar_main 中注册了信号处理函数，创建snapshot线程进行快照工作（有上锁）
* 计时方法
    * 最后输出报告是benchmark的printReport()
    * 是用clock_gettime()对t1、t2设置当前时间
    * 在Savitar_thread_wait中直接就可以完成目标计时NDD,但是是分operarion的，需要加合起来


## 70服务器跑通Pronto遇到的坑
* dependency.sh更改 
    * apt-get -> yum 有很多包找不到，用#标记
    * wget手动安装
    * Get-pip.py更改下载地址（python2版本）
* init_ext4.sh /dev/pmem12更改
* src/savitar.hpp MAX_CORE 80更改
* 源码debug：
    * ckpt_alloc.cpp:361 #if DEBUG -> #ifdef DEBUG
* error: ‘MAP_HUGE_SHIFT’ was not declared in this scope: #define MAP_HUGE_SHIFT 26
* 编译jemalloc时
    * touch doc/jemalloc.html
    * touch doc/jemalloc.3
* Pmdk-1.4在70服务器上编译不过 改成了1.10版本
* apt install libpmemobj-cpp-dev
* 把70上的C++版本从9切换为7 https://blog.csdn.net/weixin_40628128/article/details/89839268
    * 要不然benchmark编译出错，与STL map等有关