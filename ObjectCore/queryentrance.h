#ifndef QueryEntrance_H
#define QueryEntrance_H
#include "Common.h"
#include "IObject.h"
using std::string;
using std::vector;
class ObjectCore;
class QueryEntrance
{
    friend class ActionEntrance;
    friend class ManagerEntrance;
    friend class ObjectCore;
public:
    


    bool GetNameById(uint id,string &name);
    bool GetIdsByName(string name,function<void(uint)> addfun);
    bool GetIdsByClass(string classname,function<void(uint)> addfun);
    bool GetIdBySid(string sid,uint &result);
    bool GetSidById(uint id,string &result);

    bool IsStart(uint oid);
private:
    QueryEntrance(ObjectCore *oc);
    
    
    bool AddobjEvent(IObjectInfo &info);
    void RemoveEvent(uint oid);
    void ReObjIdEvnet(uint old,uint newid);
    void StartObject(uint oid);
    void StopObject(uint oid);


    bool GetObjectInfo(uint id,IObjectInfo &info);
    umap<uint,string> itnt; //id to name
    umap<string,uint> stit;//SID to ID
    umap<uint,string> itst;//ID to SID

    mumap<string,uint> ntist;   //由名字得到一群同名的对象id
    mumap<string,uint> ctist;   //由classname得到一群同类对象ID

    umap<uint,IObjectInfo> infos;
    umap<uint,bool> startt;
    //以下对象不应被释放
    ObjectCore *oc;



};

#endif // QueryEntrance_H
