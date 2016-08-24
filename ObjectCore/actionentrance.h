#ifndef ACTIONENTRANCE_H
#define ACTIONENTRANCE_H
#include "Common.h"
#include "IObject.h"
using std::vector;
using std::string;
class ObjectCore;

class ActionEntrance
{
    friend class QueryEntrance;
    friend class ManagerEntrance;
    friend class ObjectCore;
public:
    

    bool AddObject(IObject *obj,uint &result);
    bool AddObjectForID(IObject *obj, uint nid); //用指定id添加一个对象，如果已经存在返回false,其他情况和普通添加一样
    int RemoveObject(uint id);
    void DeleteObject(uint id);

    bool ReObjId(uint id,uint nid); //将一个id指向的对象替换一个新id，返回成功与否


    IObject *GetObject(uint id);
    bool GetObjectId(IObject *, uint &result);

private:
    ActionEntrance(ObjectCore *oc);
    
    umap<uint,IObject *> itot;
    umap<IObject *,uint> otit;

    vector<IObject *> GetNRObjectsByMObj(string sid);
    vector<IObject *> GetRObjectsByMObj(string sid);

    //以下对象不可释放
    ObjectCore *oc;

    //以下为辅助函数
    uint GetRandId();
};

#endif // ACTIONENTRANCE_H
