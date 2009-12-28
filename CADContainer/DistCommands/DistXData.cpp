#include "stdAfx.h"
#include "distxdata.h"

CDistXData::CDistXData(void)
{
}

CDistXData::CDistXData(AcDbObjectId& ObjId,const char *strRegName)
{
	m_pEnt = NULL;
	m_bOpenAndClose = FALSE;
	memset(m_strRegName,0,sizeof(char)*1024);

	SetEntity(ObjId,strRegName);
	ReadXData();
}

CDistXData::CDistXData(AcDbEntity* pEnt,const char *strRegName)
{
	m_pEnt = NULL;
	m_bOpenAndClose = FALSE;
	memset(m_strRegName,0,sizeof(char)*1024);

	SetEntity(pEnt,strRegName);
	ReadXData();
}

CDistXData::~CDistXData(void)
{
	if(NULL != m_pEnt && m_bOpenAndClose==TRUE) 
		m_pEnt->close();
	m_ArrayFldName.RemoveAll();
	m_ArrayFldValue.RemoveAll();
}

void CDistXData::DeleteEntitySelf()
{
	if(NULL != m_pEnt && m_bOpenAndClose==TRUE) 
	{
		m_pEnt->erase();
		m_pEnt->close();m_bOpenAndClose = FALSE;
		m_pEnt = NULL;
	}
	m_ArrayFldName.RemoveAll();
	m_ArrayFldValue.RemoveAll();
}

BOOL CDistXData::DeleteAllRecord()
{
	struct resbuf *pHead = NULL;
	pHead = m_pEnt->xData(m_strRegName);
	if (NULL == pHead)
		acdbRegApp(m_strRegName);
	pHead = acutNewRb(AcDb::kDxfRegAppName);
	pHead->resval.rstring = (char*) malloc(strlen(m_strRegName) + 1);
	strcpy(pHead->resval.rstring, m_strRegName);
	pHead->rbnext = NULL;

	Acad::ErrorStatus es = m_pEnt->setXData(pHead);
	if(es != Acad::eOk)
	{
		m_pEnt->upgradeOpen();//从 Read 方式到 Write 方式
		m_pEnt->setXData(pHead);
	}

	acutRelRb(pHead);

	return TRUE;
}


void CDistXData::Close()
{
	if(NULL != m_pEnt && m_bOpenAndClose==TRUE) 
	{
		m_pEnt->close();m_bOpenAndClose = FALSE;
		m_pEnt = NULL;
	}

	m_ArrayFldName.RemoveAll();
	m_ArrayFldValue.RemoveAll();
}


void CDistXData::SetEntity(AcDbObjectId& ObjId,const char *strRegName)
{
	if(NULL != m_pEnt && m_bOpenAndClose==TRUE) 
		m_pEnt->close();

	memset(m_strRegName,0,sizeof(char)*1024);
	m_pEnt = NULL;
	if(Acad::eOk == acdbOpenAcDbEntity(m_pEnt,ObjId,AcDb::kForWrite))
		m_bOpenAndClose = TRUE;

	strcpy(m_strRegName,strRegName);

}

void CDistXData::SetEntity(AcDbEntity* pEnt,const char *strRegName)
{
	if(NULL != m_pEnt && m_bOpenAndClose==TRUE) 
		m_pEnt->close();

	memset(m_strRegName,0,sizeof(char)*1024);
	m_pEnt = pEnt;
	m_bOpenAndClose = FALSE;
	strcpy(m_strRegName,strRegName);
}

int CDistXData::GetRecordCount()
{
	return m_ArrayFldValue.GetSize();
}

void CDistXData::GetFldNameArray(CStringArray& array)
{
	array.Append(m_ArrayFldName);
}

void CDistXData::GetFldValueArray(CStringArray& array)
{
	array.Append(m_ArrayFldValue);
}

void CDistXData::ReadXData()
{
	m_ArrayFldName.RemoveAll();
	m_ArrayFldValue.RemoveAll();
	
	if(NULL == m_pEnt) return;

	int nPos = 1;
	struct resbuf *pHead = NULL;
	pHead = m_pEnt->xData(m_strRegName);
	if (pHead != NULL) 
	{
		while(NULL != pHead->rbnext)
		{
			CString str = pHead->rbnext->resval.rstring;
			if(nPos % 2 == 1)
				m_ArrayFldName.Add(str);
			else
				m_ArrayFldValue.Add(str);
			nPos++;
			pHead = pHead->rbnext;
		}
		acutRelRb(pHead);
	} 
}

void CDistXData::Update()
{
	struct  resbuf  *pHead=NULL, *pTemp=NULL;

	int nNum = m_ArrayFldName.GetSize();
	if(NULL == m_pEnt || nNum < 1) return;


	pHead = m_pEnt->xData(m_strRegName);
	if (NULL == pHead)
		acdbRegApp(m_strRegName);

	pHead = acutNewRb(AcDb::kDxfRegAppName);
	pTemp = pHead;
	pTemp->resval.rstring = (char*) malloc(strlen(m_strRegName) + 1);
	strcpy(pTemp->resval.rstring, m_strRegName);


	for(int i=0; i<nNum; i++)
	{
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdAsciiString);
		pTemp = pTemp->rbnext;
		pTemp->resval.rstring = (char*) malloc(m_ArrayFldName[i].GetLength() + 1);
		strcpy(pTemp->resval.rstring, m_ArrayFldName[i].GetBuffer(0));
		pTemp->rbnext = acutNewRb(AcDb::kDxfXdAsciiString);
		pTemp = pTemp->rbnext;
		pTemp->resval.rstring = (char*) malloc(m_ArrayFldValue[i].GetLength() + 1);
		strcpy(pTemp->resval.rstring, m_ArrayFldValue[i].GetBuffer(0));
		pTemp->rbnext = NULL;

	}

	Acad::ErrorStatus tt = m_pEnt->setXData(pHead);
	if(tt != Acad::eOk)
	{
		m_pEnt->upgradeOpen();//从 Read 方式到 Write 方式
		m_pEnt->setXData(pHead);
	}

	acutRelRb(pHead);
}

BOOL CDistXData::Add(CStringArray& strFldNameArray,CStringArray& strFldValueArray,BOOL bRemoveOld)
{
	if(strFldNameArray.GetSize() == 0 || strFldValueArray.GetSize() == 0 ||
	   strFldNameArray.GetSize() != strFldValueArray.GetSize())  return FALSE;

	if(bRemoveOld)
	{
		m_ArrayFldName.RemoveAll();
		m_ArrayFldValue.RemoveAll();
	}

	int nCount = m_ArrayFldName.GetSize();
	if(nCount == 0)
	{
		m_ArrayFldName.Append(strFldNameArray);
		m_ArrayFldValue.Append(strFldValueArray);
	}
	else
	{
		int nTempCount = strFldNameArray.GetSize();
		for(int i=0; i<nTempCount; i++)
			Add(strFldNameArray.GetAt(i),strFldValueArray.GetAt(i),TRUE);
	}
	
	return TRUE;
}

BOOL CDistXData::Add(const char *strFldName,const char *strFldValue,BOOL bIfExistsToUpdate/* =TRUE */)
{
	if(NULL == strFldValue || NULL == strFldName) return FALSE;
	int nCount = m_ArrayFldName.GetSize();

	CString strTempA,strTempB;
	for(int i=0; i<nCount;i++)
	{
		strTempA = strFldName;
		strTempA.MakeUpper();
		strTempB = m_ArrayFldName.GetAt(i);
		strTempB.MakeUpper();

		if(strTempA == strTempB)
		{
			if(bIfExistsToUpdate==TRUE)
			{
				m_ArrayFldValue.SetAt(i,strFldValue);
			}
			return TRUE; //已经存在，返TRUE
		}
	}

	m_ArrayFldName.Add(strFldName);
	m_ArrayFldValue.Add(strFldValue);

	return TRUE;
}

BOOL CDistXData::Modify(const char *strFldName,const char *strFldValue)
{
	if(NULL == strFldValue || NULL == strFldName) return FALSE;
	int nCount = m_ArrayFldName.GetSize();

	CString strTempA,strTempB;
	for(int i=0; i<nCount;i++)
	{
		strTempA = strFldName;
		strTempA.MakeUpper();
		strTempB = m_ArrayFldName.GetAt(i);
		strTempB.MakeUpper();

		if(strTempA == strTempB)
		{
			m_ArrayFldValue.SetAt(i,strFldValue);
			return TRUE; //已经存在，返TRUE
		}
	}
	return FALSE;
}

BOOL CDistXData::Delete(const char *strFldName)
{
	int nCount = m_ArrayFldName.GetSize();

	CString strTempA,strTempB;
	for(int i=0; i<nCount;i++)
	{
		strTempA = strFldName;
		strTempA.MakeUpper();
		strTempB = m_ArrayFldName.GetAt(i);
		strTempB.MakeUpper();

		if(strTempA == strTempB)
		{
			m_ArrayFldValue.RemoveAt(i);
			m_ArrayFldName.RemoveAt(i);

			return TRUE; //已经存在，返TRUE
		}
	}

	return FALSE;
}

BOOL CDistXData::Select(const char *strFldName,CString& strValue)
{
	strValue.Empty();
	int nCount = m_ArrayFldName.GetSize();

	CString strTemp;
	for(int i=0; i<nCount;i++)
	{
		strTemp = m_ArrayFldName.GetAt(i);

		if(strTemp.CompareNoCase(strFldName) == 0)
		{
			strValue = m_ArrayFldValue.GetAt(i);

			return TRUE; //已经存在，返TRUE
		}
	}

	return FALSE;
}

BOOL CDistXData::Select(int nIndex,CString& strValue)
{
	int nCount = m_ArrayFldValue.GetSize();
	if(nIndex<0 || nIndex>=nCount-1)
		return FALSE;
	else
		strValue = m_ArrayFldValue.GetAt(nIndex);

	return TRUE;
}


BOOL CDistXData::Select(CStringArray& strFldNameArray,CStringArray& strFldValueArray)
{
	strFldNameArray.RemoveAll();
	strFldValueArray.RemoveAll();

	strFldNameArray.Append(m_ArrayFldName);
	strFldValueArray.Append(m_ArrayFldValue);

	return TRUE;
}

