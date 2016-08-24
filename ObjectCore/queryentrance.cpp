#include "queryentrance.h"
#include "objectcore.h"
#include "extern.h"
using std::string;
using std::vector;

QueryEntrance::QueryEntrance(ObjectCore *oc)
{
    this->oc=oc;

}

bool QueryEntrance::GetNameById(uint id, std::string &name)
{
    return GetOneByOne(itnt,id,name);
}

bool QueryEntrance::GetIdsByName(std::string name, function<void(uint)> addfun)
{
    short t=GetMulByOne(ntist,name,[&addfun](uint &r)->bool{
            addfun(r);
            return true;});
    if(t==0) return true;
    else return false;
}

bool QueryEntrance::GetIdsByClass(std::string classname, function<void(uint)> addfun)
{
    short t=GetMulByOne(ctist,classname,[&addfun](uint &r)->bool{
            addfun(r);
            return true;});
    if(t==0) return true;
    else return false;
}

bool QueryEntrance::GetIdBySid(std::string sid, uint &result)
{
    return GetOneByOne(stit,sid,result);
}

bool QueryEntrance::GetSidById(uint id, std::string &result)
{
    return GetOneByOne(itst,id,result);
}

bool QueryEntrance::IsStart(uint oid)
{
    auto ptr=startt.find(oid);
    if(ptr==startt.end()) return false;
    return ptr->second;
}

bool QueryEntrance::AddobjEvent(IObjectInfo &info)
{
    if(stit.find(info.sid)!=stit.end()) return false;
    itnt.insert(std::pair<uint,string>(info.id,info.name));
    itst.insert(std::pair<uint,string>(info.id,info.sid));
    stit.insert(std::pair<string,uint>(info.sid,info.id));

    ntist.insert(std::pair<string,uint>(info.name,info.id));
    ctist.insert(std::pair<string,uint>(info.classname,info.id));

    startt.insert(std::pair<uint,bool>(info.id,false));
    infos.insert(std::pair<uint,IObjectInfo>(info.id,info));
    return true;
}

void QueryEntrance::RemoveEvent(uint oid)
{
    auto ptr=infos.find(oid);
    if(ptr==infos.end()) return;
    IObjectInfo &info=ptr->second;
    itnt.erase(oid);
    itst.erase(oid);
    stit.erase(info.sid);
    ntist.erase(info.name);
    ctist.erase(info.classname);
    infos.erase(oid);
    startt.erase(info.id);
}

void QueryEntrance::ReObjIdEvnet(uint old, uint newid)
{
    auto ptr=infos.find(old);
    if(ptr==infos.end()) return;
    IObjectInfo &info=ptr->second;
    ReKey(infos,old,newid);
    ReKey(startt,old,newid);
    ReKey(itnt,old,newid);
    ReKey(itst,old,newid);
    stit[info.sid]=newid;
    ReValues(ntist,info.name,old,newid,1);
    ReValues(ctist,info.classname,old,newid,1);
}

void QueryEntrance::StartObject(uint oid)
{
    auto ptr=startt.find(oid);
    if(ptr==startt.end()) return;
    ptr->second=true;
}

void QueryEntrance::StopObject(uint oid)
{
    auto ptr=startt.find(oid);
    if(ptr==startt.end()) return;
    ptr->second=false;
}

bool QueryEntrance::GetObjectInfo(uint id, IObjectInfo &info)
{
    auto ptr=infos.find(id);
    if(ptr==infos.end()) return false;
    info=ptr->second;
    return true;
}



