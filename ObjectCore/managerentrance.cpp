#include "managerentrance.h"
#include "objectcore.h"
#include "actionentrance.h"
#include "queryentrance.h"
#include "thread_pool.h"
//
ManagerEntrance::ManagerEntrance(ObjectCore *oc)
{

    this->oc=oc;

}

ManagerEntrance::~ManagerEntrance()
{
}

bool ManagerEntrance::AddMObject(IMObject *mobj, string &sid, string *msid)
{
    mobj->Init();
    IMObjectInfo info;
    info.sid=mobj->StringID();
    if(stmot.find(info.sid)!=stmot.end()) return false; //如果SID有冲突则直接放弃
    
    info.msid=mobj->MRStringID();
    info.name=mobj->Name();

    
    stmot.insert(pair<string,IMObject *>(info.sid,mobj));
    mstmot.insert(pair<string,IMObject *>(info.msid,mobj));
    infos.insert(pair<string,IMObjectInfo>(info.sid,info));
    tmqt.insert(pair<IMObject *,string>(mobj,info.sid));
    //填充函数集
    MFunctions funs;
    FillFuns(info,funs);
    
    mobj->Start(oc->aeno,oc->qeno,funs);//启动
    
    mobjset.insert(info.sid);   //正式可用
    
    ReRegister(info.sid);   //注册
    
    sid=info.sid;
    if(msid) *msid=info.msid;
    return true;
}

void ManagerEntrance::RemoveMObject(string sid)
{
    if(mobjset.find(sid)==mobjset.end()) return;
    IMObject *mobj=stmot[sid];
    mobj->Delete();
    
    //开始删除
    mobjset.erase(sid);
    
    IMObjectInfo &info=infos[sid];
    stmot.erase(sid);
    mstmot.erase(info.msid);
    tmqt.erase(mobj);
    DeleteUpdate(sid);  //更新表项
    infos.erase(sid);
}

IMObject *ManagerEntrance::GetMObjBySID(string sid)
{
    auto ptr=stmot.find(sid);
    if(ptr==stmot.end()) return nullptr;
    return ptr->second;
}

vector<IMObject *> ManagerEntrance::GetMObjsByMSID(string msid)
{
    vector<IMObject *> ret;
    auto range=mstmot.equal_range(msid);
    for(auto i=range.first;i!=range.second;++i)
    {
        ret.push_back(i->second);
    }
    return ret;
    
}

bool ManagerEntrance::GetInfoBySID(string sid, const IMObjectInfo *&result)
{
    auto ptr=infos.find(sid);
    if(ptr==infos.end()) return false;
    result=&(ptr->second);
    return true;
}

bool ManagerEntrance::TestMObj(string sid)
{
    if(mobjset.find(sid)==mobjset.end()) return false;
    return true;
}

bool ManagerEntrance::TestMObj(IMObject *mobj)
{
    auto ptr=tmqt.find(mobj);
    if(ptr==tmqt.end()) return false;
    return TestMObj(ptr->second);   //最终还是要查询总表
}

int ManagerEntrance::GetMObjCountByMSID(string msid)
{
    int ret=0;
    auto ragne=mstmot.equal_range(msid);
    for(auto i=ragne.first;i!=ragne.second;++i)
    {
        ++ret;
    }
    return ret;
}

bool ManagerEntrance::AddEvent(IObject *obj, const IObjectInfo &info)
{
    //根据info中的监听表更新listent
    for(string str:info.mrlist)
    {
        auto ptr=mobjset.find(str);
        if(ptr!=mobjset.end())
        {
            listent.insert(pair<string,uint>(str,info.id));
        }
    }
    //
    auto mil=RegisterListen(&info,obj);
    milts.insert(pair<uint,map<string,short>>(info.id,*mil));
    return true;
}

void ManagerEntrance::RemoveEvent( const IObjectInfo &info)
{
    auto &m=milts[info.id];
    for(auto t:m)
    {

        if(t.second==0)
        {
            IMObject *mobj=stmot[t.first];
            mobj->Responst(RType::Delete,&info);
        }
    }
    //从listent中删除所有与其相关的对象
    for(string t:info.mrlist)
    {
        auto range=listent.equal_range(t);
        for(auto i=range.first;i!=range.second;++i)
        {
            if(i->second==info.id)
            {
                listent.erase(i);
                break;
            }
        }
    }
    //
    milts.erase(info.id);
}

void ManagerEntrance::ReObjIdEvent(IObject *obj, const IObjectInfo &curinfo, uint oldid)
{
    auto &m=milts[oldid];
    for(auto t:m)
    {
        if(t.second==0)
        {
            IMObject *mobj=stmot[t.first];
            mobj->Responst(ReSetId,&curinfo,obj,oldid);
        }
    }

    for(auto t:curinfo.mrlist)
    {
        auto range=listent.equal_range(t);
        for(auto i=range.first;i!=range.second;++i)
        {
            if(i->second==oldid)
            {
                i->second=curinfo.id;
                break;
            }
        }
    }
    milts.insert(pair<uint,map<string,short>>(curinfo.id,m));
    milts.erase(oldid);

}

void ManagerEntrance::StartObjEvent(const IObjectInfo &info)
{
    auto &m=milts[info.id];
    for(auto t:m)
    {
        if(t.second==0)
        {
            IMObject *mobj=stmot[t.first];
            mobj->Responst(RType::Start,&info);
        }
    }
}

void ManagerEntrance::StopObjEvent(const IObjectInfo &info)
{
    auto &m=milts[info.id];
    for(auto t:m)
    {
        if(t.second==0)
        {
            IMObject *mobj=stmot[t.first];
            mobj->Responst(RType::Stop,&info);
        }
    }
}

void ManagerEntrance::AddedEvnet(const IObjectInfo &info)
{
    auto &m=milts[info.id];
    for(auto t:m)
    {
        if(t.second==0)
        {
            IMObject *mobj=stmot[t.first];
            mobj->Responst(RType::Added,&info);
        }
    }
}

const map<string, short> &ManagerEntrance::GetMILTable(uint oid)
{
    return milts[oid];
}

IMObject *ManagerEntrance::GetMobject(string sid)
{
    if(mobjset.find(sid)==mobjset.end()) return nullptr;
    return stmot[sid];
}

void ManagerEntrance::FillFuns(const IMObjectInfo &info, MFunctions &funs)
{
    funs.DeleteMe=[this,info](){this->RemoveMObject(info.sid);};
    funs.GetInfoTableByMSID=[this](string msid)->const set<IMObjectInfo *>{
        set<IMObjectInfo *> ret;
        auto range=mstmot.equal_range(msid);
        for(auto i=range.first;i!=range.second;++i)
        {
            auto mobjp=i->second;
            string sid=tmqt[mobjp];
            IMObjectInfo &info=infos[sid];
            ret.insert(&info);
        }
        return ret;
    };
    funs.GetInfoTableBySID=[this](string sid)->IMObjectInfo &{
        return infos[sid];
    };
    funs.GetMSIDCount=[this](string msid)->size_t{
        return this->mstmot.count(msid);
    };
    funs.GetNRObjectsForMe=[this,info]()->vector<uint>{
        vector<uint> ret;
        auto range=listent.equal_range(info.sid);
        for(auto i=range.first;i!=range.second;++i)
        {
            uint id=i->second;
            if(milts[id][info.sid]==1) ret.push_back(id);
        }
        return ret;
    };
    funs.GetRObjectsForMe=[this,info]()->vector<uint>{
        vector<uint> ret;
        auto range=listent.equal_range(info.sid);
        for(auto i=range.first;i!=range.second;++i)
        {
            uint id=i->second;
            if(milts[id][info.sid]==0) ret.push_back(id);
        }
        return ret;
    };
    funs.GetObjectsForMe=[this,info]()->vector<uint>{
        vector<uint> ret;
        auto range=listent.equal_range(info.sid);
        for(auto i=range.first;i!=range.second;++i)
        {
            ret.push_back(i->second);
        }
        return ret;
    };
    funs.TestSID=[this](string sid)->bool{
        if(mobjset.find(sid)==mobjset.end()) return false;
        return true;
    };

    funs.SendSyncInfo=[this,info](string sid,DataBase data)->DataBase{
        auto ptr=stmot.find(sid);
        if(ptr==stmot.end()) return DataBase();
        IMObject *mobj=ptr->second;
        data.sid=info.sid;

        DataBase td= mobj->SyncInfoResponst(data);
        td.sid=sid;
        return td;
    };
    funs.SendASyncInfo=[this,info](string sid,DataBase data,function<void(DataBase)> cbk){
        auto ptr=stmot.find(sid);
        if(ptr==stmot.end()) return;
        IMObject *mobj=ptr->second;
        data.sid=info.sid;
        auto fun=[sid,data,cbk,mobj](){DataBase td=mobj->ASyncInfoResponst(data);td.sid=sid;cbk(td);};

        oc->tpool->AddTask(fun);
    };
    funs.SendSyncMsgToObj=[this,info](uint id,DataBase data)->ObjectDBase{
        IObject *obj=oc->aeno->GetObject(id);
        if(obj==nullptr) return ObjectDBase();
        data.sid=info.sid;
        ObjectDBase ret=obj->MObjMSGResponst(data);
        ret.id=id;
        oc->qeno->GetSidById(id,ret.sid);
        return ret;
    };
    funs.SendASyncMsgToObj=[this,info](uint id,DataBase data,function<void(ObjectDBase)> cbk)
    {
        IObject *obj=oc->aeno->GetObject(id);
        if(obj==nullptr) return;
        data.sid=info.sid;
        auto fun=[this,id,data,cbk,obj](){
            ObjectDBase ret=obj->AMObjMSGResponst(data);
            ret.id=id;
            oc->qeno->GetSidById(id,ret.sid);
            cbk(ret);
        };



        oc->tpool->AddTask(fun);
    };
}

void ManagerEntrance::ReRegister(string sid)
{
    auto range=listent.equal_range(sid);
    for(auto i=range.first;i!=range.second;++i)
    {
        auto &m=milts[i->second];
        IMObject *mobj=stmot[sid];
        IObjectInfo tinfo;
        oc->qeno->GetObjectInfo(i->second,tinfo);
        if(mobj->Responst(RType::Register,&tinfo)) m[sid]=0;
        else m[sid]=1;
    }
}

void ManagerEntrance::DeleteUpdate(string sid)
{
    auto range=listent.equal_range(sid);
    for(auto i=range.first;i!=range.second;++i)
    {
        auto &m=milts[i->second];
        m[sid]=2;
    }
}

map<string, short> *ManagerEntrance::RegisterListen(const IObjectInfo *info,IObject *obj)
{
    auto ret=new map<string,short>();
    for(string sid:info->mrlist)
    {
        if(mobjset.find(sid)==mobjset.end()) ret->insert(pair<string,short>(sid,2));
        IMObject *mobj=stmot[sid];
        if(mobj->Responst(RType::Add,info,obj)) ret->insert(pair<string,short>(sid,0));
        else ret->insert(pair<string,short>(sid,1));
    }
    return ret;
}
