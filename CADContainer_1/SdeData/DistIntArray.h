///////////////////////////////////////////////////////////////////////////////
//上海数慧系统技术有限公司拥有本代码之完全版权，未经上海数慧系统技术有限公司 //
//的明确书面许可，不得复制、取用、分发此源代码。                             //
//任何单位和个人在未获得许可情况下，不得以任何形式复制、修改和发布本软件的任 //
//何部分，否则将视为非法侵害，我公司将保留依法追究其责任的权利。在未获得许可 //
//情况下，任何组织或个人发布的产品都不能使用Dist的标志和Logo。               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
//作者 : 王晖                                                                //
//                                                                           //
// 版权所有 (C) 2007－2010 Dist Inc. 保留所有权利。                          //
///////////////////////////////////////////////////////////////////////////////

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
