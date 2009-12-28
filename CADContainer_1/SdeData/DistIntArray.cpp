// DistIntArray.cpp: implementation of the CDistIntArray class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DistIntArray.h"

int CDistIntArray::m_nStep = 2000;

CDistIntArray::CDistIntArray()
{
	m_pData = NULL;
	m_nBufLength  = 0;
	m_nDataLength = 0;
}

CDistIntArray::~CDistIntArray()
{
	RemoveAll();	
}

void CDistIntArray::Add(int nValue)
{
	if(m_pData == NULL)  //第一次使用直接创建
	{
		m_pData = new int[m_nStep];
		m_nBufLength = m_nStep;
	}
	else if(m_nBufLength == m_nDataLength) //需要增加空间
	{
		m_nBufLength += m_nStep;//更新数组大小
		int* pTemp = new int[m_nBufLength]; //重新开辟空间
		memcpy(pTemp,m_pData,sizeof(int)*m_nDataLength);//复制以前的数据

		delete[] m_pData;  //删除旧数组
		m_pData = pTemp;
	}

	m_pData[m_nDataLength] = nValue; //添加新元素
	m_nDataLength++;//更新长度

}

void CDistIntArray::InsertAt(int nIndex,int nValue)
{
	if((nIndex<0) || (nIndex >= m_nDataLength)) return; //插入位置不合法，直接退出

    if(m_nBufLength == m_nDataLength) //需要增加空间
	{
		m_nBufLength += m_nStep;//更新数组大小
		int* pTemp = new int[m_nBufLength]; //重新开辟空间
		memcpy(pTemp,m_pData,sizeof(int)*m_nDataLength);//复制以前的数据

		delete[] m_pData;  //删除旧数组
		m_pData = pTemp;
	}
    for(int i=m_nDataLength-1;i>=nIndex;i--) //将指定位置后面的数据后移
		m_pData[i+1] = m_pData[i]; 
	m_pData[nIndex] = nValue;  //插入新数据
	m_nDataLength++; //长度增加
}


int CDistIntArray::GetLength()
{
	return m_nDataLength;
}

int CDistIntArray::GetAt(int nIndex)
{
	if((nIndex<0) || (nIndex >= m_nDataLength)) 
		return -1;
	else
		return m_pData[nIndex];
	
}

void CDistIntArray::SetAt(int nIndex,int nValue)
{
	if((nIndex<0) || (nIndex >= m_nDataLength)) return;
	m_pData[nIndex] = nValue;
}


void CDistIntArray::RemoveAt(int nIndex)
{
	if((nIndex<0) || (nIndex >= m_nDataLength)) return;

    for(int i= nIndex;i<m_nDataLength-1;i++)
		m_pData[i] = m_pData[i+1];

	m_nDataLength--;
	RemoveSub();
}


void CDistIntArray::RemoveAll()
{
	if(NULL != m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
	m_nBufLength  = 0;
	m_nDataLength = 0;
}

void CDistIntArray::RemoveSub()
{
	if(m_nBufLength >= m_nDataLength + m_nStep)
	{
		m_nBufLength -= m_nStep;  //修改缓冲区长度
		int* pTemp = new int[m_nBufLength]; //重新开辟较小空间

		memcpy(pTemp,m_pData,sizeof(int)*m_nDataLength);//复制数组数据
	
		delete [] m_pData;//删除原数组

		m_pData = pTemp;
	}
}

CDistIntArray& CDistIntArray::operator = (CDistIntArray& pts)
{
	RemoveAll();
	int nCount = pts.GetLength();
	for(int i=0; i<nCount; i++)
		Add(pts.GetAt(i));

	return *this;
}



int CDistIntArray::operator[] (int nIndex)
{
	if(nIndex < 0 || nIndex+1 > GetLength()) 
		return -1;
	else
		return m_pData[nIndex];
}



//判断点是否在点集合中已经存在
long CDistIntArray::IsValueInArray(int nValue)
{
	int bResult = 0;
	for(int i=0;i<m_nDataLength;i++)
	{
		if(nValue == GetAt(i))
		{
			bResult = 1;
			break;
		}
	}

	return bResult;
}
