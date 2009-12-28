// (C) Copyright 2002-2003 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.h
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "LayerManager.h"
#include "SampToolWnd.h"
#include "SampToolBar.h"
#include "Resourcehelper.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

#include "DistLayerTree_Ext.h"

CLayerManager * pLayerManager = NULL;

CSampToolBar* gpBar = NULL;
CSampToolWnd* gpWnd = NULL;

DISTLAYERTEE_EXT BOOL Sys_LoadLayerTree(LPCTSTR lpXml, long nType)
{
	if (NULL == pLayerManager)
	{
		pLayerManager = new CLayerManager;

		CWnd *pWnd = acedGetAcadFrame();

		pLayerManager->Create(pWnd, "图层控制面板", nType);
	}

	pLayerManager->DockControlBar(CBRS_ALIGN_LEFT,CRect(0, 0, 200, 150));
	pLayerManager->RestoreControlBar();
	pLayerManager->ShowDockPane(TRUE);

	return pLayerManager->m_pLayerTreeDlg->LoadXmlFromBuffer(lpXml);
}

DISTLAYERTEE_EXT void Sys_GetCheckedLayers(CStringArray& arrayLayers)
{
	if (NULL == pLayerManager)
		return;

	pLayerManager->m_pLayerTreeDlg->GetCheckedLayers(arrayLayers);
}

DISTLAYERTEE_EXT void Sys_SetCheckedLayers(CStringArray& arrayLayers)
{	
	if (NULL == pLayerManager)
		return;

	pLayerManager->m_pLayerTreeDlg->SetCheckedLayers(arrayLayers);
}

DISTLAYERTEE_EXT void Sys_ShowDockTreePane(long nType) // nType： 项目ID 六线调整为0,电子展板为-1
{
	if (NULL == pLayerManager)
	{
		pLayerManager = new CLayerManager;

		CWnd *pWnd = acedGetAcadFrame();

		pLayerManager->Create(pWnd, "图层控制面板", nType);
	}

	pLayerManager->DockControlBar(CBRS_ALIGN_LEFT,CRect(0, 0, 200, 150));
	pLayerManager->RestoreControlBar();
	pLayerManager->ShowDockPane(TRUE);
}

void CreateSampToolBar();
void DeleteSampToolBar();
DISTLAYERTEE_EXT void Sys_ShowSampToolBar(BOOL bShow)
{
	CMDIFrameWnd* pFrame = acedGetAcadFrame();

	if (bShow)
	{
		if (NULL == gpWnd)
		{
			CreateSampToolBar();
		}

		gpBar->GetToolBarCtrl().EnableButton(IDC_TB_STARTEDIT, TRUE);
		gpBar->GetToolBarCtrl().EnableButton(IDC_TB_ENDEDIT, FALSE);
		gpBar->GetToolBarCtrl().EnableButton(IDC_TB_CANCELEDIT, FALSE);
		gpBar->GetToolBarCtrl().EnableButton(IDC_TB_LOADHISTORYDATA, TRUE);
		gpBar->GetToolBarCtrl().EnableButton(IDC_TB_VIEWLOG, TRUE);
	}
	else
	{
		DeleteSampToolBar();
	}

//	pFrame->RecalcLayout();
}

// 开始编辑
DISTLAYERTEE_EXT void Sys_StartEdit()
{
	if (gpWnd != NULL)		gpWnd->SendMessage(WM_COMMAND, IDC_TB_STARTEDIT, 0);
}

// 取消编辑
DISTLAYERTEE_EXT void Sys_EndEdit() 
{
	if (gpWnd != NULL)		gpWnd->SendMessage(WM_COMMAND, IDC_TB_ENDEDIT, 0);
}

// 结束编辑
DISTLAYERTEE_EXT void Sys_CancelEdit() 
{
	if (gpWnd != NULL)		gpWnd->SendMessage(WM_COMMAND, IDC_TB_CANCELEDIT, 0);
}

// 向CAD窗口发送命令
static void  SendMessageToCad(CString& strCommand, CStringArray& strParamArray)
{
	MSXML2::IXMLDOMDocumentPtr lpDocument;
	long hr = lpDocument.CreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) ) 
		_com_raise_error(hr);

	MSXML2::IXMLDOMProcessingInstructionPtr lpInstruction = lpDocument->createProcessingInstruction("xml","version='1.0' encoding='GB2312'");
	lpDocument->appendChild(lpInstruction);

	MSXML2::IXMLDOMElementPtr lpRoot = lpDocument->createElement(LPCTSTR("root"));

	lpDocument->appendChild(lpRoot);

	// 数据类型：命令或数据
	MSXML2::IXMLDOMElementPtr lpCommand = lpDocument->createElement("command");

	lpCommand->setAttribute("name", _variant_t(strCommand));

	lpRoot->appendChild(lpCommand);

	// 参数列表
	for (int i=0; i<strParamArray.GetSize(); i++)
	{
		CString strParam = strParamArray.GetAt(i);
		CString strName = strParam.Left(strParam.Find('='));
		CString strValue = strParam.Right(strParam.GetLength()-strParam.Find('=')-1);

		MSXML2::IXMLDOMElementPtr lpParam = lpDocument->createElement("param");
		lpParam->setAttribute("name", _variant_t(strName));
		lpParam->setAttribute("value", _variant_t(strValue));

		lpCommand->appendChild(lpParam);
	}

	CString strXML = (LPCTSTR)(lpDocument->xml);
	OutputDebugString(lpDocument->xml);

	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = strXML.GetLength()+1;
	cds.lpData = (void*)strXML.GetBuffer(0);
	strXML.ReleaseBuffer();
	::SendMessage(acedGetAcadFrame()->m_hWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}

void CreateSampToolBar()
{
	InitCommonControls();
	CMDIFrameWnd* pFrame = acedGetAcadFrame();
	pFrame->EnableDocking(CBRS_ALIGN_ANY);

	CTemporaryResourceOverride  rxNew;
	gpWnd = new CSampToolWnd();

	gpWnd->Create("AfxControlBar", NULL, WS_CHILD | WS_MINIMIZE | CBRS_ALIGN_ANY, 
		CRect(0,0, 1,1), pFrame, 0, NULL);
	gpBar = new CSampToolBar();
	gpBar->Create(pFrame);
	gpBar->LoadToolBar(IDR_TOOLBAR);
	gpBar->SetBarStyle(gpBar->GetBarStyle() | 
		CBRS_ALIGN_ANY | CBRS_TOOLTIPS | CBRS_FLYBY );

	// enable the hidden window to intercept the message
	gpBar->GetToolBarCtrl().SetOwner(gpWnd);

	gpBar->EnableDocking(CBRS_ALIGN_ANY);
	gpBar->SetWindowText(_T("SampToolbar"));
	AfxSetResourceHandle(acedGetAcadResourceInstance());

	//	pFrame->DockControlBar(gpBar, AFX_IDW_DOCKBAR_TOP);
	pFrame->ShowControlBar(gpBar, TRUE, TRUE);

	// restore the previous toolbar state
//	pFrame->LoadBarState("SampToolbar");

	pFrame->RecalcLayout();
}

void DeleteSampToolBar()
{
	if(gpWnd)
	{
		delete gpWnd;
		gpWnd = NULL;
	}
	if(gpBar)
	{
		delete gpBar;
		gpBar = NULL;
	}
	acedGetAcadFrame()->RecalcLayout();
}

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CDistLayerTreeApp : public AcRxArxApp {

public:
	CDistLayerTreeApp () : AcRxArxApp () 
	{
	}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) 
	{
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		// TODO: Add your initialization code here

		CAcModuleResourceOverride resourceOverride;

		CreateSampToolBar();
/*
		if (NULL == pLayerManager)
		{
			pLayerManager = new CLayerManager;

			CWnd *pWnd = acedGetAcadFrame();

			pLayerManager->Create(pWnd, "图层控制面板");
			pLayerManager->ShowDockPane(TRUE);
		}
*/
		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) 
	{
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;
		// TODO: Add your code here

		if(pLayerManager != NULL)
		{
			delete pLayerManager;
			pLayerManager = NULL;
		}

		DeleteSampToolBar();

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CDistLayerTreeApp)

//ACED_ARXCOMMAND_ENTRY_AUTO(CDistLayerTreeApp, DistLayerTree, _ShowDockTreePane_, ShowDockTreePane_, ACRX_CMD_TRANSPARENT, NULL)
