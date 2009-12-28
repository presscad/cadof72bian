// DlgHistory.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgHistory.h"

#include "DataDefine.h"

extern MapString2ContentTree Glb_mapHistoryTree; // 六线历史数据
extern CStringArray Glb_arrayHistoryLayer; // 历史图层
extern ST_BIZZINFO Glb_BizzInfo; // 业务信息


// 向CAD窗口发送命令
void  SendMessageToCad(CString& strCommand, CStringArray& strParamArray)
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
	//	OutputDebugString(lpDocument->xml);

	COPYDATASTRUCT cds;
	cds.dwData = 0;
	cds.cbData = strXML.GetLength()+1;
	cds.lpData = (void*)strXML.GetBuffer(0);
	strXML.ReleaseBuffer();
	::SendMessage(acedGetAcadFrame()->m_hWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}

// CDlgHistory 对话框

IMPLEMENT_DYNAMIC(CDlgHistory, CDialog)
CDlgHistory::CDlgHistory(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHistory::IDD, pParent)
{
}

CDlgHistory::~CDlgHistory()
{
}

void CDlgHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE, m_wndTree);
	DDX_Control(pDX, IDC_DATETIMEPICKER, m_wndDate);
}


BEGIN_MESSAGE_MAP(CDlgHistory, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgHistory 消息处理程序

BOOL CDlgHistory::OnInitDialog() 
{
	CDialog::OnInitDialog();

	MapString2ContentTree::iterator _iter = Glb_mapHistoryTree.begin();
	while(_iter != Glb_mapHistoryTree.end())
	{
		CString strTempCadLayerName = _iter->first;
		STB_CONTENT_TREE xxTempContentTree  = _iter->second;

		HTREEITEM hItem = m_wndTree.InsertItem(strTempCadLayerName);

		_iter++;
	}

	return TRUE;
}

void CDlgHistory::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OutputDebugString("----is about to invoke OnOK()----");
	OnOK();

	CTime time;
	m_wndDate.GetTime(time);

	CString strTime;
	strTime.Format("%4d%02d%02d%02d%02d%02d", time.GetYear(), time.GetMonth(), time.GetDay(), 23, 59, 59);
//	strTime.Format("%4d-%02d-%02d", time.GetYear(), time.GetMonth(), time.GetDay());
	
	Glb_arrayHistoryLayer.RemoveAll();

	HTREEITEM hItem = m_wndTree.GetRootItem();
	while (hItem != NULL)
	{
		if (m_wndTree.GetCheck(hItem))
		{
			Glb_arrayHistoryLayer.Add(m_wndTree.GetItemText(hItem));
		}
		hItem = m_wndTree.GetNextItem(hItem, TVGN_NEXT);
	}

	CString strCommand = "loadHistoryLayer";
	CStringArray arrayParams;
	CString strParam;
	strParam.Format("where=EDITUSER='%s' AND EDITTIME <= '%s'", Glb_BizzInfo.strLoginUserName, strTime);
	arrayParams.Add(strParam);
	OutputDebugString("----is about to invoke SendMessageToCad----");
	SendMessageToCad(strCommand, arrayParams);
}
