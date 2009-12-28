// XFtpConn.cpp: implementation of the XFtpConn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XFtpConn.h"

#include <stdlib.h>
#include <shlwapi.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XFtpConn::XFtpConn()
{
	strcpy(m_strServer, "127.0.0.1");
	strcpy(m_strUser, "anonymous");
	strcpy(m_strPassword, "");

	m_nPort = 21;	// 端口号

	m_hConnect = NULL;
	m_hSession = NULL;
}

XFtpConn::~XFtpConn()
{
	CloseConnect();
}

// 打开ftp连接
HRESULT XFtpConn::OpenConnect()
{
	m_hSession = InternetOpen("Ftp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	
	if (NULL == m_hSession)
		return GetLastError();

	m_hConnect = InternetConnect(m_hSession,
		m_strServer,
		m_nPort,
		m_strUser,
		m_strPassword,
		INTERNET_SERVICE_FTP,
		INTERNET_FLAG_PASSIVE,
		0);
	
	if (NULL == m_hConnect)
	{
		InternetCloseHandle(m_hSession);
		m_hSession = NULL;
		return GetLastError();
	}

	return S_OK;
}

// 关闭ftp连接
HRESULT XFtpConn::CloseConnect()
{
	InternetCloseHandle(m_hConnect);
	InternetCloseHandle(m_hSession);

	m_hConnect = NULL;
	m_hSession = NULL;

	return S_OK;
}

void XFtpConn::SetFtpSetting(LPCTSTR lpServer, LPCTSTR lpUser, LPCTSTR lpPassword, INTERNET_PORT nPort)
{
	strcpy(m_strServer, lpServer);
	strcpy(m_strUser, lpUser);
	strcpy(m_strPassword, lpPassword);

	m_nPort = nPort;
}

void XFtpConn::GetFtpSetting(LPTSTR lpServer, LPTSTR lpUser, LPTSTR lpPassword, INTERNET_PORT nPort)
{
	strcpy(lpServer, m_strServer);
	strcpy(lpUser, m_strUser);
	strcpy(lpPassword, m_strPassword);

	nPort = m_nPort;
}

// 设置ftp当前路径，不存在则创建
HRESULT XFtpConn::FtpSetCurDirectory(LPCTSTR lpPath, BOOL bCreate /* = FALSE*/)
{
	// 打开ftp连接
	if ((NULL == m_hSession) || (NULL == m_hConnect))
	{
		HRESULT hr = OpenConnect();
		if (hr != S_OK)	return hr;
	}

	// 直接调用API设置当前路径
	if (::FtpSetCurrentDirectory(m_hConnect, lpPath))
		return S_OK;

	// 设置根目录
	if (!::FtpSetCurrentDirectory(m_hConnect, "\\"))
		return GetLastError();

	if (NULL == lpPath)
		return S_OK;

	if (!bCreate)
	{
		if (!::FtpSetCurrentDirectory(m_hConnect, lpPath))
			return GetLastError();
	}
	else
	{
		TCHAR szPath[MAX_PATH] = {0};
		strcpy(szPath, lpPath);
		
		HINTERNET hFind = NULL;
		WIN32_FIND_DATA xxDirInfo;
		ZeroMemory(&xxDirInfo, sizeof(WIN32_FIND_DATA));
		BOOL bWorking = FALSE;
		
		int iPos = 0;
		// 去掉Path前面的反斜杠
		while (szPath[iPos] == '\\')
			iPos++;
		
		TCHAR *p = szPath+iPos;
		
		do{
			int nCount = 0;
			while ((*p != '\\') && (*p != '\0'))
			{
				p++;
				nCount++;
			}
			*p = '\0';	// 分离出目录名称
			
			// 目录不存在
			if (!::FtpSetCurrentDirectory(m_hConnect, szPath+iPos))
			{
				// 创建该目录
				if (!::FtpCreateDirectory(m_hConnect, szPath+iPos))
					return GetLastError();	

				// 设置当前目录
				if (!::FtpSetCurrentDirectory(m_hConnect, szPath+iPos))
					return GetLastError();
			}
			
			
			// 取下一级目录名称
			p++;
			iPos += nCount+1;
		}while (*p != '\0');
	}

	return S_OK;
}

// 从Ftp上删除文件
HRESULT XFtpConn::FtpDelFile(LPCTSTR lpFile)
{
	// 打开ftp连接
	if ((NULL == m_hSession) || (NULL == m_hConnect))
	{
		HRESULT hr = OpenConnect();
		if (hr != S_OK)	return hr;
	}

	if (!::FtpSetCurrentDirectory(m_hConnect, "\\"))
		return GetLastError();

// 	WIN32_FIND_DATA wFindData;
// 	HINTERNET hFind = FtpFindFirstFile(m_hConnect, lpFile, &wFindData, 0, 0);
// 
// 	if (NULL == hFind)
// 		return GetLastError();

	if (!::FtpDeleteFile(m_hConnect, lpFile))
		return GetLastError();

	return S_OK;
}

// 从FTP上删除目录
HRESULT XFtpConn::FtpDelDirectory(LPCTSTR lpDirectory, BOOL bDelRecursive /*= TRUE*/)
{
	// 打开ftp连接
	if ((NULL == m_hSession) || (NULL == m_hConnect))
	{
		HRESULT hr = OpenConnect();
		if (hr != S_OK)	return hr;
	}

	if (!bDelRecursive)
	{
		if (!FtpRemoveDirectory(m_hConnect, lpDirectory))
			return GetLastError();
	}
	else // 递归删除
	{
		// 保存原来的当前路径
		TCHAR szPreDir[MAX_PATH];
		DWORD dwSize = sizeof(szPreDir);
		
		if (!::FtpGetCurrentDirectory(m_hConnect, szPreDir, &dwSize))
			return GetLastError();
		
		if (!::FtpSetCurrentDirectory(m_hConnect, lpDirectory))
			return GetLastError();
		
		TCHAR lpFile[MAX_PATH] = {0};
		strcpy(lpFile, _T("*.*"));
//		strcpy(lpFile, lpDirectory);
//		PathAppend(lpFile, _T("*.*"));
		
		// 遍历子目录
		WIN32_FIND_DATA wFindData;
		HINTERNET hFind = ::FtpFindFirstFile(m_hConnect, lpFile, &wFindData, 0, 0);
		
		if (NULL == hFind)
		{
			// 空目录
			if (GetLastError() == ERROR_NO_MORE_FILES)
			{
				if (!::FtpSetCurrentDirectory(m_hConnect, szPreDir))
					return GetLastError();
				
				if (!::FtpRemoveDirectory(m_hConnect, lpDirectory))
					return GetLastError();
			}
		}
		else
		{
			do {
				//如果是"."或者".."
				if (wFindData.cFileName[0] == TEXT('.')&&
					((wFindData.cFileName[1]==TEXT('\0'))||
					((wFindData.cFileName[1]==TEXT('.'))&&(wFindData.cFileName[2]==TEXT('\0')))
					))
					continue;

				// 如果是目录
				if (wFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					HRESULT hr = FtpDelDirectory(wFindData.cFileName, bDelRecursive);
					if (hr != S_OK) return hr;
				}
				else // 如果是文件
				{
					if (!FtpDeleteFile(m_hConnect, wFindData.cFileName))
						return GetLastError();
				}
			} while(::InternetFindNextFile(hFind, &wFindData));

			if (!::FtpRemoveDirectory(m_hConnect, lpDirectory))
				return GetLastError();
		}
	}

	return S_OK;
}

// ftp上传文件
HRESULT XFtpConn::FtpUpLoad(LPCTSTR lpLocalFile, LPCTSTR lpRemoteFile)
{
	// 打开ftp连接
	if ((NULL == m_hSession) || (NULL == m_hConnect))
	{
		HRESULT hr = OpenConnect();
		if (hr != S_OK)	return hr;
	}

	TCHAR szRemotePath[MAX_PATH] = {0};
	strcpy(szRemotePath, lpRemoteFile);
	
	// 从lpRemoteFile中分离出目录名称
	PathRemoveFileSpec(szRemotePath);
	
	// 确保FTP上有相关的目录，不存在则创建
	if (FtpSetCurDirectory(szRemotePath, TRUE) != S_OK)
	{
		return S_FALSE;
	}
	
	// 通过FTP传送文件
	if (!FtpPutFile(m_hConnect, lpLocalFile, lpRemoteFile,
		FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 0))
	{
		return S_FALSE;
	} 

	return S_OK;
}

// ftp下载文件
HRESULT XFtpConn::FtpDownLoad(LPCTSTR lpRemoteFile,  LPCTSTR lpLocalFile)
{
	// 打开ftp连接
	if ((NULL == m_hSession) || (NULL == m_hConnect))
	{
		HRESULT hr = OpenConnect();
		if (hr != S_OK)	return hr;
	}

	if (!FtpGetFile(m_hConnect, lpRemoteFile, lpLocalFile, FALSE, 
		FILE_ATTRIBUTE_NORMAL ,FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_NEED_FILE, 0))
	{
		return S_FALSE;
	}

	return S_OK;
}
