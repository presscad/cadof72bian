/*
	Module : FTPTRANSFERDLG.CPP
	Purpose: Defines the implementation for an MFC dialog which performs FTP uploads 
	and downloads similiar to the Internet Explorer download dialog
	Created: PJN / 3-01-2000
	History: None

	Copyright (c) 2000 by PJ Naughter.	
	All rights reserved.
*/


/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "FTPTransferDlg.h"

///////////////////////////////// Defines /////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT WM_FTPTRANSFER_THREAD_FINISHED = WM_APP + 1;
const UINT WM_FTPTRANSFER_ASK_OVERWRITE_FILE = WM_APP + 2;

///////////////////////////////// Implementation //////////////////////////////
IMPLEMENT_DYNAMIC(CFTPTransferDlg, CDialog);

CFTPTransferDlg::CFTPTransferDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFTPTransferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFTPTransferDlg)
	//}}AFX_DATA_INIT
	m_hInternetSession = NULL;
	m_hFTPConnection = NULL;
	m_hFTPFile = NULL;
	m_bAbort = FALSE;
	m_bSafeToClose = FALSE;
	m_pThread = NULL;
	m_bUsingAttached = FALSE;
	m_bDownload = TRUE;
	m_nPort = INTERNET_DEFAULT_FTP_PORT;
	m_bBinary = TRUE;
}

void CFTPTransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFTPTransferDlg)
	DDX_Control(pDX, IDC_STATUS, m_ctrlStatus);
	DDX_Control(pDX, IDC_TRANSFER_RATE, m_ctrlTransferRate);
	DDX_Control(pDX, IDC_TIMELEFT, m_ctrlTimeLeft);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
	DDX_Control(pDX, IDC_FILESTATUS, m_ctrlFileStatus);
	DDX_Control(pDX, IDC_ANIMATE1, m_ctrlAnimate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFTPTransferDlg, CDialog)
	//{{AFX_MSG_MAP(CFTPTransferDlg)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FTPTRANSFER_THREAD_FINISHED, OnThreadFinished)
	ON_MESSAGE(WM_FTPTRANSFER_ASK_OVERWRITE_FILE, OnAskOverwrite)
END_MESSAGE_MAP()

void CFTPTransferDlg::AttachToExisitingHandles(HINTERNET hInternetSession, HINTERNET hFTPConnection)
{
	ASSERT(m_hInternetSession == NULL);
	ASSERT(m_hFTPConnection == NULL);
	m_hInternetSession = hInternetSession;
	m_hFTPConnection = hFTPConnection;
	m_bUsingAttached = TRUE;
}

LRESULT CFTPTransferDlg::OnAskOverwrite(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return 1L;
/*
	CString sMsg;
	AfxFormatString1(sMsg, IDS_TRANSFER_OK_TO_OVERWRITE, m_sRemoteFile);
	return (AfxMessageBox(sMsg, MB_YESNO) == IDYES);
*/
}

LRESULT CFTPTransferDlg::OnThreadFinished(WPARAM wParam, LPARAM /*lParam*/)
{
	//It's now safe to close since the thread has signaled us
	m_bSafeToClose = TRUE;
	
	//Stop the animation
	m_ctrlAnimate.Stop();
	
	//If an error occurred display the message box
	if (m_bAbort)
		EndDialog(IDCANCEL);
	else if (wParam)
	{
		if (m_sError.GetLength())  
			AfxMessageBox(m_sError);
		EndDialog(IDCANCEL);
	}
	else
		EndDialog(IDOK);
	
	return 0L;
}

BOOL CFTPTransferDlg::OnInitDialog() 
{
	//Let the parent class do its thing
	CDialog::OnInitDialog();
	
	//Change the caption of the dialog if necessary
	if (!m_bDownload)
	{
		CString sCaption;
		sCaption.LoadString(IDS_FTPTRANSFER_UPLOAD_CAPTION);
		SetWindowText(sCaption);
	}
	
	//Setup the animation control
	m_ctrlAnimate.Open(m_bDownload ? IDR_FTPDOWNLOAD_ANIMATION : IDR_FTPUPLOAD_ANIMATION);
	
	//Check to see if the file we are downloading exists and if
	//it does, then ask the user if they were it overwritten
	if (m_bDownload)
	{
		CFileStatus fs;
		ASSERT(m_sLocalFile.GetLength());
		if (CFile::GetStatus(m_sLocalFile, fs))
		{
			CString sMsg;
			AfxFormatString1(sMsg, IDS_FTPTRANSFER_OK_TO_OVERWRITE, m_sLocalFile);
			if (AfxMessageBox(sMsg, MB_YESNO) != IDYES)
			{
				TRACE(_T("Failed to confirm file overwrite, download aborted\n"));
				EndDialog(IDCANCEL);
				return TRUE;
			}
		}
	}
	
	//Try and open the file we will downloading into / uploading from
	if (m_bDownload)
	{
		if (!m_LocalFile.Open(m_sLocalFile, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		{
			TRACE(_T("Failed to open the file to download into, Error:%d\n"), GetLastError());
			CString sError;
			sError.Format(_T("%d"), ::GetLastError());
			CString sMsg;
			AfxFormatString1(sMsg, IDS_FTPTRANSFER_FAIL_LOCALFILE_OPEN1, sError);
			AfxMessageBox(sMsg);
			EndDialog(IDCANCEL);
			return TRUE;
		}
	}
	else
	{
		if (!m_LocalFile.Open(m_sLocalFile, CFile::modeRead | CFile::shareDenyWrite))
		{
			TRACE(_T("Failed to open the file to upload, Error:%d\n"), GetLastError());
			CString sError;
			sError.Format(_T("%d"), ::GetLastError());
			CString sMsg;
			AfxFormatString1(sMsg, IDS_FTPTRANSFER_FAIL_LOCALFILE_OPEN2, sError);
			AfxMessageBox(sMsg);
			EndDialog(IDCANCEL);
			return TRUE;
		}
	}
	
	//Set the file status text
	CString sFileStatus;
	ASSERT(m_sRemoteFile.GetLength());
	ASSERT(m_sServer.GetLength());
/*
	if (m_bDownload)
	AfxFormatString2(sFileStatus, IDS_FTPTRANSFER_FILESTATUS1, m_sRemoteFile, m_sServer);
	else
	AfxFormatString2(sFileStatus, IDS_FTPTRANSFER_FILESTATUS2, m_sRemoteFile, m_sServer);
*/
	sFileStatus = m_sFileName;
	
	m_ctrlFileStatus.SetWindowText(sFileStatus);
	
	//Spin off the background thread which will do the actual file transfer
	m_pThread = AfxBeginThread(_TransferThread, this, THREAD_PRIORITY_NORMAL, CREATE_SUSPENDED);
	if (m_pThread == NULL)
	{
		TRACE(_T("Failed to create transfer thread, dialog is aborting\n"));
		EndDialog(IDCANCEL);
		return TRUE;
	}
	m_pThread->m_bAutoDelete = FALSE;
	m_pThread->ResumeThread();
	
	return TRUE;
}

UINT CFTPTransferDlg::_TransferThread(LPVOID pParam)
{
	//Convert from the SDK world to the C++ world
	CFTPTransferDlg* pDlg = (CFTPTransferDlg*) pParam;
	ASSERT(pDlg);
	ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CFTPTransferDlg)));
	pDlg->TransferThread();
	return 0;
}

void CFTPTransferDlg::SetPercentage(int nPercentage)
{
	//Change the progress control
	m_ctrlProgress.SetPos(nPercentage);
	
	//Change the caption text
	CString sPercentage;
	sPercentage.Format(_T("%d"), nPercentage);
	CString sCaption;
	AfxFormatString1(sCaption, IDS_FTPTRANSFER_PERCENTAGE, sPercentage);
//	AfxFormatString2(sCaption, IDS_FTPTRANSFER_PERCENTAGE, sPercentage, m_sRemoteFile);
	SetWindowText(sCaption);
}

void CFTPTransferDlg::SetProgressRange(short nLower, short nUpper)
{
	m_ctrlProgress.SetRange(0, 100);
}

void CFTPTransferDlg::SetProgress(int nPercentage)
{
	int nLower, nUpper;
	m_ctrlProgress.GetRange(nLower, nUpper);
	m_ctrlProgress.SetPos(nPercentage*nUpper/100);
}

void CFTPTransferDlg::SetTimeLeft(DWORD dwSecondsLeft, DWORD dwBytesRead, DWORD dwFileSize)
{
	CString sCopied;
	if (dwBytesRead < 1024)
	{
		CString sBytes;
		sBytes.Format(_T("%d"), dwBytesRead);
		AfxFormatString1(sCopied, IDS_FTPTRANSFER_BYTES, sBytes);
	}
	else if (dwBytesRead < 1048576)
	{
		CString sKiloBytes;
		sKiloBytes.Format(_T("%0.1f"), dwBytesRead/1024.0);
		AfxFormatString1(sCopied, IDS_FTPTRANSFER_KILOBYTES, sKiloBytes);
	}
	else
	{
		CString sMegaBytes;
		sMegaBytes.Format(_T("%0.2f"), dwBytesRead/1048576.0);
		AfxFormatString1(sCopied, IDS_FTPTRANSFER_MEGABYTES, sMegaBytes);
	}
	
	CString sTotal;
	if (dwFileSize < 1024)
	{
		CString sBytes;
		sBytes.Format(_T("%d"), dwFileSize);
		AfxFormatString1(sTotal, IDS_FTPTRANSFER_BYTES, sBytes);
	}
	else if (dwFileSize < 1048576)
	{
		CString sKiloBytes;
		sKiloBytes.Format(_T("%0.1f"), dwFileSize/1024.0);
		AfxFormatString1(sTotal, IDS_FTPTRANSFER_KILOBYTES, sKiloBytes);
	}
	else
	{
		CString sMegaBytes;
		sMegaBytes.Format(_T("%0.2f"), dwFileSize/1048576.0);
		AfxFormatString1(sTotal, IDS_FTPTRANSFER_MEGABYTES, sMegaBytes);
	}
	
	CString sOf;
	AfxFormatString2(sOf, IDS_FTPTRANSFER_OF, sCopied, sTotal);
	
	CString sTime;
	if (dwSecondsLeft < 60)
	{
		CString sSeconds;
		sSeconds.Format(_T("%d"), dwSecondsLeft);
		AfxFormatString1(sTime, IDS_FTPTRANSFER_SECONDS, sSeconds);
	}
	else
	{
		DWORD dwMinutes = dwSecondsLeft / 60;
		DWORD dwSeconds = dwSecondsLeft % 60;
		CString sSeconds;
		sSeconds.Format(_T("%d"), dwSeconds);
		CString sMinutes;
		sMinutes.Format(_T("%d"), dwMinutes);
		if (dwSeconds == 0)
			AfxFormatString1(sTime, IDS_FTPTRANSFER_MINUTES, sMinutes);
		else
			AfxFormatString2(sTime, IDS_FTPTRANSFER_MINUTES_AND_SECONDS, sMinutes, sSeconds);
	}
	
	CString sTimeLeft;
	AfxFormatString2(sTimeLeft, IDS_FTPTRANSFER_TIMELEFT, sTime, sOf);
	m_ctrlTimeLeft.SetWindowText(sTimeLeft);
}

void CFTPTransferDlg::SetStatus(const CString& sCaption)
{
	m_ctrlStatus.SetWindowText(sCaption);
}

void CFTPTransferDlg::SetStatus(UINT nID)
{
	CString sCaption;
	sCaption.LoadString(nID);
	SetStatus(sCaption);
}

void CFTPTransferDlg::SetStatus(UINT nID, const CString& lpsz1)
{
	CString sStatus;
	AfxFormatString1(sStatus, nID, lpsz1);
	SetStatus(sStatus);
}

void CFTPTransferDlg::SetTransferRate(double KbPerSecond)
{
	CString sRate;
	if (KbPerSecond < 1)
	{
		CString sBytesPerSecond;
		sBytesPerSecond.Format(_T("%0.0f"), KbPerSecond*1024);
		AfxFormatString1(sRate, IDS_FTPTRANSFER_BYTESPERSECOND, sBytesPerSecond);
	}
	else if (KbPerSecond < 10)
	{
		CString sKiloBytesPerSecond;
		sKiloBytesPerSecond.Format(_T("%0.2f"), KbPerSecond);
		AfxFormatString1(sRate, IDS_FTPTRANSFER_KILOBYTESPERSECOND, sKiloBytesPerSecond);
	}
	else
	{
		CString sKiloBytesPerSecond;
		sKiloBytesPerSecond.Format(_T("%0.0f"), KbPerSecond);
		AfxFormatString1(sRate, IDS_FTPTRANSFER_KILOBYTESPERSECOND, sKiloBytesPerSecond);
	}
	m_ctrlTransferRate.SetWindowText(sRate);
}

void CFTPTransferDlg::PlayAnimation()
{
	m_ctrlAnimate.Play(0, (UINT)-1, (UINT)-1);
}

void CFTPTransferDlg::HandleThreadErrorWithLastError(UINT nIDError, DWORD dwLastError)
{
	//Form the error string to report
	CString sError;
	
	DWORD dwErr = dwLastError;
	if (dwErr == 0)
		dwErr = ::GetLastError();
	if (dwErr == ERROR_INTERNET_EXTENDED_ERROR)
	{
		DWORD dwInetError;
		DWORD dwSize=0;
		::InternetGetLastResponseInfo(&dwInetError, NULL, &dwSize);
		TCHAR* pszResponse = new TCHAR[dwSize+1];
		::InternetGetLastResponseInfo(&dwInetError, pszResponse, &dwSize);
		pszResponse[dwSize] = _T('\0');
		sError = pszResponse;
		sError = _T("\n") + sError; //Add a line feed between the normal message
		//and the verbose error message
		delete [] pszResponse;
	}
	else
		sError.Format(_T("%d"), dwErr);
	AfxFormatString1(m_sError, nIDError, sError);
	
	//Delete the file being downloaded to if it is present
	if (m_bDownload)
	{
		m_LocalFile.Close();
		::DeleteFile(m_sLocalFile);
	}
	
	PostMessage(WM_FTPTRANSFER_THREAD_FINISHED, 1);
}

void CFTPTransferDlg::HandleThreadError(UINT nIDError)
{
	m_sError.LoadString(nIDError);
	PostMessage(WM_FTPTRANSFER_THREAD_FINISHED, 1);
}

void CFTPTransferDlg::TransferThread()
{
	//Create the Internet session handle (if needed)
	if (!m_bUsingAttached)
	{
		ASSERT(m_hInternetSession == NULL);
//		m_hInternetSession = ::InternetOpen(AfxGetAppName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		m_hInternetSession = InternetOpen("Ftp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (m_hInternetSession == NULL)
		{
			TRACE(_T("Failed in call to InternetOpen, Error:%d\n"), ::GetLastError());
			HandleThreadErrorWithLastError(IDS_FTPTRANSFER_GENERIC_ERROR);
			return;
		}
		
		//Should we exit the thread
		if (m_bAbort)
		{
			PostMessage(WM_FTPTRANSFER_THREAD_FINISHED);
			return;
		} 
	} 
	ASSERT(m_hInternetSession);  
	
	//Setup the status callback function on the Internet session handle
	INTERNET_STATUS_CALLBACK pOldCallback = ::InternetSetStatusCallback(m_hInternetSession, _OnStatusCallBack);
	if (pOldCallback == INTERNET_INVALID_STATUS_CALLBACK)
	{
		TRACE(_T("Failed in call to InternetSetStatusCallback, Error:%d\n"), ::GetLastError());
		HandleThreadErrorWithLastError(IDS_FTPTRANSFER_GENERIC_ERROR);
		return;
	}
	
	//Should we exit the thread
	if (m_bAbort)
	{
		if (pOldCallback)
			::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
		PostMessage(WM_FTPTRANSFER_THREAD_FINISHED);
		return;
	}  
	
	//Make the connection to the FTP server (if needed)
	if (!m_bUsingAttached)
	{
		ASSERT(m_hFTPConnection == NULL);
		ASSERT(m_sServer.GetLength());
		if (m_sUserName.GetLength())
//			m_hFTPConnection = ::InternetConnect(m_hInternetSession, m_sServer, m_nPort, m_sUserName, 
//			m_sPassword, INTERNET_SERVICE_FTP, 0, (DWORD) this);
			
			m_hFTPConnection = ::InternetConnect(m_hInternetSession, m_sServer, m_nPort, m_sUserName,
			m_sPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		else
			m_hFTPConnection = ::InternetConnect(m_hInternetSession, m_sServer, m_nPort, NULL, 
			NULL, INTERNET_SERVICE_FTP, 0, (DWORD) this);
		if (m_hFTPConnection == NULL)
		{
			TRACE(_T("Failed in call to InternetConnect, Error:%d\n"), ::GetLastError());
			if (pOldCallback)
				::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
			HandleThreadErrorWithLastError(IDS_FTPTRANSFER_FAIL_CONNECT_SERVER);
			return;
		}
		
		//Should we exit the thread
		if (m_bAbort)
		{
			if (pOldCallback)
				::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
			PostMessage(WM_FTPTRANSFER_THREAD_FINISHED);
			return;
		} 
	}
	ASSERT(m_hFTPConnection); 
	
	//Start the animation to signify that the download is taking place
	PlayAnimation();
	
	//Update the status control to reflect that we are getting the file information
	SetStatus(IDS_FTPTRANSFER_GETTING_FILE_INFORMATION);
	
	// Get the length of the file to transfer		   
	DWORD dwFileSize = 0;
	BOOL bGotFileSize = FALSE;
	if (m_bDownload)
	{
		WIN32_FIND_DATA wfd;
		HINTERNET hFind = ::FtpFindFirstFile(m_hFTPConnection, m_sRemoteFile, &wfd, INTERNET_FLAG_RELOAD | 
			INTERNET_FLAG_DONT_CACHE, (DWORD) this); 
		if (hFind)
		{
			//Set the progress control range
			bGotFileSize = TRUE;
			dwFileSize = (DWORD) wfd.nFileSizeLow;
//			SetProgressRange(dwFileSize);
			SetProgressRange(0,100);
			
			//Destroy the enumeration handle now that we are finished with it
			InternetCloseHandle(hFind);
		}
	}
	else
	{
		bGotFileSize = TRUE;
		dwFileSize = m_LocalFile.GetLength();
//		SetProgressRange(dwFileSize);
		SetProgressRange(0, 100);
	}
	
	//Should we exit the thread
	if (m_bAbort)
	{
		if (pOldCallback)
			::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
		PostMessage(WM_FTPTRANSFER_THREAD_FINISHED);
		return;
	}  
	
	//check to see if the file already exists on the server  
	if (!m_bDownload)
	{
		WIN32_FIND_DATA wfd;
		HINTERNET hFind = ::FtpFindFirstFile(m_hFTPConnection, m_sRemoteFile, &wfd, INTERNET_FLAG_RELOAD | 
			INTERNET_FLAG_DONT_CACHE, (DWORD) this); 
		BOOL bFound = FALSE;
		if (hFind)
		{
			bFound = TRUE;
			
			//Destroy the enumeration handle now that we are finished with it
			InternetCloseHandle(hFind);
		}
		
		if (bFound && SendMessage(WM_FTPTRANSFER_ASK_OVERWRITE_FILE) == 0)	
		{
			if (pOldCallback)
				::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
			PostMessage(WM_FTPTRANSFER_THREAD_FINISHED, 1);
			return;
		}
	}
	
	//Should we exit the thread
	if (m_bAbort)
	{
		if (pOldCallback)
			::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
		PostMessage(WM_FTPTRANSFER_THREAD_FINISHED);
		return;
	}  
	
	//Open the remote file
	ASSERT(m_hFTPFile == NULL);
	if (m_bDownload)
	{
		if (m_bBinary)
			m_hFTPFile = FtpOpenFile(m_hFTPConnection, m_sRemoteFile, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, (DWORD) this);
		else
			m_hFTPFile = FtpOpenFile(m_hFTPConnection, m_sRemoteFile, GENERIC_READ, FTP_TRANSFER_TYPE_ASCII | 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, (DWORD) this);
	}
	else
	{
		if (m_bBinary)	
			m_hFTPFile = FtpOpenFile(m_hFTPConnection, m_sRemoteFile, GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY | 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, (DWORD) this);
		else
			m_hFTPFile = FtpOpenFile(m_hFTPConnection, m_sRemoteFile, GENERIC_WRITE, FTP_TRANSFER_TYPE_ASCII | 
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, (DWORD) this);
	}
	if (m_hFTPFile == NULL)
	{
		TRACE(_T("Failed in call to FtpOpenFile, Error:%d\n"), ::GetLastError());
		if (pOldCallback)
			::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
		HandleThreadErrorWithLastError(IDS_FTPTRANSFER_FAIL_OPEN_FILE);
		return;
	}
	
	//Should we exit the thread
	if (m_bAbort)
	{
		if (pOldCallback)
			::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
		PostMessage(WM_FTPTRANSFER_THREAD_FINISHED);
		return;
	}  
	
	//Update the status to say that we are now uploading / downloading the file
	if (m_bDownload)
		SetStatus(IDS_FTPTRANSFER_RETREIVEING_FILE);
	else
		SetStatus(IDS_FTPTRANSFER_UPLOADING_FILE);
	
	//Now do the actual reading / writing of the file
	DWORD dwStartTicks = ::GetTickCount();
	DWORD dwCurrentTicks = dwStartTicks;
	DWORD dwBytesRead = 0;
	DWORD dwBytesWritten = 0;
	char szReadBuf[1024];
	DWORD dwBytesToRead = 1024;
	DWORD dwTotalBytesRead = 0;
	DWORD dwTotalBytesWritten = 0;	
	DWORD dwLastTotalBytes = 0;
	DWORD dwLastPercentage = 0;
	do
	{
		if (m_bDownload)
		{
			//Read from the remote file
			if (!::InternetReadFile(m_hFTPFile, szReadBuf, dwBytesToRead, &dwBytesRead))
			{
				TRACE(_T("Failed in call to InternetReadFile, Error:%d\n"), ::GetLastError());
				if (pOldCallback)
					::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
				HandleThreadErrorWithLastError(IDS_FTPTRANSFER_ERROR_READFILE);
				return;
			}
			else if (dwBytesRead && !m_bAbort)
			{
				//Write the data to file
				TRY
				{
					m_LocalFile.Write(szReadBuf, dwBytesRead);
				}
				CATCH(CFileException, e);										   
				{
					TRACE(_T("An exception occured while writing to the download file\n"));
					if (pOldCallback)
						::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
					HandleThreadErrorWithLastError(IDS_FTPTRANSFER_ERROR_READFILE, e->m_lOsError);
					e->Delete();
					return;
				}
				END_CATCH
					
					//Increment the total number of bytes read
					dwTotalBytesRead += dwBytesRead;  
				
				UpdateControlsDuringTransfer(dwStartTicks, dwCurrentTicks, dwTotalBytesRead, dwLastTotalBytes, 
					dwLastPercentage, bGotFileSize, dwFileSize);
			}
		}
		else
		{
			//Read the data from the local file
			TRY
			{
				dwBytesRead = m_LocalFile.Read(szReadBuf, 1024);
			}
			CATCH(CFileException, e);										 
			{
				TRACE(_T("An exception occured while reading the local file\n"));
				if (pOldCallback)
					::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
				HandleThreadErrorWithLastError(IDS_FTPTRANSFER_ERROR_READFILE, e->m_lOsError);
				e->Delete();
				return;
			}
			END_CATCH
				
				//Write to the remote file
				if (dwBytesRead)
				{
					if (!::InternetWriteFile(m_hFTPFile, szReadBuf, dwBytesRead, &dwBytesWritten))
					{
						TRACE(_T("Failed in call to InternetWriteFile, Error:%d\n"), ::GetLastError());
						if (pOldCallback)
							::InternetSetStatusCallback(m_hInternetSession, pOldCallback);
						HandleThreadErrorWithLastError(IDS_FTPTRANSFER_ERROR_WRITEFILE);
						return;
					}
					else if (dwBytesWritten && !m_bAbort)
					{
						//Increment the total number of bytes read
						dwTotalBytesWritten += dwBytesWritten;	
						
						UpdateControlsDuringTransfer(dwStartTicks, dwCurrentTicks, dwTotalBytesWritten, dwLastTotalBytes, 
							dwLastPercentage, bGotFileSize, dwFileSize);
					}
				}
		}
	} 
	while (dwBytesRead && !m_bAbort);
	
	//Delete the file being downloaded, if it is present and the download was aborted
	m_LocalFile.Close();
	if (m_bAbort && m_bDownload)
		::DeleteFile(m_sLocalFile);
	
	//We're finished
	PostMessage(WM_FTPTRANSFER_THREAD_FINISHED);
}

void CFTPTransferDlg::UpdateControlsDuringTransfer(DWORD dwStartTicks, DWORD& dwCurrentTicks, DWORD dwTotalBytesRead, DWORD& dwLastTotalBytes, 
												   DWORD& dwLastPercentage, BOOL bGotFileSize, DWORD dwFileSize)
{
	if (bGotFileSize)
	{
		//Update the percentage downloaded in the caption
		DWORD dwPercentage = (DWORD) (dwTotalBytesRead * 100.0 / dwFileSize);
		if (dwPercentage != dwLastPercentage)
		{
			SetPercentage(dwPercentage);
			dwLastPercentage = dwPercentage;
			
			//Update the progress control bar
//			SetProgress(dwTotalBytesRead);
			SetProgress((int)dwPercentage);
		}
	}
	
	//Update the transfer rate amd estimated time left every second
	DWORD dwNowTicks = GetTickCount();
	DWORD dwTimeTaken = dwNowTicks - dwCurrentTicks;
	if (dwTimeTaken > 1000)
	{
		double KbPerSecond = ((double)(dwTotalBytesRead) - (double)(dwLastTotalBytes)) / ((double)(dwTimeTaken));
		SetTransferRate(KbPerSecond);
		
		//Setup for the next time around the loop
		dwCurrentTicks = dwNowTicks;
		dwLastTotalBytes = dwTotalBytesRead;
		
		if (bGotFileSize)
		{
			//Update the estimated time left
			if (dwTotalBytesRead)
			{
				DWORD dwSecondsLeft = (DWORD) (((double)dwNowTicks - dwStartTicks) / dwTotalBytesRead * 
					(dwFileSize - dwTotalBytesRead) / 1000);
				SetTimeLeft(dwSecondsLeft, dwTotalBytesRead, dwFileSize);
			}
		}
	}
}

void CALLBACK CFTPTransferDlg::_OnStatusCallBack(HINTERNET hInternet, DWORD dwContext, DWORD dwInternetStatus, 
												 LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	//Convert from the SDK C world to the C++ world
	CFTPTransferDlg* pDlg = (CFTPTransferDlg*) dwContext;
	ASSERT(pDlg);
	ASSERT(pDlg->IsKindOf(RUNTIME_CLASS(CFTPTransferDlg)));
	pDlg->OnStatusCallBack(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
}

void CFTPTransferDlg::OnStatusCallBack(HINTERNET /*hInternet*/, DWORD dwInternetStatus, 
									   LPVOID lpvStatusInformation, DWORD /*dwStatusInformationLength*/)
{
	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		{
			SetStatus(IDS_FTPTRANSFER_RESOLVING_NAME, (LPCTSTR) lpvStatusInformation);
			break;
		}
	case INTERNET_STATUS_NAME_RESOLVED:
		{
			SetStatus(IDS_FTPTRANSFER_RESOLVED_NAME, (LPCTSTR) lpvStatusInformation);
			break;
		}
	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		{
			SetStatus(IDS_FTPTRANSFER_CONNECTING, (LPCTSTR) lpvStatusInformation);
			break;
		}
	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		{
			SetStatus(IDS_FTPTRANSFER_CONNECTED, (LPCTSTR) lpvStatusInformation);
			break;
		}
	default:
		{
			break;
		}
	}
}

void CFTPTransferDlg::OnDestroy() 
{
	//Wait for the worker thread to exit
	if (m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		delete m_pThread;
		m_pThread = NULL;
	}
	
	//Free up the internet handles we may be using
	::InternetCloseHandle(m_hFTPFile);
	m_hFTPFile = NULL;
	
	if (!m_bUsingAttached)
	{
		::InternetCloseHandle(m_hFTPConnection);
		::InternetCloseHandle(m_hInternetSession);
	}
	m_hFTPConnection = NULL;
	m_hInternetSession = NULL;
	
	//Let the parent class do its thing
	CDialog::OnDestroy();
}

void CFTPTransferDlg::OnCancel() 
{
	//Just set the abort flag to TRUE and
	//disable the cancel button
	m_bAbort = TRUE;	
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	SetStatus(IDS_FTPTRANSFER_ABORTING_TRANSFER);
}

void CFTPTransferDlg::OnClose() 
{
	if (m_bSafeToClose) 
		CDialog::OnClose();
	else
	{
		//Just set the abort flag to TRUE and
		//disable the cancel button
		m_bAbort = TRUE;	
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		SetStatus(IDS_FTPTRANSFER_ABORTING_TRANSFER);
	}
}

BOOL CFTPTransferDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreCreateWindow(cs);
}
