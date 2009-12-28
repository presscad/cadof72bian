// SampToolWnd.cpp : implementation file

#include "stdafx.h"
#include "SampToolWnd.h"
#include <aced.h>
#include "Resource.h"
#include "SampToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern Adesk::Boolean acedPostCommand(const char *pCmd);

extern CSampToolBar* gpBar;

/////////////////////////////////////////////////////////////////////////////
// CSampToolWnd

IMPLEMENT_DYNCREATE(CSampToolWnd, CWnd)

CSampToolWnd::CSampToolWnd() : CWnd()
{
}

CSampToolWnd::~CSampToolWnd()
{
//	CFrameWnd::~CFrameWnd();
}


BEGIN_MESSAGE_MAP(CSampToolWnd, CWnd)
	//{{AFX_MSG_MAP(CSampToolWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampToolWnd message handlers
void SendCmdToAcad(const char* pCh)
{
	CString str = pCh;
	COPYDATASTRUCT cmdMsg;
	cmdMsg.dwData = (DWORD)1;
	cmdMsg.cbData = (DWORD)_tcslen(str) + 1;
	cmdMsg.lpData = str.GetBuffer(str.GetLength()+1) ;                   
	SendMessage(adsw_acadMainWnd(), WM_COPYDATA, (WPARAM)adsw_acadMainWnd(), (LPARAM)&cmdMsg);
}

void SendMyCmdToAcad(const char* pCh)
{
	CString str = pCh;
	COPYDATASTRUCT cmdMsg;
	cmdMsg.dwData = (DWORD)2;
	cmdMsg.cbData = (DWORD)_tcslen(str) + 1;
	cmdMsg.lpData = str.GetBuffer(str.GetLength()+1) ;                   
	SendMessage(adsw_acadMainWnd(), WM_COPYDATA, (WPARAM)adsw_acadMainWnd(), (LPARAM)&cmdMsg);
}

/*
void broadMes(const char* pCh)
{
	CString str = pCh;
	DWORD re = BSM_APPLICATIONS;
	COPYDATASTRUCT cmdMsg;
	cmdMsg.dwData = (DWORD)1;
	cmdMsg.cbData = (DWORD)_tcslen(str) + 1;
	cmdMsg.lpData = str.GetBuffer(str.GetLength()+1) ;                   
	BroadcastSystemMessage(BSF_QUERY,&re, WM_COPYDATA, (WPARAM)adsw_acadMainWnd(), (LPARAM)&cmdMsg);

}
*/
BOOL CSampToolWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch(wParam)
	{
		case IDC_TB_STARTEDIT:
		{
			if (!gpBar->GetToolBarCtrl().IsButtonEnabled(IDC_TB_STARTEDIT))
				break;

			SendMyCmdToAcad("开始编辑");
			if (gpBar != NULL)
			{
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_STARTEDIT, FALSE);
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_ENDEDIT, TRUE);
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_CANCELEDIT, TRUE);
			}
			break;
		}
		case IDC_TB_ENDEDIT:
			if (!gpBar->GetToolBarCtrl().IsButtonEnabled(IDC_TB_ENDEDIT))
				break;

			SendMyCmdToAcad("结束编辑");
			if (gpBar != NULL)
			{
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_STARTEDIT, TRUE);
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_ENDEDIT, FALSE);
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_CANCELEDIT, FALSE);
			}
			break;
		case IDC_TB_CANCELEDIT:
			if (!gpBar->GetToolBarCtrl().IsButtonEnabled(IDC_TB_CANCELEDIT))
				break;

			SendMyCmdToAcad("取消编辑");
			if (gpBar != NULL)
			{
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_STARTEDIT, TRUE);
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_ENDEDIT, FALSE);
				gpBar->GetToolBarCtrl().EnableButton(IDC_TB_CANCELEDIT, FALSE);
			}
			break;
		case IDC_TB_LOADHISTORYDATA:
			SendCmdToAcad("LoadHistoryLayer ");
			break;
		case IDC_TB_VIEWLOG:
			SendCmdToAcad("ViewLog ");
			break;
		default:
			break;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

