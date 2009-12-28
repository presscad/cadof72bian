#include "StdAfx.h"
#include "AcDocOperation.h"

//静态变量
DWORD			XAcDocOp::sm_dwStartAutoClose	= 0;		//启用动态关闭的时间，以毫秒为单位
DWORD			XAcDocOp::sm_dwTimerId			= 0;		//定时器的ID
CArray<HWND , HWND>	XAcDocOp::sm_arrThreadWindows;

//枚举当前的已经打开的文档
Acad::ErrorStatus XAcDocOp::enumAcDocument(AcApDocuments &arr)
{
	arr.RemoveAll();

	AcApDocumentIterator *pIter;

	pIter	= acDocManager->newAcApDocumentIterator();
	if(pIter == NULL)
		return Acad::eNullIterator;

	for( ; !pIter->done() ; pIter->step())
	{
		arr.Add(pIter->document());
	}

	delete pIter;	//added 2006-11

	return Acad::eOk;
}

void XAcDocOp::newDocCallBack(void *pData)
{
	ASSERT(acDocManager->isApplicationContext());

	CString strTemplateName;

	if(pData == NULL || !AfxIsValidAddress(pData , sizeof(_CREATEDOCUMENTINFO)))
		return;

	Acad::ErrorStatus	es;
	try
	{
		_CREATEDOCUMENTINFO	*pCreateInfo;
		pCreateInfo		= (_CREATEDOCUMENTINFO*)pData;
		strTemplateName = pCreateInfo->pszName;
		strTemplateName.Trim();

		es = acDocManager->appContextNewDocument(strTemplateName);

		if(pCreateInfo->bActiveOldDoc && pCreateInfo->pOldDoc != NULL)
			acDocManager->activateDocument(pCreateInfo->pOldDoc);
	}
	catch (...)
	{
	}
}

//在CAD中新建一个CAD文件，返回一个状态和输出新建文档的指针
Acad::ErrorStatus XAcDocOp::newAcDocument(CString			strTemplateName , 
										  AcApDocument	*&pDoc ,
										  BOOL			bKeepOldActive)
{
	AcApDocuments		arrDocs , arrDocs1;
	Acad::ErrorStatus	es;

	strTemplateName.Trim();
	if(strTemplateName.GetLength() == 0)
		return Acad::eFileNotFound;

	pDoc	= NULL;

	//先获取现今的所有文档
	es	= enumAcDocument(arrDocs);
	if(es != Acad::eOk)
		return es;

	if(acDocManager->isApplicationContext())
	{
		AcApDocument	*pOldDoc = NULL;

		if(bKeepOldActive)
			pOldDoc	= curDoc();

		es = acDocManager->appContextNewDocument(strTemplateName);

		if(es != Acad::eOk)
			return es;

		if(bKeepOldActive && pOldDoc != NULL)
			acDocManager->activateDocument(pOldDoc);
	}
	else
	{
		_CREATEDOCUMENTINFO	createInfo;
        ::ZeroMemory(&createInfo , sizeof(_CREATEDOCUMENTINFO));
		_tcscpy(createInfo.pszName , strTemplateName);

		if(bKeepOldActive)
		{
			createInfo.bActiveOldDoc	= TRUE;
			createInfo.pOldDoc			= curDoc();
		}

		acDocManager->executeInApplicationContext(newDocCallBack , (void *)&createInfo);
	}

	//再一次获取所有文档
	es	= enumAcDocument(arrDocs1);
	if(es != Acad::eOk)
		return es;

	if(arrDocs.GetSize() == arrDocs1.GetSize())	//创立失败
		return Acad::eCreateFailed;

	BOOL	bFind;
	for(int i = 0 ; i < arrDocs1.GetSize() ; i++)
	{
		bFind	= FALSE;
		for(int j = 0 ; j < arrDocs.GetSize() ; j++)
		{
			if(arrDocs.GetAt(j) == arrDocs1.GetAt(i))
			{
				bFind	= TRUE;
				break;
			}
		}

		if(!bFind)
		{
			pDoc = arrDocs1.GetAt(i);
			return Acad::eOk;
		}
	}

	return Acad::eCreateFailed;
}

void XAcDocOp::openDocCallBack(void *pData)
{
	if(pData == NULL || !AfxIsValidAddress(pData , sizeof(_CREATEDOCUMENTINFO)))
		return;

	CString strFilePathName;

	if(pData == NULL || !AfxIsValidAddress(pData , sizeof(_CREATEDOCUMENTINFO)))
		return;

	Acad::ErrorStatus	es;
	try
	{
		_CREATEDOCUMENTINFO	*pCreateInfo;
		pCreateInfo		= (_CREATEDOCUMENTINFO*)pData;
		strFilePathName = pCreateInfo->pszName;
		strFilePathName.Trim();

		//判断当前是否处于SDI模式
		BOOL	bIsSDIMode = FALSE;
		resbuf	*pRb;

		pRb	= acutNewRb(RTSHORT);
		if(acedGetVar("SDI" , pRb) == RTNORM && pRb->resval.rint == 1)	//是SDI模式
		{
			pCreateInfo->bActiveOldDoc	= FALSE;	//不能切换到先前的文档
			if(curDoc() != NULL)
				es = acDocManager->closeDocument(curDoc());	//关闭当前文档
		}
		acutRelRb(pRb);

		es = acDocManager->appContextOpenDocument(strFilePathName);

		if(pCreateInfo->bActiveOldDoc && pCreateInfo->pOldDoc != NULL)
			acDocManager->activateDocument(pCreateInfo->pOldDoc);
	}
	catch (...)
	{
	}
}

BOOL CALLBACK XAcDocOp::EnumThreadWndProc(
								HWND hwnd,      // handle to window
								LPARAM lParam   // application-defined value
								)
{
	sm_arrThreadWindows.Add(hwnd);

	return TRUE;
}

void XAcDocOp::EnumCurrentThreadWindows()
{
	sm_arrThreadWindows.RemoveAll();
	::EnumThreadWindows(::GetCurrentThreadId() , XAcDocOp::EnumThreadWndProc , 0);
}

void XAcDocOp::AutoCloseOnlyReadWarningDlg()
{
	if(sm_dwTimerId == 0)
	{
		//启动定时器
		sm_dwTimerId		= ::SetTimer(NULL , 0 , 100 , AutoCloseTimerProc);
	}

	sm_dwStartAutoClose	= ::GetTickCount();
}

void CALLBACK XAcDocOp::AutoCloseTimerProc(
									  HWND hwnd,         // handle to window
									  UINT uMsg,         // WM_TIMER message
									  UINT_PTR idEvent,  // timer identifier
									  DWORD dwTime       // current system time
									  )
{
	assert(sm_dwTimerId == idEvent);

	BOOL	bKillTimer	= FALSE;

	//枚举当前线程中窗口
	EnumCurrentThreadWindows();

	HWND	hFindedWnd;
	DWORD	dwStyle;
	TCHAR	lpszCaption[100];
	//查找符合要求的
	for(int i  = 0 ; i < sm_arrThreadWindows.GetCount() ; i++)
	{
		hFindedWnd	= sm_arrThreadWindows.GetAt(i);
		if(!::IsWindow(hFindedWnd))
			continue;

		dwStyle	= ::GetWindowLong(hFindedWnd , GWL_STYLE);
		if(dwStyle & WS_CHILD)	//是子窗口
			continue;

		::GetWindowText(hFindedWnd , lpszCaption , 100);
		if(_tcsicmp(lpszCaption , _T("AutoCAD 警告")) == 0)	//找到了只读提示对话框
		{
			HWND	hOkWnd;
			hOkWnd	= ::GetDlgItem(hFindedWnd , IDYES);
			if(hOkWnd == NULL)
				continue;

			//发送消息
			::SendMessage(hFindedWnd , WM_COMMAND ,(WPARAM) MAKELONG(IDYES , BN_CLICKED) , (LPARAM)hOkWnd);

			bKillTimer	= TRUE;
			break;
		}
	}
	
	//是否销毁定时器销毁定时器
	if(!bKillTimer)
	{
		DWORD	dwCurrent;
		dwCurrent	= ::GetTickCount();
		if(dwCurrent > sm_dwStartAutoClose)	//时间大于10秒
		{
			if(dwCurrent - sm_dwStartAutoClose > 10000)
				bKillTimer	= TRUE;
		}
		else
		{
			if(sm_dwStartAutoClose - dwCurrent > 10000)
				bKillTimer	= TRUE;
		}		
	}

	if(bKillTimer)
	{
		::KillTimer(NULL , idEvent);

		sm_dwStartAutoClose	= 0;
		sm_dwTimerId		= 0;
	}
}

//在CAD中根据一个文件名打开一个文档，返回一个状态和输出打开的文档
Acad::ErrorStatus XAcDocOp::openAcDocument(CString		strFilePathName , 
										 AcApDocument	*&pDoc ,
										 BOOL			bKeepOldActive ,
										 BOOL bAutoCloseOnlyReadWarning)
{
	AcApDocuments		arrDocs , arrDocs1;
	Acad::ErrorStatus	es;

	strFilePathName.Trim();
	if(strFilePathName.GetLength() == 0)
		return Acad::eFileNotFound;

	pDoc	= NULL;

	//先获取现今的所有文档
	es	= enumAcDocument(arrDocs);
	if(es != Acad::eOk)
		return es;

	if(acDocManager->isApplicationContext())
	{
		AcApDocument	*pOldDoc = NULL;

		//判断当前是否处于SDI模式
		BOOL	bIsSDIMode = FALSE;
		resbuf	*pRb;

		pRb	= acutNewRb(RTSHORT);
		if(acedGetVar("SDI" , pRb) == RTNORM && pRb->resval.rint == 1)	//是SDI模式
		{
			bKeepOldActive	= FALSE;	//不能切换到先前的文档
			if(curDoc() != NULL)
                es = acDocManager->closeDocument(curDoc());	//关闭当前文档
        }
		acutRelRb(pRb);

		if(bKeepOldActive)
			pOldDoc = curDoc();

		if(bAutoCloseOnlyReadWarning)	//自动关闭只读提示对话框
			AutoCloseOnlyReadWarningDlg();

        es = acDocManager->appContextOpenDocument(strFilePathName);

		if(es != Acad::eOk)
			return es;

		if(bKeepOldActive && pOldDoc != NULL)
			acDocManager->activateDocument(pOldDoc);		
	}
	else
	{
		_CREATEDOCUMENTINFO	createInfo;
        ::ZeroMemory(&createInfo , sizeof(_CREATEDOCUMENTINFO));
		_tcscpy(createInfo.pszName , strFilePathName);

		if(bKeepOldActive)
		{
			createInfo.bActiveOldDoc	= TRUE;
			createInfo.pOldDoc			= curDoc();
		}

		if(bAutoCloseOnlyReadWarning)	//自动关闭只读提示对话框
			AutoCloseOnlyReadWarningDlg();

		acDocManager->executeInApplicationContext(openDocCallBack , (void *)&createInfo);
	}

	//再一次获取所有文档
	es	= enumAcDocument(arrDocs1);
	if(es != Acad::eOk)
		return es;

	if(arrDocs.GetSize() == arrDocs1.GetSize())	//创立失败
		return Acad::eCreateFailed;

	BOOL	bFind;
	for(int i = 0 ; i < arrDocs1.GetSize() ; i++)
	{
		bFind	= FALSE;
		for(int j = 0 ; j < arrDocs.GetSize() ; j++)
		{
			if(arrDocs.GetAt(j) == arrDocs1.GetAt(i))
			{
				bFind	= TRUE;
				break;
			}
		}

		if(!bFind)
		{
			pDoc = arrDocs1.GetAt(i);
			return Acad::eOk;

			//检查名字是否相等
			CString	strName;
			strName	= pDoc->fileName();

			ASSERT(strName.Compare(strFilePathName) == 0);
		}
	}

	return Acad::eCreateFailed;
}