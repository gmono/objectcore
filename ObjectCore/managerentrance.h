#ifndef MANAGERENTRANCE_H
#define MANAGERENTRANCE_H
#include "IMObject.h"
#include "Common.h"
#include "IObject.h"
class Thread_pool;
using std::vector;
using std::string;
class ObjectCore;


class ManagerEntrance
{
    friend class QueryEntrance;
    friend class ActionEntrance;
    friend class ObjectCore;
public:
    

    ~ManagerEntrance();

    bool AddMObject(IMObject *mobj, string &sid, string *msid=nullptr);
    void RemoveMObject(string sid);
    IMObject *GetMObjBySID(string sid);
    vector<IMObject *> GetMObjsByMSID(string msid);

    bool GetInfoBySID(string sid, const IMObjectInfo *&result);
    bool TestMObj(string sid);  //测试指定sid的MOBJ是否可用
    bool TestMObj(IMObject *mobj);  //测试指定的对象指针的对象是否可用
    int GetMObjCountByMSID(string msid);

private:
    ManagerEntrance(ObjectCore *oc);
    
    bool AddEvent(IObject *obj,const IObjectInfo &info);

    void RemoveEvent(const IObjectInfo &info);
    void ReObjIdEvent(IObject *obj,const IObjectInfo &curinfo,uint oldid);
    void StartObjEvent(const IObjectInfo &info);
    void StopObjEvent(const IObjectInfo &info);
    void AddedEvnet(const IObjectInfo &info);

    const map<string,short> &GetMILTable(uint oid);  //获取指定id对象的MIL表

    //MIL表集合
    //OBJECT相关
    umap<uint,map<string,short>>  milts;
    mumap<string,uint> listent; //为每个Mobj的SID保存一个监听的对象的集合方便查询
    //MOBJECT相关
    umap<string,IMObject *> stmot;  //sid to mobj
    mumap<string,IMObject *> mstmot;    //msid to mobjs

    umap<string,IMObjectInfo> infos;

    umap<IMObject *,string> tmqt;   //反向查询表
    uset<string> mobjset;   //mobj集合
    //以下对象不应被释放
    ObjectCore *oc;
    

    IMObject *GetMobject(string sid);
    //以下为辅助函数
    void FillFuns(const IMObjectInfo &info,MFunctions &funs);
    
    void ReRegister(string sid); //在有一个新的MOBJ时注册所有和这个SID相关联的对象,并更新MIL表
    void DeleteUpdate(string sid);  //当删除时更新所有MIL表
    map<string,short> *RegisterListen(const IObjectInfo *info, IObject *obj);
};

#endif // MANAGERENTRANCE_H
