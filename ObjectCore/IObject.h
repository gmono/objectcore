#ifndef IOBJECT_H
#define IOBJECT_H
#include "Common.h"
using std::map;
using std::function;
class QueryEntrance;
using std::string;
using std::vector;
struct IObjectInfo
{
    string name;
    string sid;
    uint id;
    string classname;
    vector<string> mrlist;
};
struct ObjectFuns
{
    function<bool(void)> RemoveMe;
    function<void(void)> DeleteMe;
    function<const map<string,short>&()> GetMILForMe;


    //发消息，加a的是异步，to后面mobj 和obj应该都懂吧
    function<DataBase(string,ObjectDBase)> SendSyncMSGToMObj;
    function<ObjectDBase(uint,ObjectDBase)> SendSyncMSGToObj;

    function<void(string,ObjectDBase,function<void(DataBase)>)> SendASyncMSGToMObj;
    function<void(uint,ObjectDBase,function<void(ObjectDBase)>)> SendASyncMSGToObj;
};

class IObject
{
public:
    virtual void Init()=0;
    virtual void AddError()=0;//添加错误时调用，调用后表示此对象没有被成功添加
    virtual void Start(ObjectFuns funs,uint mid)=0;   //启动函数，提供QE访问权，funs为特种函数，mid为注册的ID
    virtual void End()=0;   //被强制卸载时调用
    virtual int Remove()=0;    //被正常卸载时调用，返回0代表可以卸载，返回的其他数值代表不能卸载，同时会被传递到调用者
    virtual string Name()=0;    //名字可以重复
    virtual string ClassName()=0;   //类名，可重复
    virtual string StringID()=0;    //特征码，不可重复
    virtual vector<string> MRList()=0;  //获取管理者列表，为管理者对象的SID
    virtual void Delete()=0;


    virtual void ReStart()=0;
    virtual void Stop()=0;

    virtual void ReIded(uint oid,uint nid)=0;    //已经重新分配id

    //响应函数
    virtual ObjectDBase MObjMSGResponst(DataBase data)=0;//mobj
    virtual ObjectDBase ObjMSGResponst(ObjectDBase data)=0;

    //异步响应
    virtual ObjectDBase AMObjMSGResponst(DataBase data)=0;  //mobj
    virtual ObjectDBase AObjMSGResponst(ObjectDBase data)=0;

};

#endif // IOBJECT_H
