// Dlg_Attribute.cpp : 实现文件
//

#include "stdafx.h"
#include "Dlg_Attribute.h"
#include "DataDefine.h"
#include "..\SdeData\DistBaseInterface.h"
#include "DistXData.h"
#include "CommonFunc.h"

extern BOOL Glb_bMultiConnection;
extern IDistConnection * Glb_pConnection;         // SDE数据库连接对象指针
extern IDistConnection * Glb_pLocalConnection;         // 市局 SDE数据库连接对象指针
extern IDistExchangeData*  Glb_pExchangeData;       // SDE<->CAD数据交换对象指针

extern MapString2ContentTree Glb_mapTree; // 树结构对照表

extern MapString2String Glb_BizzInfoEx;// 业务信息

extern ST_BIZZINFO Glb_BizzInfo; // 业务信息


// CDlgAttribute 对话框

IMPLEMENT_DYNAMIC(CDlgAttribute, CAcUiDialog)
CDlgAttribute::CDlgAttribute(CWnd* pParent /*=NULL*/)
	: CAcUiDialog(CDlgAttribute::IDD, pParent)
{
	m_bInsertOnly = FALSE;
}

CDlgAttribute::~CDlgAttribute()
{
}

void CDlgAttribute::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_XDATA, m_wndList);
}


BEGIN_MESSAGE_MAP(CDlgAttribute, CAcUiDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE(WM_ON_BEGINEDIT, OnBeginEdit)
	ON_MESSAGE(WM_ON_ENDEDIT, OnEndEdit)
END_MESSAGE_MAP()


// CDlgAttribute 消息处理程序

void CDlgAttribute::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCount = paramList.GetCount();
	char** pValues = new char*[nCount];

	char strXDataName[1024]={0};
	sprintf(strXDataName,"DIST_%s",m_strSdeLayerName);

	int nLenNum=m_objIds.logicalLength();

	for(int i=0;i<nLenNum;i++)
	{
		AcDbObjectId objId = m_objIds.at(i);

		CDistXData tempXdata(objId,strXDataName);

		for(int i=0; i<nCount; i++)
		{
			CString strField = paramList.GetAt(i).strName;

			CString strValue = m_wndList.GetItemText(i, 1);

			if (!strValue.IsEmpty())
			{
				tempXdata.Add(strField, strValue);
			}
		}

		tempXdata.Update();
		tempXdata.Close();
	}


	IDistConnection* pConnection = Glb_pConnection;
	IDistExchangeData* pExchangeData = Glb_pExchangeData;

//	AcDbObjectIdArray objIdArray;
//	objIdArray.append(m_objId);
	SaveSdeLayer(pConnection, pExchangeData, m_strSdeLayerName, m_objIds, DocVars.docData().GetProjectId(), false, m_bInsertOnly);

	OnOK();
}

BOOL CDlgAttribute::OnInitDialog() 
{
	CAcUiDialog::OnInitDialog();

	//ListCtrl初始化
	m_wndList.SetGridLines();
	m_wndList.SetFullSelectedStyle();
	m_wndList.SetExtendedStyle(LVS_EX_INFOTIP);
	m_wndList.SetHeadings( _T("名称,90;内容,120") );
	m_wndList.SetAllowEdit();
	m_wndList.SetEditTextColor(RGB(255,0,0));
	m_wndList.SetEditBkColor(RGB(255,255,255));

	IDistConnection* pConnection = Glb_pConnection;

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	IDistCommand* pCommand = CreateSDECommandObjcet();
	pCommand->SetConnectObj(pConnection);

	long nRet = pCommand->GetTableInfo(&pParam,&nParamNum,m_strSdeLayerName.GetBuffer(),"SDE");
	m_strSdeLayerName.ReleaseBuffer();
	if(1 != nRet)
	{
		pCommand->Release(); 
		return FALSE;
	}

	for(int i=0; i<nParamNum; i++)
	{
		if(pParam[i].fld_nType == kShape) 
			continue;

		if(pParam[i].fld_nRowIdType == kRowIdColumnSde)
			continue;

		COLUMN_VALUE xxParam;
		xxParam.strName = pParam[i].fld_strName;
		xxParam.strAlias = pParam[i].fld_strAliasName;

		xxParam.strValue.ReleaseBuffer();

		long nType = 0;
		switch(pParam[i].fld_nType)
		{
		case kInt16:
		case kInt32:
			nType = 1;
			break;
		case kFloat32:
		case kFloat64:
			nType = 2;
			break;
		case kDate:
			nType = 3;
			break;
		case kString:
		case kNString:
		case kBLOB:
		case kShape:
			break;
		default:
			break;
		}

		bool bReadOnly = false;
		if (xxParam.strName.CompareNoCase("XMID") == 0)
		{
			xxParam.strValue = DocVars.docData().GetProjectId();
			bReadOnly = true;
		}
		if (xxParam.strName.CompareNoCase("XMBH") == 0)
		{
			xxParam.strValue = Glb_BizzInfo.strBizzCode;
			bReadOnly = true;
		}
		else if (xxParam.strName.CompareNoCase("XMMC") == 0)
		{
			xxParam.strValue = Glb_BizzInfo.strBizzName;
			bReadOnly = true;
		}
		else if (xxParam.strName.CompareNoCase("JSDW") == 0)
		{
			xxParam.strValue = Glb_BizzInfo.strBzOwner;
			bReadOnly = true;
		}
		else
		{
			MapString2String::iterator iter = Glb_BizzInfoEx.begin();
			while (iter != Glb_BizzInfoEx.end()) 
			{
				CString strName = iter->first;
				CString strValue = iter->second;

				CString strTable = strName.Left(strName.Find("→"));
				CString strField = strName.Right(strName.GetLength()-strName.Find("→")-strlen("→"));

//				if (strTable.CompareNoCase(m_trCadLayerName) == 0)
				{
					if (strField.CompareNoCase(xxParam.strName) == 0)
					{
						xxParam.strValue = strValue;
						break;
					}
				}

				iter++;
			}
		}

		paramList.Add(xxParam);
		int nIndex = m_wndList.AddItem(nType, xxParam.strAlias, xxParam.strValue);

		if (bReadOnly)
			m_wndList.SetItemTextColor(nIndex, 1, GetSysColor(COLOR_GRAYTEXT));
	}

	pCommand->Release(); 
	return TRUE;
}

LRESULT CDlgAttribute::OnBeginEdit(WPARAM wParam, LPARAM lParam)
{
	int nItem = wParam;
	int nSubItem = lParam;

	COLORREF textColor;
	m_wndList.GetItemTextColor(nItem, nSubItem, textColor);

	if (textColor == GetSysColor(COLOR_GRAYTEXT))
		return S_FALSE;

	return S_OK;
}

LRESULT CDlgAttribute::OnEndEdit(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加控件通知处理程序代码

	return S_OK;
}
