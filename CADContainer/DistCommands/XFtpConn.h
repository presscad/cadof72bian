// XFtpConn.h: interface for the XFtpConn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XFTPCONN_H__B066BA86_2620_432F_9EC0_E3E5B4B2DF0E__INCLUDED_)
#define AFX_XFTPCONN_H__B066BA86_2620_432F_9EC0_E3E5B4B2DF0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wininet.h"

class XFtpConn  
{
public:
	TCHAR m_strServer[256]; // 服务器
	TCHAR m_strUser[256];	// 用户名
	TCHAR m_strPassword[256]; // 密码

	INTERNET_PORT m_nPort;	// 端口号
	
	HINTERNET m_hSession; // 会话
	HINTERNET m_hConnect; // 连接

public:
	XFtpConn();
	virtual ~XFtpConn();

	void SetFtpSetting(LPCTSTR lpServer, LPCTSTR lpUser, LPCTSTR lpPassword, INTERNET_PORT lpPort);
	void GetFtpSetting(LPTSTR lpServer, LPTSTR lpUser, LPTSTR lpPassword, INTERNET_PORT lpPort);

	HRESULT OpenConnect();
	HRESULT CloseConnect();

	// 设置ftp当前路径，不存在则创建
	HRESULT FtpSetCurDirectory(LPCTSTR lpPath, BOOL bCreate = FALSE);

	// 从Ftp上删除文件
	HRESULT FtpDelFile(LPCTSTR lpFile);
	// 从FTP上删除目录
	HRESULT FtpDelDirectory(LPCTSTR lpDirectory, BOOL bDelRecursive = FALSE);

	// ftp上传文件
	HRESULT FtpUpLoad(LPCTSTR lpLocalFile, LPCTSTR lpRemoteFile);
	// ftp下载文件
	HRESULT FtpDownLoad(LPCTSTR lpRemoteFile,  LPCTSTR lpLocalFile);

};

#endif // !defined(AFX_XFTPCONN_H__B066BA86_2620_432F_9EC0_E3E5B4B2DF0E__INCLUDED_)
