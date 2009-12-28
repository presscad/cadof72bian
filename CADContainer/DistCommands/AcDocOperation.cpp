#include "StdAfx.h"
#include "AcDocOperation.h"

//��̬����
DWORD			XAcDocOp::sm_dwStartAutoClose	= 0;		//���ö�̬�رյ�ʱ�䣬�Ժ���Ϊ��λ
DWORD			XAcDocOp::sm_dwTimerId			= 0;		//��ʱ����ID
CArray<HWND , HWND>	XAcDocOp::sm_arrThreadWindows;

//ö�ٵ�ǰ���Ѿ��򿪵��ĵ�
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

//��CAD���½�һ��CAD�ļ�������һ��״̬������½��ĵ���ָ��
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

	//�Ȼ�ȡ�ֽ�������ĵ�
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

	//��һ�λ�ȡ�����ĵ�
	es	= enumAcDocument(arrDocs1);
	if(es != Acad::eOk)
		return es;

	if(arrDocs.GetSize() == arrDocs1.GetSize())	//����ʧ��
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

		//�жϵ�ǰ�Ƿ���SDIģʽ
		BOOL	bIsSDIMode = FALSE;
		resbuf	*pRb;

		pRb	= acutNewRb(RTSHORT);
		if(acedGetVar("SDI" , pRb) == RTNORM && pRb->resval.rint == 1)	//��SDIģʽ
		{
			pCreateInfo->bActiveOldDoc	= FALSE;	//�����л�����ǰ���ĵ�
			if(curDoc() != NULL)
				es = acDocManager->closeDocument(curDoc());	//�رյ�ǰ�ĵ�
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
		//������ʱ��
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

	//ö�ٵ�ǰ�߳��д���
	EnumCurrentThreadWindows();

	HWND	hFindedWnd;
	DWORD	dwStyle;
	TCHAR	lpszCaption[100];
	//���ҷ���Ҫ���
	for(int i  = 0 ; i < sm_arrThreadWindows.GetCount() ; i++)
	{
		hFindedWnd	= sm_arrThreadWindows.GetAt(i);
		if(!::IsWindow(hFindedWnd))
			continue;

		dwStyle	= ::GetWindowLong(hFindedWnd , GWL_STYLE);
		if(dwStyle & WS_CHILD)	//���Ӵ���
			continue;

		::GetWindowText(hFindedWnd , lpszCaption , 100);
		if(_tcsicmp(lpszCaption , _T("AutoCAD ����")) == 0)	//�ҵ���ֻ����ʾ�Ի���
		{
			HWND	hOkWnd;
			hOkWnd	= ::GetDlgItem(hFindedWnd , IDYES);
			if(hOkWnd == NULL)
				continue;

			//������Ϣ
			::SendMessage(hFindedWnd , WM_COMMAND ,(WPARAM) MAKELONG(IDYES , BN_CLICKED) , (LPARAM)hOkWnd);

			bKillTimer	= TRUE;
			break;
		}
	}
	
	//�Ƿ����ٶ�ʱ�����ٶ�ʱ��
	if(!bKillTimer)
	{
		DWORD	dwCurrent;
		dwCurrent	= ::GetTickCount();
		if(dwCurrent > sm_dwStartAutoClose)	//ʱ�����10��
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

//��CAD�и���һ���ļ�����һ���ĵ�������һ��״̬������򿪵��ĵ�
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

	//�Ȼ�ȡ�ֽ�������ĵ�
	es	= enumAcDocument(arrDocs);
	if(es != Acad::eOk)
		return es;

	if(acDocManager->isApplicationContext())
	{
		AcApDocument	*pOldDoc = NULL;

		//�жϵ�ǰ�Ƿ���SDIģʽ
		BOOL	bIsSDIMode = FALSE;
		resbuf	*pRb;

		pRb	= acutNewRb(RTSHORT);
		if(acedGetVar("SDI" , pRb) == RTNORM && pRb->resval.rint == 1)	//��SDIģʽ
		{
			bKeepOldActive	= FALSE;	//�����л�����ǰ���ĵ�
			if(curDoc() != NULL)
                es = acDocManager->closeDocument(curDoc());	//�رյ�ǰ�ĵ�
        }
		acutRelRb(pRb);

		if(bKeepOldActive)
			pOldDoc = curDoc();

		if(bAutoCloseOnlyReadWarning)	//�Զ��ر�ֻ����ʾ�Ի���
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

		if(bAutoCloseOnlyReadWarning)	//�Զ��ر�ֻ����ʾ�Ի���
			AutoCloseOnlyReadWarningDlg();

		acDocManager->executeInApplicationContext(openDocCallBack , (void *)&createInfo);
	}

	//��һ�λ�ȡ�����ĵ�
	es	= enumAcDocument(arrDocs1);
	if(es != Acad::eOk)
		return es;

	if(arrDocs.GetSize() == arrDocs1.GetSize())	//����ʧ��
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

			//��������Ƿ����
			CString	strName;
			strName	= pDoc->fileName();

			ASSERT(strName.Compare(strFilePathName) == 0);
		}
	}

	return Acad::eCreateFailed;
}