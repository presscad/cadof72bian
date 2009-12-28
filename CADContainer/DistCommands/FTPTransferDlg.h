/*
	Module : FTPTRANSFERDLG.H
	Purpose: Defines the interface for an MFC dialog which performs FTP uploads 
	and downloads similiar to the Internet Explorer download dialog
	Created: PJN / 3-01-2000
	History: None

	Copyright (c) 2000 by PJ Naughter.	
	All rights reserved.
*/

////////////////////////////////// Macros ///////////////////////////

#ifndef __FTPTRANSFER_H__
#define __FTPTRANSFER_H__

#include "StdAfx.h"
#include "Resource.h"
#include "Wininet.h"

#pragma comment(lib, "Wininet.lib")

/////////////////////////// Classes /////////////////////////////////


class CFTPTransferDlg : public CDialog
{
public:
	//Constructors / Destructors
	CFTPTransferDlg(CWnd* pParent = NULL);
	
	//Public Member variables
	BOOL		m_bDownload;   //TRUE if it's a download, FALSE if an upload
	CString 	m_sServer;	   //e.g. "ftp.microsoft.com"
	CString 	m_sRemoteFile; //e.g. "/pub/somefile.ext"
	CString 	m_sLocalFile;  //e.g. "c:\temp\somfile.txt"
	CString 	m_sFileName;
	CString 	m_sUserName;   //Username to login to the server with
	CString 	m_sPassword;   //password to login to the server with
	INTERNET_PORT m_nPort;	   //If you want to change the port to make access on, by default it will be 21
	BOOL		m_bBinary;	   //TRUE if binary transfer, FALSE for ascii transfer
	
	//Public methods
	void AttachToExisitingHandles(HINTERNET hInternetSession, HINTERNET hFTPConnection);
	
protected:
	//{{AFX_DATA(CFTPTransferDlg)
	enum { IDD = IDD_FTPTRANSFER };
	CStatic m_ctrlStatus;
	CStatic m_ctrlTransferRate;
	CStatic m_ctrlTimeLeft;
	CProgressCtrl	m_ctrlProgress;
	CStatic m_ctrlFileStatus;
	CAnimateCtrl m_ctrlAnimate;
	//}}AFX_DATA
	
	//{{AFX_VIRTUAL(CFTPTransferDlg)
public:
	virtual void DoDataExchange(CDataExchange* pDX);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	
	//{{AFX_MSG(CFTPTransferDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LRESULT OnThreadFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAskOverwrite(WPARAM wParam, LPARAM /*lParam*/);
	
	DECLARE_MESSAGE_MAP()
		DECLARE_DYNAMIC(CFTPTransferDlg);
	
	static void CALLBACK _OnStatusCallBack(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, 
		LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	void OnStatusCallBack(HINTERNET hInternet, DWORD dwInternetStatus, 
		LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	static UINT _TransferThread(LPVOID pParam);
	void UpdateControlsDuringTransfer(DWORD dwStartTicks, DWORD& dwCurrentTicks, DWORD dwTotalBytesRead, DWORD& dwLastTotalBytes, 
		DWORD& dwLastPercentage, BOOL bGotFileSize, DWORD dwFileSize);
	void HandleThreadErrorWithLastError(UINT nIDError, DWORD dwLastError=0);
	void HandleThreadError(UINT nIDError);
	void TransferThread();
	void SetPercentage(int nPercentage);
	void SetTimeLeft(DWORD dwSecondsLeft, DWORD dwBytesRead, DWORD dwFileSize);
	void SetStatus(const CString& sCaption);
	void SetStatus(UINT nID);
	void SetStatus(UINT nID, const CString& lpsz1);
	void SetTransferRate(double KbPerSecond);
	void PlayAnimation();
	
	void SetProgressRange(short nLower, short nUpper);
	void SetProgress(int nPercentage);
	
	CString 	m_sError;
	HINTERNET	m_hInternetSession;
	HINTERNET	m_hFTPConnection;
	HINTERNET	m_hFTPFile;
	BOOL		m_bAbort;
	BOOL		m_bSafeToClose;
	CFile		m_LocalFile;
	CWinThread* m_pThread;
	BOOL		m_bUsingAttached;
};


#endif //__FTPTRANSFERDLG_H__


