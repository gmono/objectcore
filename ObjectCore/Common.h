/**
  请注意，此OBJECTCORE中所有MOBJECT和OBJECT之间的消息传递都使用shared_ptr指针进行自动管理
  其他情况下，一般不使用智能指针，但不绝对
  标准状况下MObject无权获得其他MObject对象的指针，但可以通过同步和异步消息传递进行通信
  每个MOBJECT对象有义务保证两个响应函数(SyncInfoResponst,ASyncInfoResponst)的正常运行
  并处理线程随机触发时可能遇到的问题

  这东西不会自动释放mobj和obj，需要你TM自己释放，好自为之……


  MIL表结构
  键：mobject的特征码
  值：0——mobject对象返回true，1——mobject对象返回false，2——mobject对象不存在
  */


#ifndef COMMON_H
#define COMMON_H
enum RType{Add,Added,Delete,ReSetId,Start,Stop,Register};
typedef unsigned int uint;
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>
#include <memory>
#include <thread>
using namespace std;
#define umap unordered_map
#define uset unordered_set
#define mumap unordered_multimap
#define muset unordered_multiset

class ManagerEntrance;
class ActionEntrance;
struct DataBase
{
    friend class ManagerEntrance;
    friend class ActionEntrance;
private:
    shared_ptr<void> ptr;
    std::string sid;


    bool isempty = false;
public:
    DataBase(void *p)
    {
        ptr = shared_ptr<void>(p);

    }
    DataBase()
    {
        isempty=true;
    }

    bool IsEmpty(){ return isempty; }


    string Sid(){ return sid; }
    void *Ptr(){ return ptr.get(); }

    operator void*(){ return ptr.get();}
    DataBase &operator=(const DataBase &t)
    {
        ptr = t.ptr;
        sid = t.sid;
        return *this;
    }
};
struct ObjectDBase
{
    friend class ManagerEntrance;
    friend class ActionEntrance;
private:
    string sid;
    uint id;
    shared_ptr<void> ptr;
    bool isempty = false;
public:
    ObjectDBase(void *p)
    {
        ptr = shared_ptr<void>(p);
    }
    bool IsEmpty(){ return isempty; }
    ObjectDBase()
    {
        isempty=true;
    }

    string Sid(){ return sid; }
    uint Id(){ return id; }
    void *Ptr(){ return ptr.get(); }
    operator void*(){ return ptr.get(); }

    ObjectDBase &operator=(const ObjectDBase &t)
    {
        ptr = t.ptr;
        sid = t.sid;
        id = t.id;
        return *this;
    }
};



#endif // COMMON_H
