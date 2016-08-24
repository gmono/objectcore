#include "objectcore.h"
#include "QueryEntrance.h"
#include "actionentrance.h"
#include "managerentrance.h"
#include "IMObject.h"
#include "thread_pool.h"
ObjectCore::ObjectCore(uint stime)
{
    qeno=new QueryEntrance(this);
    meno=new ManagerEntrance(this);
    aeno=new ActionEntrance(this);
    tpool=new Thread_pool(100,10,stime,10,10);
}

ObjectCore::~ObjectCore()
{
    delete qeno;
    delete meno;
    delete aeno;
}


void ObjectCore::GetEntrance(QueryEntrance *&qo, ActionEntrance *&ao, ManagerEntrance *&mo)
{
    qo=qeno;
    mo=meno;
    ao=aeno;
}
