#include "CommonFunc.h"
#include "DataDefine.h"
#include "DistEntityTool.h"
#include "DistSelSet.h"
#include "DistUiPrompts.h" 
#include "ARXPline.h" 
#include "Dlg_Attribute.h"

extern IDistConnection * Glb_pConnection;         //SDE���ݿ����Ӷ���ָ��
extern IDistExchangeData * Glb_pExchangeData;       //SDE<->CAD���ݽ�������ָ��

extern MapString2ContentTree Glb_mapTree; // ���ṹ���ձ�

extern ST_BIZZINFO Glb_BizzInfo; // ҵ����Ϣ

extern MapLong2Power Glb_mapPower; // ͼ��Ȩ�޶��ձ�

extern HWND Glb_hDapForm; // ������


extern BOOL bInitEnvironment;
//typedef BOOL (__stdcall * LoadFormItemText)(LPCTSTR lpDapPath, LPCTSTR lpBzCode);

void OutputDebugStringX(LPCTSTR fmt, ...)
{
	char lpBuffer[1024] = {0};

	va_list marker;
	va_start(marker, fmt);
	vsprintf(lpBuffer, fmt, marker);
	va_end(marker);

	OutputDebugString(lpBuffer);
}

void _activeCurrentCadView()
{
	if(acedGetAcadFrame() != NULL)
	{
		CFrameWnd	*pFrame;
		pFrame	= acedGetAcadFrame()->GetActiveFrame();

		if(pFrame != NULL)
		{
			CView	*pView;
			pView	= pFrame->GetActiveView();

			if(pView != NULL)
			{
				pView->SetFocus();
			}			
		}
	}
}

BOOL getViewExtent(AcGePoint3d &leftBottom , AcGePoint3d &rightTop)
{
	CView	*pView;
	pView	= acedGetAcadDwgView();
	if(pView == NULL) return FALSE;

	//��ȡ��ǰ��ͼ�Ŀͻ�����Χ
	CRect rc;
	pView->GetClientRect(&rc);
	CPoint pt1,pt2;

	pt1.x	= 0;
	pt1.y	= rc.bottom;
	pt2.x	= rc.right;
	pt2.y	= 0;

	//�������굽���������ת��
	acedDwgPoint outPt1,outPt2;
	acedCoordFromPixelToWorld(pt1 , outPt1);
	acedCoordFromPixelToWorld(pt2 , outPt2);

	//��������ĸ�ʽת��
	leftBottom	= asPnt3d(outPt1);
	rightTop	= asPnt3d(outPt2);

	return TRUE;
}

void fSetViewportToExtents(AcDbViewportTableRecord *&pActVp)
{
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

	//lets update the database extents first
	pDb->updateExt(false); //true gives the best fit but will take time

	double mHeight;
	AcGePoint2d mCentPt;

	//get the screen aspect ratio to calculate the height and width
	double mScrRatio;
	mScrRatio = (pActVp->width() / pActVp->height());//width/height

	//calculate the view height and center point
	double mWidth;
	AcGePoint3d mMaxExt = pDb->extmax(), mMinExt = pDb->extmin();

	//prepare Matrix for DCS to WCS transformation
	AcGeMatrix3d matWCS2DCS;
	matWCS2DCS.setToPlaneToWorld(pActVp->viewDirection());
	matWCS2DCS = AcGeMatrix3d::translation(pActVp->target() - AcGePoint3d::kOrigin) * matWCS2DCS; //for DCS target point is the origin
	matWCS2DCS = AcGeMatrix3d::rotation(-pActVp->viewTwist(), pActVp->viewDirection(),pActVp->target()) * matWCS2DCS;//WCS Xaxis is twisted by twist abgle
	matWCS2DCS = matWCS2DCS.inverse();

	//tranform the extents to the DCS defined by the viewdir
	AcDbExtents mExtents(pDb->extmin(),pDb->extmax());
	mExtents.transformBy(matWCS2DCS);

	//width of the extents in current view
	mWidth = (mExtents.maxPoint().x  - mExtents.minPoint().x);

	//height of the extents in current view
	mHeight = (mExtents.maxPoint().y  - mExtents.minPoint().y);

	//get the view center point
	mCentPt.x = (mExtents.maxPoint().x  + mExtents.minPoint().x)*0.5;
	mCentPt.y = (mExtents.maxPoint().y  + mExtents.minPoint().y)*0.5;

	//check if the width 'fits' in current window,if not then get the new height as per the viewports aspect ratio
	if (mWidth > (mHeight * mScrRatio))mHeight = mWidth / mScrRatio;

	//set the viewport parameters
	pActVp->setHeight(mHeight);    //set the view height
	pActVp->setWidth((mHeight * mScrRatio));
	pActVp->setCenterPoint(mCentPt); //set the view center

	return;
}

void fZoomExtents()
{
	Acad::ErrorStatus mEs;
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase(); 

	//update the view table records
	mEs = acedVports2VportTableRecords();

	//get the active viewport object
	AcDbViewportTable *pVpT = NULL;
	AcDbViewportTableRecord *pActVp = NULL;

	//get the *Active view port
	mEs = pDb->getViewportTable(pVpT,AcDb::kForRead);

	if (Acad::eOk != mEs)
	{
		return;
	}

	mEs = pVpT->getAt(_T("*Active"),pActVp,AcDb::kForWrite);
	pVpT->close();

	if (Acad::eOk != mEs)
	{
		return;
	}

	//set the viewport to extents along the current view dir

	fSetViewportToExtents(pActVp);

	//close the Active viewport
	pActVp->close();

	//update the viewport
	mEs = acedVportTableRecords2Vports();
}


void GetArxPath(LPTSTR lpPath, long nSize)
{
	GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpPath, nSize);
	PathRemoveFileSpec(lpPath);
}

//����˵��:
//source�� ��Ҫ�ָ��Դ�ַ�����
//dest�� �洢�ָ��ĸ����ַ���������Ŀ�������
//division���ָ��ַ�

void decodeCString(CString source, CStringArray& dest, char division)
{
	dest.RemoveAll();
	for(int i=0; i<source.GetLength(); i++)
	{
		if(source.GetAt(i)== division)
		{
			dest.Add(source.Left(i)); //ȥ���ұ�
			for(int j=0; j<(dest.GetSize()-1); j++)
			{
				dest[dest.GetSize()-1] = dest[dest.GetSize()-1].Right(dest[dest.GetSize()-1].GetLength()-dest[j].GetLength()-1); //ȥ�����
			}
		}
	}
}

BOOL InitEnvironment(LPCTSTR lpPath)
{
	if (lpPath[0] == '\0')
		return FALSE;

	// verify the path
	DWORD	dwType = GetFileAttributes(lpPath);
	if( dwType & FILE_ATTRIBUTE_DIRECTORY )
	{
		// get system path
		TCHAR lpEnvironment[32768];
		DWORD dwLen = GetEnvironmentVariable(TEXT("PATH"), lpEnvironment, 32767);

		// add ndapserv path to system path 
		TCHAR lpNewPath[32768];
		sprintf(lpNewPath, "%s;%s", lpPath, lpEnvironment);
		SetEnvironmentVariable(TEXT("PATH"), lpNewPath);
	}
	return TRUE;
}

void SendCommandToAutoCAD(CString strCommand)
{
	CWnd *pWnd = acedGetAcadDockCmdLine();
	if (NULL == pWnd)
		return;

	struct resbuf * rb_cmdecho_Off = acutBuildList(RTSHORT,0,0);
	struct resbuf * rb_cmdecho_On = acutBuildList(RTSHORT,1,0);

	OutputDebugStringX("acedSetVar : %s", "rb_cmdecho_Off");
	acedSetVar("cmdecho", rb_cmdecho_Off);

	OutputDebugString("pWnd->SendMessage(WM_CHAR,VK_ESCAPE,0)");

	pWnd->SendMessage(WM_CHAR,VK_ESCAPE,0);
	pWnd->SendMessage(WM_CHAR,VK_ESCAPE,0);

	OutputDebugStringX("command:%s;length:%d", strCommand, strCommand.GetLength());

	for (int i=0; i < strCommand.GetLength() ; i++)
	{
//		OutputDebugStringX("pWnd->SendMessage(WM_CHAR, %c)", strCommand.GetAt(i));
		pWnd->SendMessage(WM_CHAR, strCommand.GetAt(i), 0);
	}

	OutputDebugStringX("acedSetVar : %s", "rb_cmdecho_On");

	acedSetVar("cmdecho", rb_cmdecho_On);
	acutRelRb(rb_cmdecho_Off);
	acutRelRb(rb_cmdecho_On);
}

// ��CAD���ڷ�������
void SendCopyDataMessage(HWND hwnd, CString& strCommand, CStringArray& strParamArray)
{
	MSXML2::IXMLDOMDocumentPtr lpDocument;
	long hr = lpDocument.CreateInstance(__uuidof(DOMDocument));

	if ( FAILED(hr) ) 
		_com_raise_error(hr);

	MSXML2::IXMLDOMProcessingInstructionPtr lpInstruction = lpDocument->createProcessingInstruction("xml","version='1.0' encoding='GB2312'");
	lpDocument->appendChild(lpInstruction);

	MSXML2::IXMLDOMElementPtr lpRoot = lpDocument->createElement(LPCTSTR("root"));

	lpDocument->appendChild(lpRoot);

	// �������ͣ����������
	MSXML2::IXMLDOMElementPtr lpCommand = lpDocument->createElement("command");

	lpCommand->setAttribute("name", _variant_t(strCommand));

	lpRoot->appendChild(lpCommand);

	// �����б�
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
	::SendMessage(hwnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}

void ShowCommandLineWindow(BOOL bShow /*= TRUE*/)
{
	CWnd *pWnd = NULL;
	pWnd = acedGetAcadDockCmdLine();
	if(NULL == pWnd) return;
	pWnd = pWnd->GetParent();
	if(NULL == pWnd) return;
	pWnd = pWnd->GetParent();
	if(NULL == pWnd) return;

	if (bShow)
	{
		pWnd->ModifyStyle(0,WS_VISIBLE|WS_OVERLAPPED);
		pWnd->ShowWindow(SW_SHOWNORMAL);//SW_SHOW
	}
	else if (!bShow)
	{
		pWnd->ModifyStyle(WS_VISIBLE|WS_OVERLAPPED,0);
		pWnd->ShowWindow(SW_HIDE);
	}
	pWnd->RedrawWindow();
	acedGetAcadFrame()->RecalcLayout();
}

// Modify
//void SendDataToDapForm(IDistConnection * pConnection)
//{
//	// �õ�arx����·��
//	TCHAR lpPath[MAX_PATH] = {0};
//	GetModuleFileName(GetModuleHandle("shDistCommands.arx"), lpPath, MAX_PATH);
//	PathRemoveFileSpec(lpPath);
//	PathAddBackslash(lpPath);
//
//	CString strXmlFile = lpPath;
//	strXmlFile += "cad_formitem.xml";
//
//	HRESULT hr = S_OK;
//	MSXML2::IXMLDOMDocumentPtr lpDocument;
//	hr = lpDocument.CreateInstance(__uuidof(DOMDocument));
//
//	if ( FAILED(hr) ) 
//		_com_raise_error(hr);
//
//	// װ��XML�ַ���
//	if (!lpDocument->load(_variant_t(strXmlFile)))
//	{
//		OutputDebugString("LoadXML failed!");
//
//		return;
//	}
//
//	MSXML2::IXMLDOMNodeListPtr lpNodeList = lpDocument->getElementsByTagName(_bstr_t("bizz"));
//
//	MSXML2::IXMLDOMNodePtr lpBizzNode = NULL;
//	for ( ; (lpBizzNode = lpNodeList->nextNode()) != NULL ; )
//	{
//		MSXML2::IXMLDOMNodePtr lpChildNode = lpBizzNode->attributes->getNamedItem("name");
//		CString strBizzId = (char*)lpChildNode->text;
//
//		if (Glb_BizzInfo.strBzId.CompareNoCase(strBizzId) == 0)
//		{
//			OutputDebugString("getElementsByTagName");
//			OutputDebugString(strBizzId);
//			break;
//		}
//	}
//
//	if (NULL == lpBizzNode)
//	{
//		OutputDebugString("NULL == lpBizzNode");
//		return;
//	}
//
//	// ���������б�
//	MSXML2::IXMLDOMNodeListPtr lpChildNodeList = lpBizzNode->GetchildNodes();
//
//	MSXML2::IXMLDOMNodePtr lpParam = NULL;
//
//	IDistCommand *pCommand = CreateSDECommandObjcet();
//	if(NULL == pCommand) return;
//	pCommand->SetConnectObj(pConnection);
//
//	CString strCommand = "UpdateFormItemText";
//	CStringArray strParamArray;
//
//	for ( ; (lpParam = lpChildNodeList->nextNode()) != NULL ; )
//	{
//		MSXML2::IXMLDOMNodePtr lpNameNode = lpParam->attributes->getNamedItem("name");
//		CString strName = (char*)lpNameNode->text;
//
//		MSXML2::IXMLDOMNodePtr lpIdNode = lpParam->attributes->getNamedItem("id");
//		CString strId = (char*)lpIdNode->text;
//
//		MSXML2::IXMLDOMNodePtr lpTableNode = lpParam->attributes->getNamedItem("table");
//		CString strTable = (char*)lpTableNode->text;
//
//		MSXML2::IXMLDOMNodePtr lpFieldNode = lpParam->attributes->getNamedItem("field");
//		CString strField = (char*)lpFieldNode->text;
//
//		CStringArray tableArray;
//		decodeCString(strTable, tableArray, ';');
//
//		double dArea = 0.0;
//		for (int i=0; i<tableArray.GetCount(); i++)
//		{
//			CString strTableName = tableArray.GetAt(i);
//
//			TCHAR lpSQL[256] = {0};
//			sprintf(lpSQL, "F:%s,T:%s,W:DZDAH='%s',##", strField, strTableName, Glb_BizzInfo.strBizzSame);
//
//			OutputDebugString(lpSQL);
//
//			IDistRecordSet *pRcdSet = NULL;
//			if (pCommand->SelectData(lpSQL, &pRcdSet) != 1)
//			{
//				pRcdSet->Release();
//				continue;
//			}
//
//			while (pRcdSet->BatchRead() == 1)
//			{
//				dArea += *(double*)(*pRcdSet)[0];
//			}
//
//			pRcdSet->Release();
//
//			if (dArea > 0.0)
//				break;
//		}
//
//		CString strParam;
//		strParam.Format("%s=%lf", strId, dArea);
//		strParamArray.Add(strParam);
//	}
//		
//	pCommand->Release();
//
//	SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
//}

//BOOL GetFormItemText()
//{
//	TCHAR lpDapPath[MAX_PATH] = {0};
//	GetArxPath(lpDapPath, MAX_PATH);
//	PathRemoveFileSpec(lpDapPath);
//
//	if (!bInitEnvironment)
//	{
//		bInitEnvironment = InitEnvironment(lpDapPath);
//	}
//
//	TCHAR lpFileName[MAX_PATH] = {0};
//	GetArxPath(lpFileName, MAX_PATH);
//	PathAppend(lpFileName, "CadContainer.dll");
//
//	OutputDebugStringX("LoadFormItemText::%s", lpFileName);
//	//װ�ص�����
//	HMODULE hModule = LoadLibrary(lpFileName);
//	if (hModule == NULL)
//	{
//		DWORD dwErr = GetLastError();
//		OutputDebugString("LoadLibrary Failed!");
//		return FALSE;
//	}
//
//	//��ȡ����ָ��
//	LoadFormItemText lpFun =  NULL;
//	lpFun = (LoadFormItemText)GetProcAddress(hModule,_T("LoadFormItemText"));
//	if (lpFun == NULL)
//	{
//		OutputDebugString("GetProcAddress Failed!");
//		FreeLibrary(hModule);
//		return FALSE;
//	}
//
//	return lpFun(lpDapPath, Glb_BizzInfo.strBizzCode);
//}

// ����sdeͼ������ȡ�ռ�����
long ReadSdeLayer(IDistConnection* pConnection, IDistExchangeData* pExchangeData, LPCTSTR lpSdeLayer, DIST_STRUCT_SELECTWAY* pFilter, LPCTSTR lpWhere, BOOL bReadOnly, BOOL bDeleteExistEntity)
{
	CStringArray saFieldName, saAttItemName, saAttItemDesc;
	CArray<VARTYPE, VARTYPE> vaAttItemType;

	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(pConnection);

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	long lRt = pCmd->GetTableInfo(&pParam, &nParamNum, lpSdeLayer, NULL);
	if (lRt != 1)
	{
		pCmd->Release();
		acutPrintf("\n%s",ReturnErrorCodeInfo(lRt));

		return lRt;
	}

	for (int i = 0; i < nParamNum; i++)
	{
		CString sFieldName = pParam[i].fld_strName;
		CString sAliasName = pParam[i].fld_strAliasName;
		long lSdeType = pParam[i].fld_nType;
		VARTYPE va = VT_BSTR;
		if (lSdeType == kInt16 || lSdeType == kInt32)
		{
			va = VT_INT;
		}
		else if (lSdeType == kString || lSdeType == kNString)
		{
			va = VT_BSTR;
		}
		else if (lSdeType == kFloat32 || lSdeType == kFloat64)
		{
			va = VT_R8;
		}
		else if (lSdeType == kDate)
		{
			va = VT_DATE;
		}
		long lSize = pParam[i].fld_nSize;
		short nDec = pParam[i].fld_nDecimal;
		BOOL bNull = pParam[i].fld_bNullAllow;
		short nRowId = pParam[i].fld_nRowIdType;

		if (sFieldName.CollateNoCase("OBJECTID") == 0 ||
			sFieldName.CollateNoCase("SHAPE") == 0)
		{
			continue;
		}

		//AfxMessageBox(sAliasName);

		saFieldName.Add(sFieldName);
		saAttItemName.Add(sAliasName);
		sAliasName += "��������Ϣ";
		saAttItemDesc.Add(sAliasName);
		vaAttItemType.Add(va);
	}
	pCmd->Release();

	CStringArray saFieldName2;
	saFieldName2.Add("OBJECTID");
	saFieldName2.Append(saFieldName);

	CString strAppNme = "DIST_";
	strAppNme += lpSdeLayer;
	lRt = pExchangeData->ReadSymbolInfo(pConnection, NULL, NULL, lpSdeLayer, strAppNme, &saFieldName2);

	if (lRt != 1)
	{
		acutPrintf("\n%s",ReturnErrorCodeInfo(lRt)); return lRt;
	}

	AcDbObjectIdArray arrId;

	//��SDE�ռ�����
	lRt = pExchangeData->ReadSdeDBToCAD(&arrId, pFilter, lpWhere, TRUE, bReadOnly, bDeleteExistEntity);
	if (lRt != 1)
	{
		acutPrintf("\n%s",ReturnErrorCodeInfo(lRt)); return lRt;
	}

	return 1;
}

long SaveSdeLayer(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
								DIST_STRUCT_SELECTWAY* pFilter /* = NULL */,
								bool bAfterSaveToDelete /* = true */)
{
	long lRt = 0;

	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(pConnection);

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	lRt = pCmd->GetTableInfo(&pParam, &nParamNum, strSdeLyName, NULL);
	if (lRt != 1)
	{
		return lRt;
	}

	CStringArray saFieldName;

	for (int i = 0; i < nParamNum; i++)
	{
		CString sFieldName = pParam[i].fld_strName;

		if (sFieldName.CollateNoCase("SHAPE") == 0)
		{
			continue;
		}

		saFieldName.Add(sFieldName);
	}
	pCmd->Release();

	CString sAppName = strSdeLyName;
	sAppName = "DIST_" + sAppName;
	//sAppName = "DIST_XDATA";

	lRt = pExchangeData->ReadSymbolInfo(pConnection, NULL, NULL, strSdeLyName, sAppName, &saFieldName);
	if (lRt != 1)
	{
		return lRt;
	}

//	pExchangeData->SetBzInfo(Glb_BizzInfo.strBizzCode, Glb_BizzInfo.strBizzName, Glb_BizzInfo.strBzOwner);

	return pExchangeData->SaveEntityToSdeDB(pFilter, bAfterSaveToDelete);
}

void GetValueFromArray(CString& strValue,CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray)
{
	strValue.Empty();
	int nCount = strFldNameArray.GetSize();
	for(int i=0; i<nCount; i++)
	{
		if(strName.CompareNoCase(strFldNameArray.GetAt(i))==0)
		{
			strValue = strFldValueArray.GetAt(i);
			break;
		}
	}
}

#include "DistXData.h"
long DeleSdeData(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
				 AcDbObjectIdArray& objIds,
				 bool bAfterSaveToDelete /*= true*/)
{
	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(pConnection);

	for (int i=0; i<objIds.logicalLength(); i++)
	{
		AcDbObjectId objId = objIds.at(i);

		//ȡʵ���ڲ���չ������Ϣ
		CStringArray strFldNameArray,strFldValueArray;
		CDistXData tXData(objId, "ROWID_OBJS");
		tXData.Select(strFldNameArray,strFldValueArray);
		tXData.Close();//����ر�ʵ��

		long nRowId = 0;
		//��ȡ��չ������Ϣ�е�RowId
		CString strName,strValue;
		if(strFldValueArray.GetSize()>1)
		{
			GetValueFromArray(strValue,"OBJECTID",strFldNameArray,strFldValueArray);
			strValue.TrimLeft(); strValue.TrimRight();
			if(!strValue.IsEmpty())
				nRowId = atoi(strValue.GetBuffer(0));
		}

		if (nRowId <= 0)
			continue;

		TCHAR strSQL[256] = {0};
		sprintf(strSQL,"T:%s,W:%s=%d,##",strSdeLyName,"OBJECTID",nRowId);
		if (pCmd->DeleteData(strSQL) == 1)
		{
			if (bAfterSaveToDelete)
			{
				AcDbEntity * pEnt = NULL;
				if (ErrorStatus::eOk == acdbOpenObject(pEnt, objId, AcDb::kForWrite))
				{
					pEnt->erase();
					pEnt->close();         
				}
			}
		}
	}

	return 1;
}

long SaveSdeLayer(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
				  AcDbObjectIdArray& objIds,
				  bool bAfterSaveToDelete /* = true */)
{
	long lRt = 0;

	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(pConnection);

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	lRt = pCmd->GetTableInfo(&pParam, &nParamNum, strSdeLyName, NULL);
	if (lRt != 1)
	{
		return lRt;
	}

	CStringArray saFieldName;

	for (int i = 0; i < nParamNum; i++)
	{
		CString sFieldName = pParam[i].fld_strName;

		if (sFieldName.CollateNoCase("SHAPE") == 0)
		{
			continue;
		}

		saFieldName.Add(sFieldName);
	}
	pCmd->Release();

	CString sAppName = strSdeLyName;
	sAppName = "DIST_" + sAppName;
	//sAppName = "DIST_XDATA";

	lRt = pExchangeData->ReadSymbolInfo(pConnection, NULL, NULL, strSdeLyName, sAppName, &saFieldName);
	if (lRt != 1)
	{
		return lRt;
	}

	return pExchangeData->SaveEntityToSdeDB(objIds, bAfterSaveToDelete);

}

long SaveSdeLayerInsert(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
						AcDbObjectIdArray& objIds,
						bool bAfterSaveToDelete)
{
	long lRt = 0;

	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(pConnection);

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	lRt = pCmd->GetTableInfo(&pParam, &nParamNum, strSdeLyName, NULL);
	if (lRt != 1)
	{
		return lRt;
	}

	CStringArray saFieldName;

	for (int i = 0; i < nParamNum; i++)
	{
		CString sFieldName = pParam[i].fld_strName;

		if (sFieldName.CollateNoCase("SHAPE") == 0)
		{
			continue;
		}

		saFieldName.Add(sFieldName);
	}
	pCmd->Release();

	CString sAppName = strSdeLyName;
	sAppName = "DIST_" + sAppName;
	//sAppName = "DIST_XDATA";

	lRt = pExchangeData->ReadSymbolInfo(pConnection, NULL, NULL, strSdeLyName, sAppName, &saFieldName);
	if (lRt != 1)
	{
		return lRt;
	}

	return pExchangeData->SaveInsertEntityToSdeDB(objIds, bAfterSaveToDelete);

}

long SaveSdeLayerModify(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
						AcDbObjectIdArray& objIds,
						bool bAfterSaveToDelete)
{
	long lRt = 0;

	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(pConnection);

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	lRt = pCmd->GetTableInfo(&pParam, &nParamNum, strSdeLyName, NULL);
	if (lRt != 1)
	{
		return lRt;
	}

	CStringArray saFieldName;

	for (int i = 0; i < nParamNum; i++)
	{
		CString sFieldName = pParam[i].fld_strName;

		if (sFieldName.CollateNoCase("SHAPE") == 0)
		{
			continue;
		}

		saFieldName.Add(sFieldName);
	}
	pCmd->Release();

	CString sAppName = strSdeLyName;
	sAppName = "DIST_" + sAppName;
	//sAppName = "DIST_XDATA";

	lRt = pExchangeData->ReadSymbolInfo(pConnection, NULL, NULL, strSdeLyName, sAppName, &saFieldName);
	if (lRt != 1)
	{
		return lRt;
	}

	return pExchangeData->SaveModifyEntityToSdeDB(objIds, bAfterSaveToDelete);

}

long CopyRecordsToHistory(IDistConnection* pConnection, CUIntArray& rowIds, LPCTSTR lpSrcLayer, LPCTSTR lpDestlayer, LPCTSTR lpUserName, int nEditRowId)
{
	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(pConnection);

	return pCmd->CopyRecordsAcrossLayer(lpDestlayer, lpSrcLayer, rowIds, lpUserName, nEditRowId);
}

long SaveSdeLayer(IDistConnection* pConnection, IDistExchangeData* pExchangeData, const char* strSdeLyName,
				  AcDbObjectIdArray& objIds,CString strXMID,
				  bool bAfterSaveToDelete, /* = true */
				  bool bInsertOnly /*=false*/)
{
	long lRt = 0;

	IDistCommand* pCmd = CreateSDECommandObjcet();
	pCmd->SetConnectObj(pConnection);

	IDistParameter* pParam = NULL;
	int nParamNum = 0;
	lRt = pCmd->GetTableInfo(&pParam, &nParamNum, strSdeLyName, NULL);
	if (lRt != 1)
	{
		OutputDebugStringX("Error::SaveSdeLayer::pCmd->GetTableInfo");
		return lRt;
	}

	CStringArray saFieldName;

	for (int i = 0; i < nParamNum; i++)
	{
		CString sFieldName = pParam[i].fld_strName;

		if (sFieldName.CollateNoCase("SHAPE") == 0)
		{
			continue;
		}

		saFieldName.Add(sFieldName);
	}
	pCmd->Release();

	CString sAppName = strSdeLyName;
	sAppName = "DIST_" + sAppName;
	//sAppName = "DIST_XDATA";

	lRt = pExchangeData->ReadSymbolInfo(pConnection, NULL, NULL, strSdeLyName, sAppName, &saFieldName);
	if (lRt != 1)
	{
		OutputDebugStringX("pExchangeData->ReadSymbolInfo");
		return lRt;
	}

	if (bInsertOnly)
		return pExchangeData->SaveInsertEntityToSdeDB(objIds, bAfterSaveToDelete);
	else
		return pExchangeData->SaveEntityToSdeDB(objIds, strXMID, bAfterSaveToDelete);

}

//BOOL CheckTheDWGFileHasInsert(CString strDWGFileName)
//{
//	AcDbObjectId rBlockId;
//
//	if(!DocVars.docData().m_mapBlockIds.Lookup(strDWGFileName,rBlockId))
//		return FALSE;
//	
//	if(!rBlockId.isErased()) 
//		return TRUE;
//
//	//����Ѳ��룬���Ǳ�ɾ���ˣ���ԭ
//	AcDbBlockReference *pBlockReference;
//	if(acdbOpenObject(pBlockReference,rBlockId,AcDb::kForWrite,TRUE)!=Acad::eOk)
//		return FALSE;
//
//	pBlockReference->erase(false);
//	pBlockReference->close();
//
//	return TRUE;	
//}

// ��Ҫ���� : �鿴�Ƿ��Ѵ���ͬ���Ŀ�
BOOL CheckTheBlockInCurrentDB(LPCTSTR szBlockName, AcDbObjectId& BlkRecId)
{
	BlkRecId = NULL;
	AcDbBlockTable *pBlockTable;
	AcDbDatabase *pCurrentDB = acdbCurDwg();

	if (NULL == pCurrentDB)
		return FALSE;

	if(pCurrentDB->getBlockTable(pBlockTable,AcDb::kForRead) != Acad::eOk)
		return FALSE;

	Acad::ErrorStatus es = pBlockTable->getAt(szBlockName,BlkRecId);
	pBlockTable->close();

	if(es == Acad::eOk) 
		return TRUE;

	return FALSE;
}

// ��Ҫ���� : �������ݿ���ÿһ���������ɫΪͳһ����ɫ
BOOL SetEntityColorInDb(AcDbDatabase *pDb, int nColor)
{
/*	AcDbLayerTable *pLayerTable = NULL;
	AcDbLayerTableIterator * pLayerIt = NULL;
	AcDbLayerTableRecord * pLayerRec = NULL;

	if (pDb->getLayerTable(pLayerTable, AcDb::kForRead) != Acad::eOk)
	{
		OutputDebugString("SetEntityColorInDb::��ȡͼ�����Ϣʧ��!");
		return FALSE;
	}

	Acad::ErrorStatus es = pLayerTable->newIterator(pLayerIt);
	if (es != Acad::eOk)
	{
		pLayerTable->close();
		return FALSE;
	}
	for ( pLayerIt->start(); !pLayerIt->done(); pLayerIt->step() )
	{
		es = pLayerIt->getRecord(pLayerRec, AcDb::kForWrite);
		char* pStrName;
		es = pLayerRec->getName(pStrName);

		if (strcmpi(pStrName, "0") == 0)
		{
			pLayerRec->close();
			continue;
		}
		pLayerRec->erase(TRUE);
		pLayerRec->close();
	}

	delete pLayerIt;
	pLayerTable->close();
*/

	AcDbBlockTable *pBlockTable_Db;
	if (pDb->getBlockTable(pBlockTable_Db, AcDb::kForRead) != Acad::eOk)
	{
		OutputDebugString("SetEntityColorInDb::��ȡ�����Ϣʧ��!");
		return FALSE;
	}

	AcDbBlockTableRecord *pBlkTblRec_Db;
	if (pBlockTable_Db->getAt(ACDB_MODEL_SPACE, pBlkTblRec_Db, AcDb::kForWrite) != Acad::eOk)
	{
		OutputDebugString("SetEntityColorInDb::��ȡģ�Ϳռ����¼ʧ��!");
		return FALSE;
	}

	OutputDebugString("Begin SetEntityColorInBlock");
	SetEntityColorInBlock(pBlkTblRec_Db,nColor);
	OutputDebugString("End SetEntityColorInBlock");

	pBlkTblRec_Db->close();

	pBlockTable_Db->close();

	// ɾ������0���DIST_DXT֮�������ͼ��
	AcDbLayerTable* pLayerTable = NULL;
	
	Acad::ErrorStatus es = pDb->getLayerTable(pLayerTable, AcDb::kForRead);

	if (es != Acad::eOk)
		return TRUE;

	AcDbLayerTableIterator *pLayerIterator = NULL;

	if (pLayerTable->newIterator(pLayerIterator) != Acad::eOk)
	{
		pLayerTable->close();
		return TRUE;
	}

	for (; !pLayerIterator->done(); pLayerIterator->step())
	{
		AcDbLayerTableRecord *pLayerTableRecord = NULL;

		if (Acad::eOk == pLayerIterator->getRecord(pLayerTableRecord, AcDb::kForWrite))
		{
			char* pStrName;
			es = pLayerTableRecord->getName(pStrName);

			if (es != Acad::eOk)
			{
				pLayerTableRecord->close();
				continue;
			}

			if ((strcmpi(pStrName, "0") == 0) || (strcmpi(pStrName, "DIST_DXT") == 0))
			{
				pLayerTableRecord->close();
				continue;
			}
			pLayerTableRecord->erase(true);
		}

		pLayerTableRecord->close();
	}

	delete pLayerIterator;

	pLayerTable->close();

	return TRUE;
}

void DeleteAllEntityInBlock(AcDbBlockTableRecord* pBlkTblRec)
{
	AcDbBlockTableRecordIterator *pBlkTblRecIter;
	pBlkTblRec->newIterator(pBlkTblRecIter);

	for(pBlkTblRecIter->start();!pBlkTblRecIter->done();pBlkTblRecIter->step())
	{
		AcDbEntity *pEnt;
		if(pBlkTblRecIter->getEntity(pEnt,AcDb::kForWrite) != Acad::eOk)
			continue;

		pEnt->erase();
		pEnt->close();
	}

	delete pBlkTblRecIter;
}

// ��Ҫ���� : ����ĳһ������¼��ÿһ���������ɫΪͳһ����ɫ
void SetEntityColorInBlock(AcDbBlockTableRecord* pBlkTblRec, int nColor)
{
	AcDbBlockTableRecordIterator *pBlkTblRecIter;
	pBlkTblRec->newIterator(pBlkTblRecIter);

	for(pBlkTblRecIter->start();!pBlkTblRecIter->done();pBlkTblRecIter->step())
	{
		AcDbEntity *pEnt;
		if(pBlkTblRecIter->getEntity(pEnt,AcDb::kForWrite) != Acad::eOk)
			continue;

		char * pStrLayerName = pEnt->layer();

		// ȥ������ͼ�Դ���ͼ��
		if ((strcmpi(pStrLayerName, "12") == 0) ||
			(strcmpi(pStrLayerName, "TK") == 0) )
		{
			pEnt->erase();
			pEnt->close();
			continue;
		}

		pEnt->setColorIndex(nColor);
		pEnt->setLayer("DIST_DXT");

		if(pEnt->isKindOf(AcDb2dPolyline::desc())) // ��AcDb2dPolylineת��ΪAcDbPolyline��AcDb2dPolyline�ڲü������в�֧�֣�
		{
			AcDbPolyline * pTempline = new AcDbPolyline;

			pTempline->convertFrom(pEnt, Adesk::kFalse);

			// ���ñ��Ϊ0
			pTempline->setElevation(0);

			AcDbObjectId ObjId = NULL;
			Acad::ErrorStatus es = pBlkTblRec->appendAcDbEntity(ObjId, pTempline);
			pTempline->close();

			pEnt->erase();
			pEnt->close();
			continue;
		}

		if(pEnt->isKindOf(AcDbBlockReference::desc()))
		{
			AcDbBlockReference *pBlkRef = AcDbBlockReference::cast(pEnt);

			AcGePoint3d pt = pBlkRef->position();
			pt.z = 0.0;
			pBlkRef->setPosition(pt);

			AcDbBlockTableRecord *pBlkTblRec;
			if(acdbOpenObject(pBlkTblRec,pBlkRef->blockTableRecord(),AcDb::kForWrite) != Acad::eOk)
			{
				OutputDebugString("�򿪿���¼����");
			}
			else
			{
				OutputDebugString("Begin SetEntityColorInBlock");
				SetEntityColorInBlock(pBlkTblRec,nColor);
				OutputDebugString("End SetEntityColorInBlock");
				pBlkTblRec->close();
			}
		}

		pEnt->close();
	}

	delete pBlkTblRecIter;
}

// ����ͼ���ļ���������Ϊ�������뵱ǰͼ��
AcDbObjectId FindDWGAndInsertAsBlock(CString strFilePath,
									 CString strFileName,
									 AcGePoint3d geInsertPt,
									 AcGeScale3d geScaleFactors,
									 double dRotation,
									 int nColor,
									 CString strRefLayerName)
{
	LockDocument _Lock;

	//CString strBlockName=strFileName.Left(strFileName.GetLength()-4);
	CString strBlockName=strFileName;
	AcDbObjectId BlkRecId;
	if(!CheckTheBlockInCurrentDB(strBlockName.GetBuffer(),BlkRecId))
	{
		char dwgName[_MAX_PATH];
		strcpy(dwgName, strFilePath);
		PathAppend(dwgName, strFileName);
//		if (acDocManager->lockDocument(curDoc()) != Acad::eOk)
//			return NULL;

		AcDbDatabase *pDb=new AcDbDatabase(Adesk::kFalse);

		CString strErr;
		strErr.Format("��ʼ���ļ� %s", dwgName);
		OutputDebugString(strErr);

//		AfxMessageBox(dwgName);

		ErrorStatus es = pDb->readDwgFile(dwgName);
		if(es!=Acad::eOk)
		{
			acutPrintf("\n��ȡdwg�ļ�ʧ��!");
			OutputDebugString("��ȡdwg�ļ�ʧ�ܣ�");
//			acDocManager->unlockDocument(curDoc());
			return NULL;
		}
		OutputDebugString("���ļ����");

//		if (acDocManager->unlockDocument(curDoc()) != Acad::eOk)
//			return NULL;

//		Acad::ErrorStatus es;

		if (nColor > 0) // ����ͼ
		{
			AcDbObjectId resultId;
			CDistEntityTool tempTool;
			tempTool.CreateLayer(resultId,"DIST_DXT",nColor);

			AcDbLayerTable * pLyTable = NULL;
			AcDbLayerTableRecord * pLyRcd = NULL;
			es = pDb->getSymbolTable(pLyTable, AcDb::kForRead);
			if (es != ErrorStatus::eOk)
			{
				return NULL;
			}

			if (!pLyTable->has("DIST_DXT"))
			{
				AcDb::LineWeight dLineWeigth = AcDb::kLnWtByLwDefault;

				es = pLyTable->upgradeOpen();
				pLyRcd=new AcDbLayerTableRecord;
				
				if(NULL == pLyRcd) 
					return NULL;

				es = pLyRcd->setName("DIST_DXT");
				es = pLyRcd->setIsFrozen(0);
				if(es != Acad::eOk) 
				{	
					pLyTable->close(); 
					delete pLyRcd; 
					return NULL;
				}
				pLyRcd->setIsLocked(0);
				pLyRcd->setIsOff(0);
				pLyRcd->setVPDFLT(0);

				AcCmColor TempColor;
				TempColor.setColorIndex(nColor);
				pLyRcd->setColor(TempColor);
				es = pLyRcd->setLineWeight(dLineWeigth);

				es = pLyTable->add(resultId, pLyRcd);
				es = pLyRcd->close();
				es = pLyTable->close();
			}

			//�������ݿ���ÿһ���������ɫΪͳһ����ɫ
			if(!SetEntityColorInDb(pDb,nColor)) 
				return NULL;
		}

//		if (Acad::eOk!=acdbCurDwg()->insert(BlkRecId,strBlockName,pDb, true))
		es = acdbCurDwg()->insert(AcGeMatrix3d::kIdentity, pDb);
		if (es != Acad::eOk)
		{
			OutputDebugString("insert failed!/n����ʧ��" );
			delete pDb;
			return NULL;
		}
	//	pDb->closeInput(true);
		delete pDb;
	}

	// ������δ������ɴ�ӡ��ʱ��ֻ�ܴ�ӡ����ĵ�һ������ͼ���ڷ�Χ�ڵ�ʵ��
/*	AcDbBlockReference *pBlockReference=new AcDbBlockReference();
	pBlockReference->setBlockTableRecord(BlkRecId);
	pBlockReference->setPosition(geInsertPt);
	pBlockReference->setScaleFactors(geScaleFactors);
	pBlockReference->setRotation(dRotation);

	if(strRefLayerName.IsEmpty())	
		GetCurrentLayer(strRefLayerName);

	pBlockReference->setLayer(strRefLayerName);

	AcDbObjectId ReferenceId = AddEntToDWGDatabase(pBlockReference);

	pBlockReference->close();

	return ReferenceId;
*/
	AcDbObjectId ReferenceId;

	return ReferenceId;
}

// ��Ҫ���� : ��ȡ��ǰͼ��
BOOL GetCurrentLayer(CString& strCADLayer)
{
	LockDocument _Lock;

	char szBuf[256];
	struct resbuf *Value = acutBuildList(RTSTR,szBuf,0); 

	int err = acedGetVar("clayer", Value);

	BOOL bRet = TRUE;

	if(err == RTNORM){
		strCADLayer = Value->resval.rstring;
	}
	else{
		strCADLayer = "";
		acutPrintf("\n��ȡ��ǰͼ��ʧ��\n");
		bRet = FALSE;
	}

	acutRelRb(Value);

	return bRet;
}

void activeCurrentCadView()
{
	if(acedGetAcadFrame() != NULL)
	{
		CFrameWnd	*pFrame;
		pFrame	= acedGetAcadFrame()->GetActiveFrame();

		if(pFrame != NULL)
		{
			CView	*pView;
			pView	= pFrame->GetActiveView();

			if(pView != NULL)
			{
				pView->SetFocus();
			}			
		}
	}
}

//��ʵ����ӵ����ݿ���
AcDbObjectId AddEntToDWGDatabase(AcDbEntity *pEnt)
{
	Acad::ErrorStatus es;
	AcDbBlockTable *pBlockTable;
	es = acdbCurDwg()->getSymbolTable(pBlockTable,AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord;
	es = pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();

	AcDbObjectId ObjId = NULL;
	es = pBlockTableRecord->appendAcDbEntity(ObjId,pEnt);

	pBlockTableRecord->close();

	return ObjId;
}

BOOL SearchLinetype(AcDbDatabase *pDbDatab,char cLtName[],AcDbObjectId &DbOIdLtId)
{
	AcDbLinetypeTable		*pDbLtT;
	pDbDatab->getSymbolTable(pDbLtT, AcDb::kForRead);

	if(!pDbLtT->has(cLtName))	{
		pDbLtT->close();
		return FALSE;
	}else{
		pDbLtT->getAt(cLtName,DbOIdLtId);
		pDbLtT->close();
		return TRUE;
	}
}

BOOL AddLinetype(char cLtName[], AcDbObjectId &DbOIdLtId, AcDbDatabase * pDbDatab)
{
	AcDbLinetypeTable		* pDbLtT;
	Acad::ErrorStatus		AES;

	DbOIdLtId=NULL;
	AES=pDbDatab->loadLineTypeFile(cLtName, ".\\acadiso.lin");
	if( AES != Acad::eOk){
		acutPrintf("\nû�з���  %s ����",cLtName);
		return FALSE;
	}else{
		AES=pDbDatab->getLinetypeTable(pDbLtT, AcDb::kForRead);
		pDbLtT->close();
		if( AES == Acad::eOk){
			pDbLtT->getAt(cLtName,DbOIdLtId);
			return TRUE;
		}else{
			acutPrintf("\n�������� %s ʧ��",cLtName);
			return FALSE;
		}
	}
}

AcDbObjectId CreateLayer(char *cLyrName , Adesk::UInt16 Aun /* = 7 */, AcDb::LineWeight ALW /* = AcDb::kLnWtByLwDefault */,
						 char *cLtName /* = "Continuous" */, AcDbDatabase * pDbDatab /* = acdbHostApplicationServices */)
{
	AcDbLayerTable			* pDbLT;
	AcDbLayerTableRecord	* pDbLTR;
	AcDbObjectId			DbOIdLtId=NULL;
	AcDbObjectId			DbOIdLyId=NULL;
	AcCmColor				CmC;
	Acad::ErrorStatus		es;
	//
	pDbDatab->getSymbolTable(pDbLT, AcDb::kForRead);
	if(!pDbLT->has(cLyrName)){		
		//
		es = pDbLT->upgradeOpen();
		pDbLTR=new AcDbLayerTableRecord;
		es = pDbLTR->setName(cLyrName);
		es = pDbLTR->setIsFrozen(0);
		pDbLTR->setIsLocked(0);
		pDbLTR->setIsOff(0);
		pDbLTR->setVPDFLT(0);

		es = CmC.setColorIndex(Aun);
		pDbLTR->setColor(CmC);
		es = pDbLTR->setLineWeight(ALW);

		if(!SearchLinetype(pDbDatab,cLtName,DbOIdLtId)){
			AddLinetype(cLtName,DbOIdLtId,pDbDatab);
		}
		pDbLTR->setLinetypeObjectId(DbOIdLtId);
		es = pDbLT->add(DbOIdLyId,pDbLTR);
		es = pDbLTR->close();
		es = pDbLT->close();
	}
	else
	{
		//
		es = pDbLT->getAt(cLyrName,DbOIdLyId);//locate the record
		//es = pDbLT->getAt(cLyrName,pDbLTR,AcDb::kForWrite);//locate the record
		es = pDbLT->close();

		//es = CmC.setColorIndex(Aun);
		//pDbLTR->setColor(CmC);
		//es = pDbLTR->setLineWeight(ALW);

		//if(!SearchLinetype(pDbDatab,cLtName,DbOIdLtId)){
		//	AddLinetype(cLtName,DbOIdLtId,pDbDatab);
		//}
		//pDbLTR->setLinetypeObjectId(DbOIdLtId);
		//es = pDbLTR->close();
	}
	struct resbuf *Value=acutBuildList(RTSTR,cLyrName,0);

	if(acedSetVar("clayer", Value) != RTNORM)
	{
		acutRelRb(Value);
		return FALSE;
	}

	acutRelRb(Value);
	return DbOIdLyId;
}

BOOL ChangeEntityLayer(AcDbObjectId objId, char * pLayer)
{
	LockDocument _Lock;

	AcDbObjectId layerId = NULL;

	layerId = CreateLayer(pLayer);

	if (layerId == NULL)
	{
		return FALSE;
	}

	AcDbEntity * pEnt = NULL;

	if (Acad::eOk != acdbOpenObject(pEnt, objId, AcDb::kForWrite))
	{
		return FALSE;
	}

	pEnt->setLayer(pLayer);

	pEnt->close();

	return TRUE;
}

/*****************************************************
��Ҫ���� : ����ɾ��ͼ�����뵱ǰͼ��
������� : strFilePath[I]----ͼ���ļ���ŵ�·��
strFileName[I]----ͼ���ļ������ƣ���.tif��׺
geInsertPt[I]----�����
geScaleFactors[I]----�������
dRotation[I]----��ת�Ƕ�
nColor[I]----ʹ��ͬһ��ɫ,���������ԭ����ɫ
�� �� ֵ : ��/true-�ɹ�;false-ʧ��/TRUE-�ɹ�;FALSE-ʧ��
��    �� : 2007-1-10,Shanghai.
��    �� : �⾧<wujing@dist.com.cn>
�޸���־ : 
****************************************************/
AcDbObjectId FindTifAndInsertToCurrent(CString strFilePath,
									   CString strFileName,
									   AcGePoint3d geInsertPt,
									   AcGeScale3d geScaleFactors,
									   double dRotation,
									   int nColor,
									   AcGePoint2dArray ShpCornerPts)
{
	AcDbDictionary* pDictionary;
	Adesk::Boolean bNameInUse;
	AcDbObjectId DictionaryId;
	AcDbObjectId RasterImgDefId;
	CString KeyWord;
	CString strFilePathName;
	AcDbRasterImageDef* pAcDbRasterImageDef;

	ErrorStatus es;

	DictionaryId=AcDbRasterImageDef::imageDictionary(acdbCurDwg());
	if(DictionaryId.asOldId()==0) 
	{
		//�ôʵ仹δ����
		es = AcDbRasterImageDef::createImageDictionary(acdbCurDwg(),DictionaryId);
		int i=0;
	}
	es = acdbOpenObject((AcDbObject*&)pDictionary,DictionaryId,AcDb::kForWrite);
	
	strFilePathName=strFilePath;
	PathAppend(strFilePathName.GetBuffer(), strFileName.GetBuffer());
	strFilePathName.ReleaseBuffer();
	strFileName.ReleaseBuffer();

	KeyWord=strFileName.Left(strFileName.GetLength()-4);
	bNameInUse=pDictionary->has(KeyWord);
	if(!bNameInUse)
	{
		pAcDbRasterImageDef=new AcDbRasterImageDef;
		es = pAcDbRasterImageDef->setSourceFileName(strFilePathName);
		es = pAcDbRasterImageDef->load();
		es = pDictionary->setAt(KeyWord,pAcDbRasterImageDef,RasterImgDefId);
		pAcDbRasterImageDef->close();
	}
	else
		pDictionary->getAt(KeyWord,RasterImgDefId);
	pDictionary->close();
	AcDbRasterImage *pRasterImg=new AcDbRasterImage();
	AcGePoint3d OrgPt;
	AcGeVector3d CorVert,HigVert;
	OrgPt=AcGePoint3d(ShpCornerPts[0].x,ShpCornerPts[0].y,0);
	CorVert.x=ShpCornerPts[1].x-ShpCornerPts[0].x;
	CorVert.y=0;
	HigVert.x=0;
	HigVert.y=ShpCornerPts[1].y-ShpCornerPts[0].y;
	pRasterImg->setImageDefId(RasterImgDefId);
	pRasterImg->setOrientation(OrgPt,CorVert,HigVert);
	AcDbObjectId ImageId=AddEntToDWGDatabase(pRasterImg);
	pRasterImg->close();
	return ImageId;
}

/******************************************************************
������: setView
����: ������ͼ���൱��Zoom Window���
�������: Pt1 -- ��ͼ���Ͻǵ�
Pt2 -- ��ͼ���½ǵ�
ex_ratio -- ��չ����,һ��Ϊ1.0
�������:
����ֵ: void
����:
*****************************************************************/
void setView(AcGePoint3d Pt1, AcGePoint3d Pt2, double ex_ratio)
{
	AcGePoint2d CenterPt;
	//��X�����Y�����غϣ���Ϊ���⣬������SetView����
	if ((fabs(Pt1.x-Pt2.x)<1e-6)||(fabs(Pt1.y-Pt2.y)<1e-6))
		return;

	//ȷ�����������ֱ�Ϊ���ϽǺ����½�
	if (Pt1.x>Pt2.x) {
		double tmp;
		tmp = Pt1.x;
		Pt1.x = Pt2.x;
		Pt2.x = tmp;
	}

	if (Pt2.y>Pt1.y) {
		double tmp;
		tmp = Pt1.y;
		Pt1.y = Pt2.y;
		Pt2.y = tmp;
	}

	//��ȡ��ǰDwgView�ĳߴ�
	CRect CADrect;
	acedGetAcadDwgView()->GetClientRect(&CADrect);
	double width,height,ratio;
	ratio = (double)(CADrect.right-CADrect.left)/(double)(CADrect.bottom-CADrect.top);

	if (fabs(ratio)<1e-6)
		return;

	if ((Pt2.x-Pt1.x)/(Pt1.y-Pt2.y) > ratio) {
		width = Pt2.x-Pt1.x;
		height = width/ratio;
	}
	else{
		height = Pt1.y-Pt2.y;
		width = height * ratio;
	}

	//���õ�ǰ��ͼ���ĵ�
	CenterPt.x = (Pt1.x+Pt2.x)/2;
	CenterPt.y = (Pt1.y+Pt2.y)/2;

	//�ı䵱ǰ��ͼ
	AcDbViewTableRecord pVwRec;
	pVwRec.setCenterPoint(CenterPt);
	pVwRec.setWidth(width * ex_ratio);
	pVwRec.setHeight(height * ex_ratio);

	acedSetCurrentView( &pVwRec, NULL );
}

void SaveCurDWGtoFtp()
{
	//LockDocument _Lock;

	// ���ļ���������ʱĿ¼�²��ϴ���FTP
	TCHAR lpTempPath[MAX_PATH] = {0};
	TCHAR lpTempFile[MAX_PATH] = {0};

	GetTempPath(MAX_PATH, lpTempPath);
	lstrcpy(lpTempFile, lpTempPath);
	GetTempFileName(lpTempPath, "$SH", 0, lpTempFile);

	AcDbDatabase *pDb;
	pDb = acdbHostApplicationServices()->workingDatabase();
	Acad::ErrorStatus er = pDb->saveAs(lpTempFile);
	if (er != Acad::eOk)
	{
		acutPrintf("\n �洢��ǰͼ����\n");
		return;
	}

	// ���ļ�����һ�ݣ������ϴ�
	TCHAR lpDwgFile[MAX_PATH] = {0};
	if (!DocVars.docData().GetFileName().IsEmpty())
	{
		lstrcpy(lpDwgFile, DocVars.docData().GetFileName());
	}
	else
	{
		lstrcat(lpDwgFile, DocVars.docData().GetProjectId());
		if (DocVars.docData().GetFileId() != "")
		{
			lstrcat(lpDwgFile, "_");
			lstrcat(lpDwgFile, DocVars.docData().GetFileId());
		}
	}

	//��ʾ�û������ַ���
	TCHAR lpFileName[MAX_PATH] = {0};

	CString strPrompt;
	strPrompt.Format("\n�������ļ�����Ĭ���ļ���Ϊ: %s\n", lpDwgFile);
	int rc = acedGetString(0, strPrompt, lpFileName);

	if (rc != RTNORM)
		return;

	if (lpFileName[0] == '\0')
		lstrcpy(lpFileName, lpDwgFile);

	PathRenameExtension(lpFileName, ".dwg");
	acutPrintf("\n���ڱ��� %s", lpFileName);

	lstrcpy(lpDwgFile, lpTempPath);
	PathAppend(lpDwgFile, lpFileName);

	CopyFile(lpTempFile, lpDwgFile, FALSE);

	// ͨ��cadcontainerʵ��ʣ�๦�ܣ��ϴ�dwg�ļ���update���ݿ��
	CString strCommand = "UploadDwgFile";
	CStringArray strParamArray;

	CString strParam;
	strParam.Format("filename=%s", lpDwgFile);
	strParamArray.Add(strParam);

	SendCopyDataMessage(Glb_hDapForm, strCommand, strParamArray);
}

BOOL gCmdCreatePolyLine(CString strCadLyName, CString strLineName, AcDbObjectIdArray& objIds, BOOL bFilter, BOOL bMustClose)
{
	int nType=1;
	CString strPrompt;
	strPrompt.Format("\nѡ������ (1)�½�%s (2)ѡ��%s (3)ɾ��%s  <Ĭ��>", strLineName, strLineName, strLineName);
	CDistUiPrIntDef prIntType(strPrompt,NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return FALSE;
	nType = prIntType.GetValue();

	//�õ�����ʵ�壨����;����ѡ�񣬻��ƣ�
	AcDbObjectId objId;
	AcDbEntity * pEnt=NULL; 
	AcDbPolyline* pLine=NULL; 
	CDistEntityTool tempTool;

	//b.�½��߽�
	if(1 == nType)  
	{
		//////////////////////////////////////////////////////////////////////////
		ads_name lastEnt;
		AcDbObjectId lastEntId;// ���һ�β�����ʵ��ID
		// �õ����һ�β�����ʵ��ID
		acdbEntLast(lastEnt);
		acdbGetObjectId(lastEntId, lastEnt);

		AcDbObjectId entId;
		//////////////////////////////////////////////////////////////////////////
		ARXPline *pline = new ARXPline;
		pline->SetColorIndex(256); //ByLayer
		pline->draw();
		entId = pline->getObjectId();
		//acutPrintf("\n num Verts is %d",pline->numVerts);
		delete pline;
		//////////////////////////////////////////////////////////////////////////
		if (lastEntId == entId)
		{
			return FALSE;
		}
		objId = entId;

		if(objId.isNull()) return FALSE;

		//����ʵ��ID�޸���ͼ�����ƺ��߿�������ʾ
		if(Acad::eOk != acdbOpenObject(pEnt, objId,AcDb::kForWrite)) return FALSE;
		if(pEnt->isKindOf(AcDbPolyline::desc())==false)
		{
			pEnt->close(); return FALSE;
		}
		pLine = (AcDbPolyline*) pEnt;

		if(bMustClose && !pLine->isClosed())
		{
			CString strMessage;
			strMessage.Format("%sû�бպϣ��޷����棬�Ƿ��Զ��պϣ�", strLineName);
			if (AfxMessageBox(strMessage, MB_YESNO) != IDYES)
			{
				pLine->close();
				return FALSE;
			}

			pLine->setClosed(Adesk::kTrue);
		}

		pLine->setLayer(strCadLyName);

		pLine->close();

		objIds.append(objId);
	}
	//a.ѡ��߽�
	else if(2 == nType) 
	{
		objId.setNull();

		ads_point pt;  
		ads_name entName;

		//���ù�����
		struct resbuf filter;
		filter.restype=AcDb::kDxfLayerName;
		filter.resval.rstring = strLineName.GetBuffer(0);
		filter.rbnext=NULL;
		strLineName.ReleaseBuffer();

		ads_name ss;
		int result;
		
		if (bFilter)
			result = acedSSGet("_", NULL, NULL, &filter, ss);
		else
			result = acedSSGet("_", NULL, NULL, NULL, ss);

		if (result != RTNORM)
		{
			acedSSFree(ss);
			return FALSE;
		}
//		AcDbObjectIdArray objIds;

		long sslen;
		if (acedSSLength(ss, &sslen) != RTNORM) 
		{
			acedSSFree(ss);
			return FALSE;
		}

		int ret;
		ads_name ent;

		for (long i=0L; i<sslen; i++) 
		{
			ret = acedSSName(ss, i, ent);
			if (ret != RTNORM)
			{
				acutPrintf(_T("\nInvalid selection set."));
				break;
			}

			Acad::ErrorStatus es = acdbGetObjectId(objId, ent);
			if (es != Acad::eOk)
			{
				acedSSFree(ent);
				continue;
			}

			acedSSFree(ent);

			if(acdbOpenObject(pEnt, objId, AcDb::kForWrite)!=Acad::eOk) return FALSE;

			if((!pEnt->isKindOf(AcDbLine::desc())) &&
				(!pEnt->isKindOf(AcDb2dPolyline::desc())) &&
				(!pEnt->isKindOf(AcDb3dPolyline::desc())) &&
				(!pEnt->isKindOf(AcDbPolyline::desc())) &&
				(!pEnt->isKindOf(AcDbSpline::desc())) &&
				(!pEnt->isKindOf(AcDbEllipse::desc())) &&
				(!pEnt->isKindOf(AcDbArc::desc())) &&
				(!pEnt->isKindOf(AcDbCircle::desc())))
			{
				pEnt->close(); 
				continue;
			}

			pLine = (AcDbPolyline*) pEnt;
			if(bMustClose && !pLine->isClosed())
			{
				CString strMessage;
				strMessage.Format("%sû�бպϣ��޷����棬�Ƿ��Զ��պϣ�", strLineName);
				if (AfxMessageBox(strMessage, MB_YESNO) != IDYES)
				{
					pLine->close();
					return FALSE;
				}

				pLine->setClosed(Adesk::kTrue);
			}

			pEnt->setLayer(strCadLyName);
			pEnt->close();

			objIds.append(objId);
/*
			AcDbEntity * pNew = NULL;

			pNew = (AcDbEntity*)pEnt->clone();

			if (pNew == NULL)
			{
				acedSSFree(ss);
				return FALSE;
			}

			pNew->setColorIndex(256); //ByLayer

			objId=AddEntToDWGDatabase(pNew);

			pNew->setLayer(strCadLyName);

			pNew->close();
*/
		}

		acedSSFree(ss);

//		for (int i=0; i<objIds.logicalLength(); i++)
		{
		}
	}
	else if (3 == nType)
	{
		objId.setNull();
		AcDbObjectIdArray deleteObjIds;

		ads_point pt;  
		ads_name entName;

		//���ù�����
		struct resbuf filter;
		filter.restype=AcDb::kDxfLayerName;
		filter.resval.rstring = strCadLyName.GetBuffer(0);
		filter.rbnext=NULL;
		strCadLyName.ReleaseBuffer();

		ads_name ss;
		int result;

			result = acedSSGet("_", NULL, NULL, &filter, ss);

		if (result != RTNORM)
		{
			acedSSFree(ss);
			return FALSE;
		}

		long sslen;
		if (acedSSLength(ss, &sslen) != RTNORM) 
		{
			acedSSFree(ss);
			return FALSE;
		}

		ads_name ent;

		for (long i=0L; i<sslen; i++) 
		{
			int ret = acedSSName(ss, i, ent);
			if (ret != RTNORM)
			{
				acutPrintf(_T("\nѡ����Ч."));
				break;
			}

			Acad::ErrorStatus es = acdbGetObjectId(objId, ent);
			if (es != Acad::eOk)
			{
				acedSSFree(ent);
				continue;
			}

			deleteObjIds.append(objId);
			
			acedSSFree(ent);
		}

		acedSSFree(ss);

		IDistConnection* pConnection = Glb_pConnection;
		IDistExchangeData* pExchangeData = Glb_pExchangeData;

		STB_CONTENT_TREE xxContentTree = Glb_mapTree[strCadLyName];

		if (1 != DeleSdeData(pConnection, pExchangeData, xxContentTree.strSdeLyName, deleteObjIds, true))
		{
			acutPrintf("\nɾ��ʧ�ܣ�");
			return FALSE;
		}
		acutPrintf("\nɾ���ɹ���");
	}
	else
		return FALSE;

	return TRUE;
}

// ���ܣ�������Χ�߽߱�
// ������strLyName     ͼ������
//       strLineName   ��Χ������
// ���أ���
BOOL gCmdCreateBound(CString strCadLyName, CString strLineName, AcDbObjectId& ObjId)
{

	int nType=1;
	//����ѡ���������
	//CDistUiPrIntDef prIntType("\nѡ������ (1)��ȡ�߽� (2)ѡ��߽� (3)�½��߽�   Ĭ��",NULL,CDistUiPrInt::kRange,1);
	CString strPrompt;
	strPrompt.Format("\nѡ������ (1)��ȡ%s (2)ѡ��%s (3)�½�%s   Ĭ��", strLineName, strLineName, strLineName);
	CDistUiPrIntDef prIntType(strPrompt,NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return FALSE;
	nType = prIntType.GetValue();

	//�õ�����ʵ�壨����;������ȡ��ѡ�񣬻��ƣ�
//	AcDbObjectId ObjId;
	AcDbEntity * pEnt=NULL; 
	AcDbPolyline* pLine=NULL; 
	CDistEntityTool tempTool;
	//tempTool.CreateLayer(ObjId,strCadLyName,nColorIndex);
	//a.��ȡ�߽�
	if(1 == nType)  
	{
		ads_name en;
		if(acdbEntLast(en)!=RTNORM){  //��ȡ���ݿ������һ��ʵ��
			acutPrintf("\nû��ʵ�屻ѡ��"); return FALSE;
		}

		ads_point adsPt;
		int nResult = acedGetPoint(NULL,"\nѡ�������ڲ���:", adsPt);
		if(nResult != RTNORM) return FALSE;
		char strPt[40]={0};
		sprintf(strPt,"%0.3f,%0.3f",adsPt[0],adsPt[1]);
		if(acedCommand(RTSTR,"-BOUNDARY",RTSTR,strPt,RTSTR,"",RTNONE)!=RTNORM) return FALSE; //�����������ɱ߽�(RTSTR,PAUSE)
		while(acdbEntNext(en,en)==RTNORM)//��ȡ���ɵı߽�
		{
			ObjId.setNull();
			if(Acad::eOk != acdbGetObjectId(ObjId, en)) 
			{
				ObjId.setNull();
				return FALSE;  //�õ�ʵ��ObjectId
			}

			if(Acad::eOk != acdbOpenObject(pEnt, ObjId,AcDb::kForRead)) 
			{
				pEnt->close(); ObjId.setNull();
				return FALSE;
			}

			if(pEnt->isKindOf(AcDbPolyline::desc())==false)
			{
				pEnt->close(); continue;
			}
			else
			{
				pEnt->close(); break;
			}
		}
	}

	//b.ѡ��߽�
	else if(2 == nType) 
	{
		ObjId.setNull();

		ads_point pt;  
		ads_name entName;

		if(acedEntSel("\n��ѡ��һ���պϵ�ʵ�壺 ", entName, pt)!=RTNORM) return FALSE;
		if(acdbGetObjectId(ObjId, entName)!=Acad::eOk) return FALSE;
		if(acdbOpenObject(pEnt, ObjId, AcDb::kForRead)!=Acad::eOk) return FALSE;
		if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			pLine = (AcDbPolyline*) pEnt;
			if(!pLine->isClosed())
			{
				acutPrintf("\nѡ��ʵ�岻�պ�");
				pEnt->close(); return FALSE; 
			}
		}
		else
		{
			acutPrintf("\nѡ��ʵ�����Ͳ���Polyline��");
			pEnt->close(); return FALSE;
		}
		pEnt->close();

		AcDbEntity * pNew = NULL;

		pNew = (AcDbEntity*)pEnt->clone();

		if (pNew == NULL)
		{
			return FALSE;
		}

		//pNew->setLayer();

		pNew->setColorIndex(256); //ByLayer

		ObjId=AddEntToDWGDatabase(pNew);
		pNew->close();
	}

	//c.�½��߽�
	else if(3 == nType)  
	{
		//////////////////////////////////////////////////////////////////////////
		ads_name lastEnt;
		AcDbObjectId lastEntId;// ���һ�β�����ʵ��ID
		// �õ����һ�β�����ʵ��ID
		acdbEntLast(lastEnt);
		acdbGetObjectId(lastEntId, lastEnt);

		AcDbObjectId entId;
		//////////////////////////////////////////////////////////////////////////
		ARXPline *pline = new ARXPline;
		pline->SetColorIndex(256); //ByLayer
		pline->draw();
		entId = pline->getObjectId();
		//acutPrintf("\n num Verts is %d",pline->numVerts);
 		delete pline;
		//////////////////////////////////////////////////////////////////////////
		if (lastEntId == entId)
		{
			return FALSE;
		}
		ObjId = entId;
	}

	if(ObjId.isNull()) return FALSE;

	//����ʵ��ID�޸���ͼ�����ƺ��߿�������ʾ
	if(Acad::eOk != acdbOpenObject(pEnt, ObjId,AcDb::kForWrite)) return FALSE;
	if(pEnt->isKindOf(AcDbPolyline::desc())==false)
	{
		pEnt->close(); return FALSE;
	}
	pLine = (AcDbPolyline*) pEnt;
	if(pLine->isClosed()==false)
	{
//		acutPrintf("\n%sδ�պ�", strLineName);

		CString strMessage;
		strMessage.Format("%sû�бպϣ��޷����棬�Ƿ��Զ��պϣ�", strLineName);
		if (AfxMessageBox(strMessage, MB_YESNO) != IDYES)
		{
			pLine->erase();
			pLine->close();
			return FALSE;
		}

		pLine->setClosed(Adesk::kTrue);
	}

	//int nNumVert = pLine->numVerts(); //��ȡ������
	//for(int i=0; i<nNumVert; i++)
	//	pLine->setWidthsAt(i,dWidth,dWidth);

	//pLine->setColorIndex(nColorIndex);
	pLine->setLayer(strCadLyName);
	
	//double dArea =0.0;
	//pLine->getArea(dArea);
	//acutPrintf("\n��ǰ�ؿ����Ϊ��%0.2f ƽ����",dArea);

/*	if (!CheckPolylineSelfCross(pLine))
	{
		acutPrintf("\n%s���ཻ", strLineName);
		pLine->erase();
		pLine->close();
		return FALSE;
	}
*/
	pLine->close();
	return TRUE;
}

// ������Ŀ��Χ��
BOOL SaveXMFWX(LPCTSTR lpCadLayer, LPCTSTR lpSdeLayer, AcDbObjectId& objId)
{
	CDlgAttribute dlg;
	dlg.SetValues(lpCadLayer, lpSdeLayer, FALSE, objId);
	if (dlg.DoModal() != IDOK)
		return FALSE;

	return TRUE;
}

// ������Ŀ����
BOOL SaveXMHX(LPCTSTR lpCadLayer, LPCTSTR lpSdeLayer, AcDbObjectIdArray& objIds)
{
	CDlgAttribute dlg;
	dlg.SetValues(lpCadLayer, lpSdeLayer, TRUE, objIds);
	if (dlg.DoModal() != IDOK)
		return FALSE;

	return TRUE;
}

//�ж��Ƿ����ཻ
BOOL CheckPolylineSelfCross(AcDbPolyline * pPline)
{
	if (!pPline->isClosed())
	{
		return FALSE;
	}

	pPline->close();

	//���ܹ���ıպ����������ཻ��
	AcDbVoidPtrArray curveSegments;

	curveSegments.append(pPline);

	AcDbVoidPtrArray regions;

	AcDbRegion *pRegion;

	if (pRegion->createFromCurves(curveSegments, regions) != Acad::eOk)
	{
		return FALSE;
	}

	return TRUE;
}