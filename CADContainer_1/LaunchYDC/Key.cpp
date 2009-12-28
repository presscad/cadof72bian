// key.cpp : implementation file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "key.h"
#include <winreg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKey

void CKey::Close()
{
	if (m_hKey != NULL)
	{
		LONG lRes = RegCloseKey(m_hKey);
		ASSERT(lRes == ERROR_SUCCESS);
		m_hKey = NULL;
	}
}

BOOL CKey::Create(HKEY hKey, LPCTSTR lpszKeyName)
{
	ASSERT(hKey != NULL);
	return (RegCreateKey(hKey, lpszKeyName, &m_hKey) == ERROR_SUCCESS);
}

BOOL CKey::Open(HKEY hKey, LPCTSTR lpszKeyName)
{
	ASSERT(hKey != NULL);
	return (RegOpenKey(hKey, lpszKeyName, &m_hKey) == ERROR_SUCCESS);
}

BOOL CKey::SetStringValue(LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
	ASSERT(m_hKey != NULL);
	return (RegSetValueEx(m_hKey, lpszValueName, NULL, REG_SZ,
		(BYTE * const)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR)) == ERROR_SUCCESS);
}

BOOL CKey::GetStringValue(CString& str, LPCTSTR lpszValueName)
{
	ASSERT(m_hKey != NULL);
	str.Empty();
	DWORD dw = 0;
	DWORD dwType = 0;
	LONG lRes = RegQueryValueEx(m_hKey, (LPTSTR)lpszValueName, NULL, &dwType,
		NULL, &dw);
	if (lRes == ERROR_SUCCESS)
	{
		ASSERT(dwType == REG_SZ);
		LPTSTR lpsz = str.GetBufferSetLength(dw);
		lRes = RegQueryValueEx(m_hKey, (LPTSTR)lpszValueName, NULL, &dwType, (BYTE*)lpsz, &dw);
		ASSERT(lRes == ERROR_SUCCESS);
		str.ReleaseBuffer();
		return TRUE;
	}
	return FALSE;
}

BOOL CKey::GetRegEnumKey(CString &str,int index)
{
	ASSERT(m_hKey != NULL);
	//strArr.RemoveAll();
	DWORD dwSize;
	char lpValueBuf[40];
	dwSize= sizeof(lpValueBuf);

    FILETIME lasttime;
    LONG lRes; 
//	LONG lRes= RegEnumKeyEx(m_hKey,0,lpValueBuf,dwSize,NULL,NULL,NULL,lasttime);
	lRes= RegEnumKeyEx(m_hKey, //hKey 
			index,                // Index of subkey   
			&lpValueBuf[0],    // Buffer to hold key name
			&dwSize,  // Length of buffer
			NULL,                 // Reserved 
			NULL,                 // Class buffer  
			NULL,                 // Class buffer length 
			&lasttime              // Last write time  
			);          
		if (lRes == ERROR_SUCCESS)
		{
			str=(CString)lpValueBuf; 
		}
		return lRes == ERROR_SUCCESS;
}

BOOL CKey::GetRegEnumValue(CString &str, int index)
{
	ASSERT(m_hKey != NULL);
	//strArr.RemoveAll();
	DWORD dwSize;
	char lpValueBuf[256];
	dwSize= sizeof(lpValueBuf);

	LONG lRes; 
	//	LONG lRes= RegEnumKeyEx(m_hKey,0,lpValueBuf,dwSize,NULL,NULL,NULL,lasttime);
	lRes= RegEnumValue(
		m_hKey,				// handle to key to query
		index,				// index of value to query
		&lpValueBuf[0],		// value buffer
		&dwSize,			// size of value buffer
		NULL,				// reserved
		NULL,				// type buffer
		NULL,				// data buffer
		NULL				// size of data buffer
		);          
	if (lRes == ERROR_SUCCESS)
	{
		str=(CString)lpValueBuf; 
	}
	return lRes == ERROR_SUCCESS;
}
