#ifndef IMOBJECT_H
#define IMOBJECT_H

#include "IObject.h"
#include "Common.h"
class ActionEntrance;
class QueryEntrance;
using std::string;
using std::vector;
struct IMObjectInfo
{
    string name;
    string sid;
    string msid;
};


///
/// \brief 提供给MObject对象的特殊函数集合
///
struct MFunctions
{
    function<void()> DeleteMe;  //强制删除自己
    function<size_t(string)> GetMSIDCount;    //获取参数提供的MSID的MObject的数量
    function<const set<IMObjectInfo *>(string)> GetInfoTableByMSID;  //由MSID获取一个MObject信息表
    function<const IMObjectInfo&(string)> GetInfoTableBySID;    //由SID获取一个MObject的信息结构体
    function<DataBase(string,DataBase)> SendSyncInfo;  //发送一个同步消息
    function<void(string,DataBase,function<void(DataBase)>)> SendASyncInfo; //发送一个异步消息，提供一个回调函数处理返回的DataBse
    function<bool(string)> TestSID; //过去参数提供的SID的MObject存不存在
    function<vector<uint>()>GetObjectsForMe;   //获取自己所管理的所有对象，包括未注册的
    function<vector<uint>()>GetNRObjectsForMe; //获取自己管理的没有注册的所有对象
    function<vector<uint>()>GetRObjectsForMe;  //获取自己管理的已经注册的所有对象

    function<ObjectDBase(uint,DataBase)>  SendSyncMsgToObj; //发送同步消息到 obj
    function<void(uint,DataBase,function<void(ObjectDBase)>)> SendASyncMsgToObj;//发送异步消息到obj
};


class IMObject
{
public:
    virtual void Init()=0;
    virtual string StringID()=0;   //特征ID，不可重复
    virtual string MRStringID()=0; //类特征ID，可重复
    virtual string Name()=0;   //名字，可重复
    /**
     * object响应函数,ADD消息时返回值作为object MIL表的值
     * objp只在Add和ReSetId和 Delete时有用，其他情况为 nullptr
     * info在任何情况下都有效，注意info必须复制再用，否则后果自负
     * oldid只在 ReSetId时有效
     * 注意 ，Add时info中的id还未真正生效，请勿使用！
     * 注意，Delete时info在 查询入口中还有残留信息，可以使用
     * Added时id已经生效，可以使用
     * 除了 Add  ReSetId和 Delete，其他情况下只有 info可用
     * */
    virtual bool Responst(RType type,const IObjectInfo *info,IObject *objp=nullptr,uint oldid=0)=0;
    virtual void Start(ActionEntrance *ae,QueryEntrance *qe,MFunctions funs)=0;   //提供 QE，AE直接访问权，funs为特种函数
    virtual void End()=0;   //被卸载时调用

    virtual void Delete()=0;    //即将卸载时调用
    virtual DataBase SyncInfoResponst(DataBase args)=0; //同步响应函数
    virtual DataBase ASyncInfoResponst(DataBase args)=0;    //异步响应函数

    virtual DataBase ObjSyncMSGResponst(ObjectDBase args)=0;    //对象同步响应函数
    virtual DataBase ObjASyncMSGResponst(ObjectDBase args)=0;   //对象异步响应函数
};

#endif // IMOBJECT_H
