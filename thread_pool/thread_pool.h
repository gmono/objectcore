#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <functional>
#include <thread>
#include <list>
#include <time.h>
#include <queue>
#include <unordered_map>
#include <mutex>
using std::unordered_map;
using std::list;
using std::thread;
using std::function;
using std::queue;
typedef unsigned int uint;
/**
 * @brief 线程池
 *
 */
class Thread_pool
{

public:
    /**
     * @brief Thread_pool 初始化线程池，注意，参数传错后果自负，大小分清楚
     * @param maxthread 最大线程数，-1为不做限制
     * @param minthread 最小保持线程数
     * @param maxsleep  任务最大等待时间，秒，0为不允许等待，在没有达到maxthread时，没有空余线程就直接创建一个
     * @param maxtsl    线程最大休眠时间，秒，0为不允许休眠，任务结束后直接销毁，不推荐
     * @param st   任务扫描间隔 毫秒
     */
    Thread_pool(int maxthread, int minthread, double maxsleep, uint maxtsl, uint st);
    /**
     * @brief AddTask   将一个任务加入到任务池中
     * @param task 任务
     */
    void AddTask(function<void(void)> task);

    ~Thread_pool();

private:

    struct Task
    {
        function<void(void)> tfun;
        time_t jointime; //加入是的时间
    };

    struct Thread
    {
        thread *t;
        uint stime; //递减,单位 秒
    };

    std::mutex thpl;
    std::mutex tapl;
    thread *mth;
    queue<Task,list<Task>> taskpool;
    std::mutex depl;
    queue<std::thread *,list<std::thread *>> deletequeue;
    unordered_map<thread::id,Thread> threadpool;
    int maxts,mints;
    uint mtsl;
    double msl;
    uint mst;
    /**
     * @brief ThreadMain 池中线程主循环函数
     */
    void ThreadMain();

    void AddThread();

    /**
     * @brief MainLoop 主循环
     */
    void MainLoop();



    bool end=false;


};

#endif // THREAD_POOL_H
