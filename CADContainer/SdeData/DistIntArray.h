#ifndef  __CDISTINTARRAY__H__
#define  __CDISTINTARRAY__H__
#include "StdAfx.h"

class CDistIntArray  
{
public:
	//构造和析构函数
	               CDistIntArray();
	virtual       ~CDistIntArray();

public:
	//操作功能函数
    int            GetLength();                        //取数组元素个数
	void           Add(int nValue);                    //添加一个点
	int            GetAt(int nIndex);                  //取指定一个点
	void           SetAt(int nIndex,int nValue);       //修改指定位置的点
	void           InsertAt(int nIndex,int nValue);    //在指定位置插入一个点
	void           RemoveAt(int nIndex);               //删除指定位置的一个点
	void           RemoveAll();                        //删除所有点


    //辅助功能
	long           IsValueInArray(int nValue);        //判断指定值是否在数组中

	//操作符重载
	CDistIntArray& operator = (CDistIntArray& pts);

	int            operator [] (int nIndex);

protected:
	void           RemoveSub();                      //达到步长，释放多余空间   

private:
	int*           m_pData;                          //数组
	int            m_nBufLength;                     //缓冲区大小
	int            m_nDataLength;                    //数组元素个数

	static int m_nStep;                              //添加删除步长	
};

#endif//__CDISTINTARRAY__H__
