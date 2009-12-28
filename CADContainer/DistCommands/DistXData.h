#ifndef  __DISTXDATA__H__
#define  __DISTXDATA__H__
//��˵��  ����չ���Բ�����
//������ʽ��XDATA���� = �ֶ�����1->�ֶ�ֵ1->�ֶ�����2->�ֶ�ֵ2->...->�ֶ�����n->�ֶ�ֵn->NULL

#include "StdAfx.h"

class CDistXData
{
private:
	         CDistXData(void);
public:
			 CDistXData(AcDbEntity* pEnt,const char *strRegName="DIST_XDATA");
			 CDistXData(AcDbObjectId& ObjId,const char *strRegName="DIST_XDATA");
	virtual ~CDistXData(void);
	
public:
	void SetEntity(AcDbEntity* pEnt,const char *strRegName="DIST_XDATA");
	void SetEntity(AcDbObjectId& ObjId,const char *strRegName="DIST_XDATA");
    void Close();
	int  GetRecordCount();

	void GetFldNameArray(CStringArray& array);
	void GetFldValueArray(CStringArray& array);

public:
	BOOL Add(const char *strFldName,const char *strFldValue,BOOL bIfExistsToUpdate=TRUE);
	BOOL Add(CStringArray& strFldNameArray,CStringArray& strFldValueArray,BOOL bRemoveOld=TRUE);
    BOOL Modify(const char *strFldName,const char *strFldValue);
	BOOL Delete(const char *strFldName);
	BOOL Select(const char *strFldName,CString& strValue);
	BOOL Select(int nIndex,CString& strValue);
	BOOL Select(CStringArray& strFldNameArray,CStringArray& strFldValueArray);
	BOOL DeleteAllRecord();

	void Update();

	void DeleteEntitySelf();

private:
	void ReadXData();
private:
	AcDbEntity*   m_pEnt;
	char          m_strRegName[1024];
	CStringArray  m_ArrayFldName;
	CStringArray  m_ArrayFldValue;
	BOOL          m_bOpenAndClose;
};



#endif  //__DISTXDATA__H__