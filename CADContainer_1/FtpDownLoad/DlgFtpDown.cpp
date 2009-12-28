// DlgFtpDown.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgFtpDown.h"
#include "DlgFTPTransfer.h"
#include <shlwapi.h>
#include "DistEntityTool.h"

// CDlgFtpDown 对话框



IMPLEMENT_DYNAMIC(CDlgFtpDown, CDialog)
CDlgFtpDown::CDlgFtpDown(CWnd* pParent /*=NULL*/)
	       : CDialog(CDlgFtpDown::IDD, pParent)
{
	m_pInetSession = NULL;
	m_strServer.Empty();
}

CDlgFtpDown::~CDlgFtpDown()
{
	if(NULL != m_pInetSession)
	{
		delete m_pInetSession; m_pInetSession=NULL;
	}

	int nLen = m_PointerArray.GetCount();
	for(int i=0; i<nLen; i++)
	{
		int nPointer = m_PointerArray.GetAt(i);
		char* pp = (char*)nPointer;
		delete [] pp;
	}
	m_PointerArray.RemoveAll();
}

void CDlgFtpDown::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_tree);
}


BEGIN_MESSAGE_MAP(CDlgFtpDown, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_TREE, OnClickTree)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnTvnSelchangedTree)
END_MESSAGE_MAP()


void CDlgFtpDown::Init(CString strServer,CString strUserName,CString strPassword,int nPort,CString strProjectId)
{
	m_strServer = strServer;
	m_strUserName = strUserName;
	m_strPassword = strPassword;
	m_nPort = nPort;
	m_strProjectId = strProjectId;
}


BOOL CDlgFtpDown::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_img.Create(16,16,ILC_COLOR,0,4);
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON1));

	//SHFILEINFO sfi; 
	//ZeroMemory(&sfi,sizeof(sfi));
	//SHGetFileInfo(".dwg", FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(sfi), SHGFI_ICON|SHGFI_USEFILEATTRIBUTES);
	//m_img.Add(sfi.hIcon);
	m_img.Add(AfxGetApp()->LoadIcon(IDI_ICON2));
	m_tree.SetItemHeight(17);
	m_tree.SetImageList(&m_img, TVSIL_NORMAL);


	return TRUE;
}


void CDlgFtpDown::SizeChanged (CRect *lpRect, BOOL bFloating, int flags) 
{
	CAcModuleResourceOverride resourceOverride ;
	SetWindowPos (this, lpRect->left + 4, lpRect->top + 4, lpRect->Width() - 4, lpRect->Height() - 4, SWP_NOZORDER) ;

	m_tree.SetWindowPos( this,lpRect->left, 2,lpRect->Width() - 6, lpRect->bottom - 10,SWP_NOZORDER);
}


//获取当前执行程序路径
CString CDlgFtpDown::GetCurAppPath()
{
	TCHAR exeFullPath[MAX_PATH]; 
	CString strPath; 
	GetModuleFileName(GetModuleHandle("shFtpDownLoad.arx"),exeFullPath,MAX_PATH); 
	strPath=(CString)exeFullPath; 
	int position=strPath.ReverseFind('\\'); 
	strPath=strPath.Left(position+1); 
	return strPath;
}

CString CDlgFtpDown::GetCurArxAppPath()
{
	CString appFileName = acedGetAppName();
	char dir[_MAX_DIR], drive[_MAX_DRIVE], path[_MAX_PATH];
	_splitpath(appFileName, drive, dir, NULL, NULL);
	_makepath(path, drive, dir, NULL, NULL);
	return path;
}


BOOL CDlgFtpDown::IsFileExists(const CString strFName)
{
	CFileStatus fileStat;
	return CFile::GetStatus(strFName, fileStat) != 0;
}



void CDlgFtpDown::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UINT uFlag;
	CPoint pt;
	HTREEITEM hSelRoot;
	GetCursorPos(&pt);
	m_tree.ScreenToClient(&pt);
	hSelRoot = m_tree.HitTest(pt,&uFlag);
	if ((NULL!=hSelRoot) && (uFlag&TVHT_ONITEM))
	{
		char* strBuf=NULL;
		strBuf = (char*) m_tree.GetItemData(hSelRoot);
		if(NULL == strBuf) return;

		CString strArxPath = GetCurAppPath();
		CString strTargeFile;
		strTargeFile.Format("%sFtp\\%s.dwg",strArxPath,m_tree.GetItemText(hSelRoot));

		CString strSourceFile,strTempA,strTempB;

		HTREEITEM hParent = hSelRoot;
		while((hParent = m_tree.GetParentItem(hParent))!=NULL)
		{
			strTempA = m_tree.GetItemText(hParent);
			strTempB = strTempA+"/"+strTempB;
		}
		strSourceFile.Format("Material/%s/%s%s",m_strProjectId,strTempB,strBuf);

		CDlgFTPTransfer dlg;
		dlg.m_sServer = m_strServer;
		dlg.m_sUserName = m_strUserName;
		dlg.m_sPassword = m_strPassword;
		dlg.m_nPort = m_nPort;
		dlg.m_sRemoteFile = strSourceFile;
		dlg.m_sLocalFile = strTargeFile;
		dlg.m_sFileName = strTargeFile;
		dlg.m_bDownload = TRUE;
		dlg.m_bBinary = TRUE;

		if (dlg.DoModal() != IDOK)
		{
			return;
		}

		CDistEntityTool tempTool;
		tempTool.OpenDwgFile(strTargeFile);
	}

	*pResult = 0;
}


void CDlgFtpDown::ConnectFtp()
{
	m_tree.DeleteAllItems();
	CInternetSession* m_pInetSession=new CInternetSession(AfxGetAppName(),1,PRE_CONFIG_INTERNET_ACCESS);
	try 
	{
		m_pFtpConnection=m_pInetSession->GetFtpConnection(m_strServer,m_strUserName,m_strPassword,m_nPort);

		if (m_pFtpConnection != NULL)
		{
			CString strRoot;
			strRoot.Format("Material/%s/",m_strProjectId);

			if(!m_pFtpConnection->SetCurrentDirectory(strRoot)) 
			{
				delete m_pInetSession; m_pInetSession=NULL;
				return;
			}
			
			DisplayFtpDir(NULL);
		}
	}
	catch (CInternetException * pEx)
	{
		TCHAR szError[1024];
		if ( pEx->GetErrorMessage(szError,1024)) 
		{
			//AfxMessageBox(szError);
		
			//char strBuf[1024]={0};
			//sprintf(strBuf,"\n%s\n%s\n%s\n%d\n%s",m_strServer,m_strUserName,m_strPassword,m_nPort,m_strProjectId);
			//MessageBox(strBuf);
		}

		else
		{
			//AfxMessageBox("There was an exception");
		}
		pEx->Delete();
		m_pFtpConnection=NULL;

		if(NULL != m_pInetSession)
		{
			delete m_pInetSession; m_pInetSession=NULL;
		}
	}

	if(NULL != m_pInetSession)
	{
		m_pFtpConnection=NULL;
		delete m_pInetSession;m_pInetSession=NULL;
	}
}



//枚举 FTP 目录
void CDlgFtpDown::DisplayFtpDir(HTREEITEM hOwnerItem)
{
	CStringArray  strDirArray;
	TREEITEMARRAY treeItemArray;


	CFtpFileFind finder(m_pFtpConnection);
	BOOL bWorking = finder.FindFile(_T("*"));

	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strTempFile;
		if(finder.IsDots()) continue;
		if(finder.IsDirectory())
		{  
			strTempFile = finder.GetFileName();
			strDirArray.Add(strTempFile);
			HTREEITEM hItem = m_tree.InsertItem(strTempFile,0,0,hOwnerItem);
			treeItemArray.Add(hItem);
		}
		else
		{
			strTempFile =finder.GetFileName();
			CString strTemp = strTempFile;
			strTempFile.MakeUpper();
			if(strTempFile.Find(".DWG")>0)
			{
				int nLen = strTempFile.GetLength();
				int nA=0,nB=0;
				for(int kk=nLen-5;kk>=0;kk--)
				{
					if(strTempFile.GetAt(kk) =='_')
					{
						if(nA>0)
						{
							nB=kk; break;
						}
						else
							nA=kk;
					}
				}
				if(nA>0 && nB>0)
					strTemp = strTempFile.Mid(nB+1,nA-nB-1);
				HTREEITEM hItem = m_tree.InsertItem(strTemp,1,1,hOwnerItem);
				char* strBuf = new char[strTempFile.GetLength()+1];
				memset(strBuf,0,sizeof(char)*strTempFile.GetLength()+1);
				strcpy(strBuf,strTempFile.GetBuffer(0));
				m_tree.SetItemData(hItem,(DWORD_PTR)strBuf);
				m_PointerArray.Add(int(strBuf));
			}
		}
	}
	finder.Close();

	m_tree.Expand(hOwnerItem,TVE_EXPAND);

	for(int i=0;i<strDirArray.GetSize();i++)
	{
		BOOL m_suc=0;
		while (!m_suc)
		{
			m_suc=m_pFtpConnection->SetCurrentDirectory(strDirArray.GetAt(i));
		}

		DisplayFtpDir(treeItemArray.GetAt(i));

		BOOL m_suc1=0;
		while(!m_suc1)
		{
			m_suc1= m_pFtpConnection->SetCurrentDirectory("..");
		}
	}
}


void CDlgFtpDown::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

}

void CDlgFtpDown::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
