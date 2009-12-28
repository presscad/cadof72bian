// INISetting.cpp: implementation of the INISetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "INISetting.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  INIBUFFERLENGTH			9000 

bool GetINISetting(LPCTSTR szINIFileName, INISetting & iniset)
{
	TCHAR szConfigFile[MAX_PATH] = {0};

	DWORD  dwSize = GetModuleFileName(GetModuleHandle("shDistMenuManager.arx"), szConfigFile, MAX_PATH);

	while (dwSize > 0) 
	{
		if(szConfigFile[dwSize - 1] == '\\') 
		{
			lstrcpy(szConfigFile + dwSize, szINIFileName);
			break;
		}

		dwSize--;
	}

	return iniset.SetFilePath((LPCTSTR)szConfigFile);
}

// 注:后两个参数必须不是 const 类型的
bool INISetting::WriteIni( CString sAppName, CStringArray& asKeyName, CStringArray& asKeyValue,CString sFileName)
{
	bool bReturn = true;

	return bReturn;
}

BOOL INISetting::ReadIni(CString sFileName, CString sAppName, CStringArray& asKeyName, CStringArray& asKeyValue)
{
	BOOL bReturn = TRUE;

	CFileStatus fs;

	if (CFile::GetStatus(sFileName,fs) == FALSE)
		return false;

	if (m_pBuffer != NULL)
		delete [] m_pBuffer;

	m_pBuffer=new char[int(fs.m_size*2)];

	if (m_pBuffer == NULL)
	{
		CString sPrompt;

		sPrompt.Format("ini文件分配内存错！\n[%d]字节",int(fs.m_size*2));

		AfxMessageBox(sPrompt);
	}

	CStringArray saValue;

	if (GetPrivateProfileSection(sAppName,m_pBuffer,int(fs.m_size*2),sFileName) && Get00Strings(m_pBuffer,saValue))
	{
		for (int i=0;i<saValue.GetSize();i++)
		{
			CString sValue =  saValue[i];

			int n= sValue.ReverseFind('=');

			if(n!=-1)
			{
				asKeyName.Add(sValue.Left(n));

				asKeyValue.Add(sValue.Mid(n+1));
			}
		}
	}

	if ((asKeyName.GetSize() == 0) || (asKeyName.GetSize() != asKeyValue.GetSize()))
		bReturn = FALSE;

	return bReturn;
}

//把一个应用程序名读入到映射
bool INISetting::ReadIni(LPCTSTR szAppName, CMapIntToKeyAndValue & mapIntToKeyAndValue,LPCTSTR szFileName)
{
	static CStringArray aKey;
	static CStringArray aValue;
	aKey.RemoveAll();
	aValue.RemoveAll();
	mapIntToKeyAndValue.RemoveAll();
	if (ReadIni(szFileName, szAppName, aKey, aValue))
	{
		for (int i = 0; i < aKey.GetSize(); i++)
		{
			KeyAndValue strTmp;
			strTmp.sKey = aKey.GetAt(i);
			strTmp.sValue = aValue.GetAt(i);

			mapIntToKeyAndValue.SetAt(i, strTmp);
		}
	}
	return !mapIntToKeyAndValue.IsEmpty();
}

//所一个以"\0\0"结尾的缓存区内的字符串放入数组,返回个数
int INISetting::Get00Strings(void *pBuffer,CStringArray & aSaver)
{
	aSaver.RemoveAll();
	CString sString;
	char *pStr=(char *)pBuffer;
	while (*pStr!='\0')
	{
		sString=pStr;
		pStr+=sString.GetLength();
		aSaver.Add(sString);
		pStr++;
	}
	return aSaver.GetSize();
}

INISetting::INISetting()
{
	m_pBuffer=NULL;

}

INISetting::INISetting(LPCTSTR szINIFilePath)
{
	m_pBuffer=NULL;
	SetFilePath(szINIFilePath);
}
INISetting::~INISetting()
{
	//	DPrintf(" \n ##### 析构 INISetting 对象 %s",m_sINIFilePath);
	ClearMap();
	if (m_pBuffer != NULL)
		delete [] m_pBuffer;
};


//设INI文件名,如果返回真，表明已打开文件并且读到了内容,如果文件打开失败
bool INISetting::SetFilePath(LPCTSTR szINIFilePath)
{
	m_sINIFilePath=szINIFilePath;
	return ReadFile();
}

CString INISetting::GetFilePath()
{
	return m_sINIFilePath;
}


//取设置值，如果应用程序名为空，这里将找第一个
CString INISetting::GetSetting(LPCTSTR szKey,LPCTSTR sDefValue,LPCTSTR szAppName)
{
	CString sKey;
	CString sValue;
	CString sAppName;
	PtrStruct *pMap;
	sKey = szKey;
	sAppName=szAppName;
	sKey.MakeUpper();
	sAppName.MakeUpper();

	BOOL bFind = FALSE;
	if (m_mapStringToMap.Lookup(sAppName,(void*&)pMap))
	{
		POSITION pp;
		pp = pMap->Data.GetStartPosition();
		while (pp)
		{
			int pos;
			KeyAndValue strTmp;
			pMap->Data.GetNextAssoc(pp, pos, strTmp);
			if (strTmp.sKey.CollateNoCase(sKey) == 0)
			{
				sValue = strTmp.sValue;
				bFind = TRUE;
				break;
			}
		}
	}

	if (!bFind)
	{
		sValue = sDefValue;
	}

	return sValue;
}

//写值,如果应用程序名为空，将写第一个找到的
bool INISetting::SetSetting(LPCTSTR szKey,LPCTSTR szValue,LPCTSTR szAppName)
{
	CString sValue;
	CString sThis;
	CString sKey;
	CString sAppName;
	PtrStruct *pMap;
	CStringArray aKey;
	CStringArray aValue;
	sValue = szValue;
	sAppName = szAppName;
	sKey = szKey;
	sAppName.MakeUpper();
	//sKey.MakeUpper();
	/*
	DPrintf("\n *** 当前设的为 文件名： %s",m_sINIFilePath);
	DPrintf("\n ***              节名： %s",sAppName);
	DPrintf("\n ***              键名： %s",sKey);
	DPrintf("\n ***              键值： %s",sValue);*/


	if (m_mapStringToMap.Lookup(sAppName,(void*&)pMap))
	{
		BOOL bFind = FALSE;
		POSITION pp;
		pp = pMap->Data.GetStartPosition();
		int pos;
		while (pp)
		{
			KeyAndValue strTmp;
			pMap->Data.GetNextAssoc(pp, pos, strTmp);
			if (strTmp.sKey.CollateNoCase(sKey) == 0)
			{
				strTmp.sValue = sValue;
				pMap->Data.SetAt(pos, strTmp);
				bFind = TRUE;
				break;
			}
		}

		if (!bFind)
		{
			KeyAndValue strTmp;
			strTmp.sKey = sKey;
			strTmp.sValue = sValue;
			pos++;
			pMap->Data.SetAt(pos, strTmp);
		}
	}
	else
	{
		pMap = new PtrStruct;
		pMap->sAppName = sAppName;
		KeyAndValue strTmp;
		strTmp.sKey = sKey;
		strTmp.sValue = sValue;
		int pos = 0;
		pMap->Data.SetAt(pos, strTmp);
		m_mapStringToMap.SetAt(pMap->sAppName, pMap);
	}

	//2.改文件
	aKey.RemoveAll();
	aValue.RemoveAll();
	aKey.Add(szKey);
	aValue.Add(sValue);
	return WriteIni(szAppName,aKey,aValue,m_sINIFilePath);
}

//取所有值,键名都专为大写，返回个数
int INISetting::GetAllKeysAndValues(LPCTSTR szAppName,CStringArray & aKeys,CStringArray & aValues)
{
	POSITION ps;
	PtrStruct *pData;
	CString sAppName;
	CString sKey;
	CString sValue;

	aKeys.RemoveAll();
	aValues.RemoveAll();

	sAppName = szAppName;
	sAppName.MakeUpper();

	CStringArray sKTmp, sVTmp;
	if (m_mapStringToMap.Lookup(sAppName,(void*&)pData))
	{
		//ps = pData->Data.GetStartPosition();
		//while (ps)
		//{
		//	int pos;
		//	KeyAndValue strTmp;
		//	pData->Data.GetNextAssoc(ps, pos, strTmp);
		//	sKTmp.Add(strTmp.sKey);
		//	sVTmp.Add(strTmp.sValue);
		//}
		for (int i=0; i<pData->Data.GetCount(); i++)
		{
			sKTmp.Add(pData->Data[i].sKey);
			sVTmp.Add(pData->Data[i].sValue);
		}

	}

	for (int i = sKTmp.GetSize() - 1; i >= 0; i--)
	{
		aKeys.Add(sKTmp.GetAt(i));
		aValues.Add(sVTmp.GetAt(i));
	}

	return aKeys.GetSize();
}

//  ----------------- 私有成员函数
//清除映射
void INISetting::ClearMap()
{
	POSITION ps;
	CString sKey;
	PtrStruct *pWillDel;
	ps=m_mapStringToMap.GetStartPosition();
	while (ps)
	{
		m_mapStringToMap.GetNextAssoc(ps,sKey,(void*&)pWillDel);
		pWillDel->Data.RemoveAll();
		delete pWillDel;
	}
	m_mapStringToMap.RemoveAll();
}

//读取设置,一般不用调用，在本ARX初化时会调用
bool INISetting::ReadFile()
{
	int i;
	CString sFilePath;
	CStringArray aKey;
	CStringArray aValue;
	PtrStruct *pWillAdd;
	sFilePath=m_sINIFilePath;
	m_aAppNameArray.RemoveAll();
	//	DPrintf("\n ***当前的设置文件是 %s\n",sFilePath);

	//2003.3.6  
	//-------------->>>>何永平 加入求文件长度
	CFileStatus fs;
	if (CFile::GetStatus(sFilePath,fs) == FALSE)
		return false;
	if (m_pBuffer != NULL)
		delete [] m_pBuffer;
	m_pBuffer=new char[int(fs.m_size*2)];
	if (m_pBuffer == NULL)
	{
		CString sPrompt;
		sPrompt.Format("ini文件分配内存错！\n[%d]字节",int(fs.m_size*2));
		AfxMessageBox(sPrompt);
	}

	//清空全局变量
	//读取所有字段
	if (GetPrivateProfileSectionNames(m_pBuffer,INIBUFFERLENGTH-3,sFilePath) &&
		Get00Strings(m_pBuffer,m_aAppNameArray))
	{
		//清空过去的映射
		ClearMap();      
		//下面读取所有字段
		for (i=0;i<m_aAppNameArray.GetSize();i++)
		{
			aKey.RemoveAll();
			aValue.RemoveAll();
			pWillAdd=new PtrStruct;
			//读入字段，并把KEY 转为大写

			if (ReadIni(m_aAppNameArray[i],pWillAdd->Data,sFilePath))
			{
				CString sKey=m_aAppNameArray[i];
				sKey.MakeUpper();
				pWillAdd->sAppName=sKey;
				//加入到映射 
				m_mapStringToMap.SetAt(sKey,pWillAdd);
				//				DPrintf("\n ***读取 %s       %d个",pWillAdd->sAppName,pWillAdd->Data.GetCount());
			}
			else
			{
				delete pWillAdd;
				//				DPrintf("\n ***读取 %s 的 [%s]失败!\n",sFilePath,m_aAppNameArray[i]);
				;//这个是空句，至于用处嘛 ……
			}
		}
	}
	return !m_mapStringToMap.IsEmpty();
}

void INISetting::GetAllAppName(CStringArray & aAppNames)
{
	aAppNames.RemoveAll();
	aAppNames.Append(m_aAppNameArray);
}
