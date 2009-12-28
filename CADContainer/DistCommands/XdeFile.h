// XdeFile.h: interface for the CXdeFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XDEFILE_H__CFE00949_FD5F_4F16_8299_9CE9F45488B3__INCLUDED_)
#define AFX_XDEFILE_H__CFE00949_FD5F_4F16_8299_9CE9F45488B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct ST_SDE_DATA
{
	CString strServer;
	CString strService;
	CString strDatabase;
	CString strUserName;
	CString strPassword;

	ST_SDE_DATA()
	{
		strServer = "";
		strService = "";
		strDatabase = "";
		strUserName = "";
		strPassword = "";
	}
};

class CXdeFile  
{
public:
	CXdeFile();
	virtual ~CXdeFile();

private:
	ST_SDE_DATA m_xxSdeData;

public:
	void Serialize(CArchive& ar);

	BOOL ReadXdeFile(HANDLE  hFile);
	BOOL WriteXdeFile(HANDLE  hFile);

	ST_SDE_DATA GetSdeData() { return m_xxSdeData; }
	void SetSdeData(ST_SDE_DATA& xxSdeData) 
	{
		m_xxSdeData = xxSdeData; 
	}
};

#endif // !defined(AFX_XDEFILE_H__CFE00949_FD5F_4F16_8299_9CE9F45488B3__INCLUDED_)
