#include "thread_pool.h"
using std::pair;
using std::unordered_map;
using std::list;
using std::thread;
using std::function;
using std::queue;
Thread_pool::Thread_pool(int maxthread, int minthread, double maxsleep, uint maxtsl,uint st)
{
    this->maxts=maxthread;
    this->mints=minthread;
    this->msl=maxsleep;
    this->mtsl=maxtsl;
    this->mst=st;
    mth=new thread([this](){this->MainLoop();});
}

void Thread_pool::AddTask(function<void ()> task)
{
    Task t;
    t.tfun=task;
    t.jointime=time(nullptr);
    tapl.lock();
    taskpool.push(t);
    tapl.unlock();
}

Thread_pool::~Thread_pool()
{

    end=true;

    for(;;)
    {
        thpl.lock();
        depl.lock();
        if(threadpool.empty()&&deletequeue.empty())
        {
            thpl.unlock();
            depl.unlock();
            break;
        }
        thpl.unlock();
        depl.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    mth->join();
    delete mth;

}

void Thread_pool::ThreadMain()
{
    thpl.lock();
    //由于此锁获得后，一下步骤必定已经完成，所以直接释放
    //if(threadpool.find(std::this_thread::get_id())==threadpool.end()) threadpool.insert(pair<thread::id,Thread>(std::this_thread::get_id(),t));
    thpl.unlock();
    while(!end)
    {
        tapl.lock();
        if(taskpool.empty())
        {
            tapl.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            thpl.lock();
            Thread tt=threadpool[std::this_thread::get_id()];
            tt.stime--;

            if(tt.stime==0&&threadpool.size()>this->mints)
            {
                thpl.unlock();
                break;
            }
            thpl.unlock();
            continue;
        }
        Task t=taskpool.front();
        taskpool.pop();
        tapl.unlock();

        t.tfun();

        thpl.lock();
        Thread tt=threadpool[std::this_thread::get_id()];
        tt.stime=this->mtsl;
        thpl.unlock();

    }

    thread::id ti=std::this_thread::get_id();
    thpl.lock();
    auto i=threadpool.find(ti);
    Thread tt=i->second;
    threadpool.erase(i);
    thpl.unlock();
    depl.lock();
    deletequeue.push(tt.t);
    depl.unlock();
}

void Thread_pool::AddThread()
{
    Thread t;
    t.stime=this->mtsl;
    t.t=new thread([this,t](){this->ThreadMain();});


    if(threadpool.find(t.t->get_id())==threadpool.end()) threadpool.insert(pair<thread::id,Thread>(t.t->get_id(),t));
}

void Thread_pool::MainLoop()
{
    while(!end)
    {
        //扫描销毁队列
        depl.lock();
        while(!deletequeue.empty())
        {
            auto i=deletequeue.front();
            deletequeue.pop();
            i->join();
            delete i;
        }
        depl.unlock();
        //若第一个任务时间间隔大于最大时间间隔
        thpl.lock();
        for(size_t i=threadpool.size();i<this->mints;++i)
        {
            AddThread();
        }

        tapl.lock();
        if(!taskpool.empty()&&threadpool.size()<this->maxts)
        {
            //线程增长问题已经解决
            const list<Task> &tl=taskpool._Get_container();
            for(auto t:tl)
            {
                if(threadpool.size()<this->maxts)
                {
                    time_t tt=t.jointime;
                    double ttt=difftime(time(nullptr),tt);
                    if(ttt>msl)
                    {
                        AddThread();
                    }
                    else break;
                }
                else break;
            }
            /*
            Task &t=taskpool.front();
            time_t tt=t.jointime;
            double ttt=difftime(time(nullptr),tt);
            if(ttt>msl)
            {
                AddThread();
            }*/
        }
        tapl.unlock();
        thpl.unlock();



        std::this_thread::sleep_for(std::chrono::milliseconds(mst));   //休眠指定间隔
    }
}
