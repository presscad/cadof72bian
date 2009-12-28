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
	TCHAR m_strServer[256]; // ������
	TCHAR m_strUser[256];	// �û���
	TCHAR m_strPassword[256]; // ����

	INTERNET_PORT m_nPort;	// �˿ں�
	
	HINTERNET m_hSession; // �Ự
	HINTERNET m_hConnect; // ����

public:
	XFtpConn();
	virtual ~XFtpConn();

	void SetFtpSetting(LPCTSTR lpServer, LPCTSTR lpUser, LPCTSTR lpPassword, INTERNET_PORT lpPort);
	void GetFtpSetting(LPTSTR lpServer, LPTSTR lpUser, LPTSTR lpPassword, INTERNET_PORT lpPort);

	HRESULT OpenConnect();
	HRESULT CloseConnect();

	// ����ftp��ǰ·�����������򴴽�
	HRESULT FtpSetCurDirectory(LPCTSTR lpPath, BOOL bCreate = FALSE);

	// ��Ftp��ɾ���ļ�
	HRESULT FtpDelFile(LPCTSTR lpFile);
	// ��FTP��ɾ��Ŀ¼
	HRESULT FtpDelDirectory(LPCTSTR lpDirectory, BOOL bDelRecursive = FALSE);

	// ftp�ϴ��ļ�
	HRESULT FtpUpLoad(LPCTSTR lpLocalFile, LPCTSTR lpRemoteFile);
	// ftp�����ļ�
	HRESULT FtpDownLoad(LPCTSTR lpRemoteFile,  LPCTSTR lpLocalFile);

};

#endif // !defined(AFX_XFTPCONN_H__B066BA86_2620_432F_9EC0_E3E5B4B2DF0E__INCLUDED_)
