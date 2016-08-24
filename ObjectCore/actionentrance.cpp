#include "actionentrance.h"
#include "objectcore.h"
#include "queryentrance.h"
#include "managerentrance.h"
#include "extern.h"
#include "thread_pool.h"
ActionEntrance::ActionEntrance(ObjectCore *oc)
{
    this->oc=oc;
}

bool ActionEntrance::AddObject(IObject *obj, uint &result)
{
    uint tid=GetRandId();
    bool ret=AddObjectForID(obj,tid);
    if(ret)
    {
        result=tid;
        return true;
    }
    else return false;
}

bool ActionEntrance::AddObjectForID(IObject *obj, uint nid)
{

    if(itot.find(nid)!=itot.end()) return false;
    obj->Init();
    IObjectInfo info;
    info.id=nid;
    info.name=obj->Name();
    info.classname=obj->ClassName();
    info.sid=obj->StringID();
    info.mrlist=obj->MRList();
    if(!oc->qeno->AddobjEvent(info))
    {
        obj->AddError();
        return false;
    }
    if(!oc->meno->AddEvent(obj,info))
    {
        oc->qeno->RemoveEvent(nid);
        obj->AddError();
        return false;
    }
    ObjectFuns funs;
    funs.DeleteMe=[nid,this](){this->DeleteObject(nid);};
    funs.RemoveMe=[nid,this]()->bool{return this->RemoveObject(nid)==0;};
    funs.GetMILForMe=[nid,this]()->const map<string,short>&{return this->oc->meno->GetMILTable(nid);};

    funs.SendSyncMSGToObj=[this,info](uint id,ObjectDBase data)->ObjectDBase{
        IObject *obj=GetObject(id);
        if(obj==nullptr) return ObjectDBase();
        data.id=info.id;
        data.sid=info.sid;
        ObjectDBase ret=obj->ObjMSGResponst(data);
        ret.id=id;
        oc->qeno->GetSidById(id,ret.sid);
        return ret;
    };
    funs.SendASyncMSGToObj=[this,info](uint id,ObjectDBase data,function<void(ObjectDBase)> cbk)
    {
        IObject *obj=GetObject(id);
        if(obj==nullptr) return;
        data.id=info.id;
        data.sid=info.sid;
        auto fun=[this,info,data,cbk,id,obj](){
            ObjectDBase ret=obj->AObjMSGResponst(data);
            ret.id=id;
            oc->qeno->GetSidById(id,ret.sid);
            cbk(ret);
        };
        oc->tpool->AddTask(fun);
    };

    funs.SendSyncMSGToMObj=[this,info](string sid,ObjectDBase data)->DataBase{
        //检测
        const map<string,short> mil=oc->meno->GetMILTable(info.id);
        auto ptr=mil.find(sid);
        if(ptr==mil.end()||ptr->second!=0) return DataBase();


        IMObject *mobj=oc->meno->GetMobject(sid);
        if(mobj==nullptr) return DataBase();
        data.id=info.id;
        data.sid=info.sid;
        auto ret=mobj->ObjSyncMSGResponst(data);
        ret.sid=sid;
        return ret;
    };

    funs.SendASyncMSGToMObj=[this,info](string sid,ObjectDBase data,function<void(DataBase)> cbk)
    {
        //检测
        const map<string,short> mil=oc->meno->GetMILTable(info.id);
        auto ptr=mil.find(sid);
        if(ptr==mil.end()||ptr->second!=0) return;

        IMObject *mobj=oc->meno->GetMobject(sid);
        if(mobj==nullptr) return;
        data.id=info.id;
        data.sid=info.sid;
        auto fun=[mobj,sid,data,cbk](){
            auto ret=mobj->ObjSyncMSGResponst(data);
            ret.sid=sid;
            cbk(ret);
        };

        oc->tpool->AddTask(fun);
    };

    itot.insert(pair<uint,IObject *>(nid,obj));
    otit.insert(pair<IObject *,uint>(obj,nid));
    obj->Start(funs,nid);
    oc->qeno->StartObject(nid);
    oc->meno->StartObjEvent(info);

    oc->meno->AddedEvnet(info);
    return true;
}

int ActionEntrance::RemoveObject(uint id)
{
    IObject *obj=GetObject(id);
    if(!obj) return false;
    int ec;
    if((ec=obj->Remove())) return ec;    //征得对象同意,若返回0代表可以卸载

    //开始
    IObjectInfo info;
    oc->qeno->GetObjectInfo(id,info);
    obj->Stop();
    oc->qeno->StopObject(id);   //设置对象状态为“停止”
    oc->meno->StopObjEvent(info);


    oc->meno->RemoveEvent(info);    //通知ME
    oc->qeno->RemoveEvent(id);  //通知QE
    //结束


    obj->End(); //通知对象它已经被卸载
    return 0;
}
/**
 * @brief 删除对象
 * @param 对象id
 * ME接到通知时QE里还有原对象信息
 */
void ActionEntrance::DeleteObject(uint id)
{
    IObject *obj=GetObject(id);
    if(!obj) return;
    obj->Delete();  //仅通知,不征得对象同意，直接强制删除

    //开始
    IObjectInfo info;
    oc->qeno->GetObjectInfo(id,info);
    obj->Stop();
    oc->qeno->StopObject(id);   //设置对象状态为“停止”
    oc->meno->StopObjEvent(info);

    oc->meno->RemoveEvent(info);    //通知ME
    oc->qeno->RemoveEvent(id);  //通知QE
    //结束


    obj->End();
}
/**
 * @brief 重命名对象id
 * @param 对象id
 * @param 新ID
 * @return 成功与否
 *
 * 在ME收到通知时QE中已经完成更新
 */
bool ActionEntrance::ReObjId(uint id, uint nid)
{
    auto ptr=itot.find(id);
    if(ptr==itot.end()) return false;

    //开始
    IObjectInfo info;
    oc->qeno->GetObjectInfo(id,info);
    IObject *obj=ptr->second;
    obj->Stop();


    oc->qeno->StopObject(id);

    oc->meno->StopObjEvent(info);



    //更改表项
    otit[obj]=nid;
    ReKey(itot,id,nid);

    oc->qeno->ReObjIdEvnet(id,nid); //通知QE

    oc->qeno->GetObjectInfo(nid,info);  //获取更新后的信息
    oc->meno->ReObjIdEvent(obj,info,id);    //通知ME

    oc->qeno->StartObject(nid);

    obj->ReStart(); //通知对象qe已经将它启动
    oc->meno->StartObjEvent(info);  //通知ME它已经开始执行
    obj->ReIded(id,nid);    //通知对象它已经被更新id
    return true;
}


IObject *ActionEntrance::GetObject(uint id)
{
    auto ptr=itot.find(id);
    if(ptr==itot.end()) return nullptr;
    return ptr->second;
}

bool ActionEntrance::GetObjectId(IObject *obj,uint &result)
{
    auto ptr=otit.find(obj);
    if(ptr==otit.end()) return false;
    result=ptr->second;
    return true;
}


uint ActionEntrance::GetRandId()
{
    uint t;
    do
    {
        t=(uint)rand();
    }while(itot.find(t)!=itot.end());
    return t;
}
