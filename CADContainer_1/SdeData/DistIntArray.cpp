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
	if(m_pData == NULL)  //��һ��ʹ��ֱ�Ӵ���
	{
		m_pData = new int[m_nStep];
		m_nBufLength = m_nStep;
	}
	else if(m_nBufLength == m_nDataLength) //��Ҫ���ӿռ�
	{
		m_nBufLength += m_nStep;//���������С
		int* pTemp = new int[m_nBufLength]; //���¿��ٿռ�
		memcpy(pTemp,m_pData,sizeof(int)*m_nDataLength);//������ǰ������

		delete[] m_pData;  //ɾ��������
		m_pData = pTemp;
	}

	m_pData[m_nDataLength] = nValue; //�����Ԫ��
	m_nDataLength++;//���³���

}

void CDistIntArray::InsertAt(int nIndex,int nValue)
{
	if((nIndex<0) || (nIndex >= m_nDataLength)) return; //����λ�ò��Ϸ���ֱ���˳�

    if(m_nBufLength == m_nDataLength) //��Ҫ���ӿռ�
	{
		m_nBufLength += m_nStep;//���������С
		int* pTemp = new int[m_nBufLength]; //���¿��ٿռ�
		memcpy(pTemp,m_pData,sizeof(int)*m_nDataLength);//������ǰ������

		delete[] m_pData;  //ɾ��������
		m_pData = pTemp;
	}
    for(int i=m_nDataLength-1;i>=nIndex;i--) //��ָ��λ�ú�������ݺ���
		m_pData[i+1] = m_pData[i]; 
	m_pData[nIndex] = nValue;  //����������
	m_nDataLength++; //��������
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
		m_nBufLength -= m_nStep;  //�޸Ļ���������
		int* pTemp = new int[m_nBufLength]; //���¿��ٽ�С�ռ�

		memcpy(pTemp,m_pData,sizeof(int)*m_nDataLength);//������������
	
		delete [] m_pData;//ɾ��ԭ����

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



//�жϵ��Ƿ��ڵ㼯�����Ѿ�����
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
