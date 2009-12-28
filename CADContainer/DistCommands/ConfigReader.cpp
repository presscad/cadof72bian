#include "configreader.h"
#include <io.h>
IMPLEMENT_SERIAL(CJn_CommandPower,CObject,1)

void CJn_CommandPower::Serialize( CArchive& archive )
{
	CObject::Serialize( archive );
	// now do the stuff for our specific class
	if( archive.IsStoring() )
	{
		for (std::map<CString,BOOL>::iterator it = this->powerList.begin();
			it != this->powerList.end(); it ++)
		{
			CString strKey = it->first;
			int n = strKey.GetLength();
			archive << n;
			for (int i = 0 ; i < n ; i ++)
			{
				char c = strKey.GetAt(i);
				UINT ui = (UINT)c;
				ui <<= 2;
				archive << ui;
			}			
			archive<< it->second;
		}
	}
	else
	{			
		int n = this->powerList.size();
		for (int i = 0 ; i < n ; i ++)
		{
			CString key;
			
			BOOL state;
			int len;
			archive >> len;
			for (int j = 0; j < len; j ++)
			{
				char c;
				UINT uc;
				archive >> uc;
				uc >>= 2;
				c = (char)uc;
				key.AppendChar(c);
			}
			archive >> state;
			this->powerList[key] = state;
		}
	}
}
CConfigReader::CConfigReader(void)
{
	_pConfigObject = NULL;
}

CConfigReader::~CConfigReader(void)
{
	if (_pConfigObject != NULL)
		delete _pConfigObject;
}

void * CConfigReader::ReadConfig()
{
	return _pConfigObject;
}

void * CCmdPwrConfigReader::ReadConfig()
{
	if (_access(_configFileName,0) == -1)
	{
		CString str;
		str.Format("命令权限控制文件缺失：%s",_configFileName);
		AfxMessageBox(str);
		return NULL;
	}
	CJn_CommandPower *pCmdPower = new CJn_CommandPower();
	CFile myFile(_configFileName,CFile::modeRead);
	CArchive ar(&myFile,CArchive::load);
	pCmdPower->Serialize(ar);
	_pConfigObject = (void*) pCmdPower;
	return _pConfigObject;
}

CCmdPwrConfigReader::CCmdPwrConfigReader( void )
{
	TCHAR pathName[1024] = "\0";
	::GetModuleFileName(acrxGetApp()->GetModuleInstance(),pathName,1024);
	PathRemoveFileSpec(pathName);
	PathAppend(pathName,"cmdPower.cfg");
	_configFileName = pathName;

	ReadConfig();
}

CCmdPwrConfigReader::~CCmdPwrConfigReader( void )
{
	
}

CCmdPwrConfigReader& CCmdPwrConfigReader::Instanse()
{
	static CCmdPwrConfigReader _ins;	
	return _ins;
}

CJn_CommandPower * CCmdPwrConfigReader::GetCmdPwrConfig()
{
	return (CJn_CommandPower *)_pConfigObject;
}