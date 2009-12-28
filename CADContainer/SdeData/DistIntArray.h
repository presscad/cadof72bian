#ifndef  __CDISTINTARRAY__H__
#define  __CDISTINTARRAY__H__
#include "StdAfx.h"

class CDistIntArray  
{
public:
	//�������������
	               CDistIntArray();
	virtual       ~CDistIntArray();

public:
	//�������ܺ���
    int            GetLength();                        //ȡ����Ԫ�ظ���
	void           Add(int nValue);                    //���һ����
	int            GetAt(int nIndex);                  //ȡָ��һ����
	void           SetAt(int nIndex,int nValue);       //�޸�ָ��λ�õĵ�
	void           InsertAt(int nIndex,int nValue);    //��ָ��λ�ò���һ����
	void           RemoveAt(int nIndex);               //ɾ��ָ��λ�õ�һ����
	void           RemoveAll();                        //ɾ�����е�


    //��������
	long           IsValueInArray(int nValue);        //�ж�ָ��ֵ�Ƿ���������

	//����������
	CDistIntArray& operator = (CDistIntArray& pts);

	int            operator [] (int nIndex);

protected:
	void           RemoveSub();                      //�ﵽ�������ͷŶ���ռ�   

private:
	int*           m_pData;                          //����
	int            m_nBufLength;                     //��������С
	int            m_nDataLength;                    //����Ԫ�ظ���

	static int m_nStep;                              //���ɾ������	
};

#endif//__CDISTINTARRAY__H__
