#pragma once

#include "StdAfx.h"
#include "AfxTempl.h"

class CAttrDataClass
{
public:
	CAttrDataClass(void);
	~CAttrDataClass(void);

	long m_fid;					//������SDE���ݿ��е�Id
								//����SDE���ݿ��еĶ��󣬴�ֵΪ-1
	long m_lCadId;
	CString m_strCADLayer;		//����ǰ����CADͼ������
	CString m_strOldCADLayer;	//����ԭ������CADͼ������
	CString m_strSDELayer;		//����ǰ����SDEͼ������
	CString m_strOldSDELayer;	//����ԭ������SDEͼ������
	long m_lShapeEditNum;		//ͼ���޸ļ�����
	long m_lLayerMoveNum;		//ͼ�����������
	CStringArray aAttr;			//����ǰSDE��������
								//ÿ�������� ��������:����ֵ �ĸ�ʽ�洢
	CStringArray aOldAttr;		//����ԭʼSDE��������

	CAttrDataClass &operator = (const CAttrDataClass &AttrDataClass);

};

typedef CMap<long,long,CAttrDataClass,CAttrDataClass&> CMapCADIdToAttrData;