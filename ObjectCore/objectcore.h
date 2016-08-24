#ifndef OBJECTCORE_H
#define OBJECTCORE_H
#include "Common.h"
class QueryEntrance;
class ActionEntrance;
class ManagerEntrance;
class Thread_pool;
class ObjectCore
{
    friend class QueryEntrance;
    friend class ActionEntrance;
    friend class ManagerEntrance;

public:
    /**
     * @brief ObjectCore
     * @param stime 异步消息级别，也就是等待每个消息最大等待其他消息执行完毕的时间
     */
    ObjectCore(uint stime);
    ~ObjectCore();
    void GetEntrance(QueryEntrance *&qo,ActionEntrance *&ao,ManagerEntrance *&mo);
private:
    QueryEntrance *qeno;
    ActionEntrance *aeno;
    ManagerEntrance *meno;

    Thread_pool *tpool;

};

#endif // OBJECTCORE_H
