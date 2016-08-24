/**
  此文件中的函数均为全局辅助函数
  */
#ifndef EXTERN_H
#define EXTERN_H
#include "Common.h"
template<class par,class res>
bool GetOneByOne(umap<par,res> &table,par &p,res &r)
{
    auto ptr=table.find(p);
    if(ptr==table.end()) return false;
    r=ptr->second;
    return true;
}

/**
 *返回0表示一切正常，表示有错误，2表示没找到,funt为函数指针，function模板或bind产物，必须能像函数那样调用接受一个res类型参数并返回bool值
 */
template<class par,class res,class funt>
short GetMulByOne(mumap<par,res> &table,par &p,funt addfun)
{
    auto range=table.equal_range(p);
    if(range.first==range.second) return 2;
    bool noerror=true;
    for(auto i=range.first;i!=range.second;++i)
    {
        if(!addfun(i->second)) noerror=false;
    }
    if(noerror) return 0;
    else return 1;
}
/**
 *重新分配键
 */
template<class tone,class ttwo>
bool ReKey(umap<tone,ttwo> &table,tone okey,tone nkey)
{
    auto ptr=table.find(okey);
    if(ptr==table.end()) return false;
    if(table.find(nkey)!=table.end()) return true;
    auto val=ptr->second;
    table.erase(okey);
    table.insert(pair<tone,ttwo>(nkey,val));
    return true;
}
/**
 *将指定键链接的值集合中的指定数量的指定值替换成另一个值,返回成功更改的数量
 */
template<class tone,class ttwo>
uint ReValues(mumap<tone,ttwo> &table,tone key,ttwo oval,ttwo nval,uint stopnum)
{
    if(stopnum==0) return 0;
    auto range=table.equal_range(key);
    if(range.first==range.second) return 0;
    uint sum=0;
    for(auto i=range.first;i!=range.second&&sum<stopnum;++i)
    {
        ttwo &val=i->second;
        if(val==oval)
        {
            val=nval;
            sum++;
        }
    }
    return sum;

}


#endif // EXTERN_H
