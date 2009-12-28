#include "CommonFunc.h"
#include "DataDefine.h"
#include "DistEntityTool.h"
#include "DistSelSet.h"
#include "DistUiPrompts.h" 
#include "ARXPline.h" 
#include "Dlg_Attribute.h"

extern IDistConnection * Glb_pConnection;         //SDE数据库连接对象指针
extern IDistExchangeData * Glb_pExchangeData;       //SDE<->CAD数据交换对象指针

extern MapString2ContentTree Glb_mapTree; // 树结构对照表

extern ST_BIZZINFO Glb_BizzInfo; // 业务信息

extern MapLong2Power Glb_mapPower; // 图层权限对照表

extern HWND Glb_hDapForm; // 父窗体


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

	//获取当前视图的客户区范围
	CRect rc;
	pView->GetClientRect(&rc);
	CPoint pt1,pt2;

	pt1.x	= 0;
	pt1.y	= rc.bottom;
	pt2.x	= rc.right;
	pt2.y	= 0;

	//象素坐标到世界坐标的转换
	acedDwgPoint outPt1,outPt2;
	acedCoordFromPixelToWorld(pt1 , outPt1);
	acedCoordFromPixelToWorld(pt2 , outPt2);

	//世界坐标的格式转换
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

//参数说明:
//source： 需要分割的源字符串；
//dest： 存储分割后的各个字符串变量的目标变量；
//division：分割字符

void decodeCString(CString source, CStringArray& dest, char division)
{
	dest.RemoveAll();
	for(int i=0; i<source.GetLength(); i++)
	{
		if(source.GetAt(i)== division)
		{
			dest.Add(source.Left(i)); //去掉右边
			for(int j=0; j<(dest.GetSize()-1); j++)
			{
				dest[dest.GetSize()-1] = dest[dest.GetSize()-1].Right(dest[dest.GetSize()-1].GetLength()-dest[j].GetLength()-1); //去掉左边
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

// 向CAD窗口发送命令
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
//	// 得到arx所在路径
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
//	// 装载XML字符串
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
//	// 解析参数列表
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
//	//装载档案库
//	HMODULE hModule = LoadLibrary(lpFileName);
//	if (hModule == NULL)
//	{
//		DWORD dwErr = GetLastError();
//		OutputDebugString("LoadLibrary Failed!");
//		return FALSE;
//	}
//
//	//获取函数指针
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

// 按照sde图层名读取空间数据
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
		sAliasName += "的描述信息";
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

	//读SDE空间数据
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

		//取实体内部扩展属性信息
		CStringArray strFldNameArray,strFldValueArray;
		CDistXData tXData(objId, "ROWID_OBJS");
		tXData.Select(strFldNameArray,strFldValueArray);
		tXData.Close();//尽快关闭实体

		long nRowId = 0;
		//提取扩展属性信息中的RowId
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
//	//如果已插入，但是被删除了，则复原
//	AcDbBlockReference *pBlockReference;
//	if(acdbOpenObject(pBlockReference,rBlockId,AcDb::kForWrite,TRUE)!=Acad::eOk)
//		return FALSE;
//
//	pBlockReference->erase(false);
//	pBlockReference->close();
//
//	return TRUE;	
//}

// 简要描述 : 查看是否已存在同名的块
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

// 简要描述 : 设置数据库中每一个对象的颜色为统一的颜色
BOOL SetEntityColorInDb(AcDbDatabase *pDb, int nColor)
{
/*	AcDbLayerTable *pLayerTable = NULL;
	AcDbLayerTableIterator * pLayerIt = NULL;
	AcDbLayerTableRecord * pLayerRec = NULL;

	if (pDb->getLayerTable(pLayerTable, AcDb::kForRead) != Acad::eOk)
	{
		OutputDebugString("SetEntityColorInDb::获取图层表信息失败!");
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
		OutputDebugString("SetEntityColorInDb::获取块表信息失败!");
		return FALSE;
	}

	AcDbBlockTableRecord *pBlkTblRec_Db;
	if (pBlockTable_Db->getAt(ACDB_MODEL_SPACE, pBlkTblRec_Db, AcDb::kForWrite) != Acad::eOk)
	{
		OutputDebugString("SetEntityColorInDb::获取模型空间块表记录失败!");
		return FALSE;
	}

	OutputDebugString("Begin SetEntityColorInBlock");
	SetEntityColorInBlock(pBlkTblRec_Db,nColor);
	OutputDebugString("End SetEntityColorInBlock");

	pBlkTblRec_Db->close();

	pBlockTable_Db->close();

	// 删除除了0层和DIST_DXT之外的所有图层
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

// 简要描述 : 设置某一个块表记录中每一个对象的颜色为统一的颜色
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

		// 去掉地形图自带的图框
		if ((strcmpi(pStrLayerName, "12") == 0) ||
			(strcmpi(pStrLayerName, "TK") == 0) )
		{
			pEnt->erase();
			pEnt->close();
			continue;
		}

		pEnt->setColorIndex(nColor);
		pEnt->setLayer("DIST_DXT");

		if(pEnt->isKindOf(AcDb2dPolyline::desc())) // 将AcDb2dPolyline转换为AcDbPolyline（AcDb2dPolyline在裁剪函数中不支持）
		{
			AcDbPolyline * pTempline = new AcDbPolyline;

			pTempline->convertFrom(pEnt, Adesk::kFalse);

			// 设置标高为0
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
				OutputDebugString("打开块表记录出错");
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

// 查找图形文件并将其作为块对象插入当前图面
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
		strErr.Format("开始读文件 %s", dwgName);
		OutputDebugString(strErr);

//		AfxMessageBox(dwgName);

		ErrorStatus es = pDb->readDwgFile(dwgName);
		if(es!=Acad::eOk)
		{
			acutPrintf("\n读取dwg文件失败!");
			OutputDebugString("读取dwg文件失败！");
//			acDocManager->unlockDocument(curDoc());
			return NULL;
		}
		OutputDebugString("读文件完毕");

//		if (acDocManager->unlockDocument(curDoc()) != Acad::eOk)
//			return NULL;

//		Acad::ErrorStatus es;

		if (nColor > 0) // 地形图
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

			//设置数据库中每一个对象的颜色为统一的颜色
			if(!SetEntityColorInDb(pDb,nColor)) 
				return NULL;
		}

//		if (Acad::eOk!=acdbCurDwg()->insert(BlkRecId,strBlockName,pDb, true))
		es = acdbCurDwg()->insert(AcGeMatrix3d::kIdentity, pDb);
		if (es != Acad::eOk)
		{
			OutputDebugString("insert failed!/n插入失败" );
			delete pDb;
			return NULL;
		}
	//	pDb->closeInput(true);
		delete pDb;
	}

	// 下面这段代码会造成打印的时候只能打印插入的第一幅地形图所在范围内的实体
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

// 简要描述 : 获取当前图层
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
		acutPrintf("\n获取当前图层失败\n");
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

//将实体添加到数据库中
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
		acutPrintf("\n没有发现  %s 线型",cLtName);
		return FALSE;
	}else{
		AES=pDbDatab->getLinetypeTable(pDbLtT, AcDb::kForRead);
		pDbLtT->close();
		if( AES == Acad::eOk){
			pDbLtT->getAt(cLtName,DbOIdLtId);
			return TRUE;
		}else{
			acutPrintf("\n加载线性 %s 失败",cLtName);
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
简要描述 : 查找删格图并插入当前图面
输入参数 : strFilePath[I]----图形文件存放的路径
strFileName[I]----图形文件的名称，带.tif后缀
geInsertPt[I]----插入点
geScaleFactors[I]----拉伸比例
dRotation[I]----旋转角度
nColor[I]----使用同一颜色,负数则采用原有颜色
返 回 值 : 无/true-成功;false-失败/TRUE-成功;FALSE-失败
日    期 : 2007-1-10,Shanghai.
作    者 : 吴晶<wujing@dist.com.cn>
修改日志 : 
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
		//该词典还未创建
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
函数名: setView
功能: 设置视图（相当于Zoom Window命令）
输入参数: Pt1 -- 视图左上角点
Pt2 -- 视图右下角点
ex_ratio -- 扩展比率,一般为1.0
输出参数:
返回值: void
其它:
*****************************************************************/
void setView(AcGePoint3d Pt1, AcGePoint3d Pt2, double ex_ratio)
{
	AcGePoint2d CenterPt;
	//若X坐标或Y坐标重合，判为意外，不进行SetView操作
	if ((fabs(Pt1.x-Pt2.x)<1e-6)||(fabs(Pt1.y-Pt2.y)<1e-6))
		return;

	//确保两个坐标点分别为左上角和右下角
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

	//获取当前DwgView的尺寸
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

	//设置当前视图中心点
	CenterPt.x = (Pt1.x+Pt2.x)/2;
	CenterPt.y = (Pt1.y+Pt2.y)/2;

	//改变当前视图
	AcDbViewTableRecord pVwRec;
	pVwRec.setCenterPoint(CenterPt);
	pVwRec.setWidth(width * ex_ratio);
	pVwRec.setHeight(height * ex_ratio);

	acedSetCurrentView( &pVwRec, NULL );
}

void SaveCurDWGtoFtp()
{
	//LockDocument _Lock;

	// 将文件保存至临时目录下并上传至FTP
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
		acutPrintf("\n 存储当前图出错！\n");
		return;
	}

	// 将文件复制一份，用于上传
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

	//提示用户输入字符串
	TCHAR lpFileName[MAX_PATH] = {0};

	CString strPrompt;
	strPrompt.Format("\n请输入文件名，默认文件名为: %s\n", lpDwgFile);
	int rc = acedGetString(0, strPrompt, lpFileName);

	if (rc != RTNORM)
		return;

	if (lpFileName[0] == '\0')
		lstrcpy(lpFileName, lpDwgFile);

	PathRenameExtension(lpFileName, ".dwg");
	acutPrintf("\n正在保存 %s", lpFileName);

	lstrcpy(lpDwgFile, lpTempPath);
	PathAppend(lpDwgFile, lpFileName);

	CopyFile(lpTempFile, lpDwgFile, FALSE);

	// 通过cadcontainer实现剩余功能（上传dwg文件，update数据库表）
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
	strPrompt.Format("\n选择类型 (1)新建%s (2)选择%s (3)删除%s  <默认>", strLineName, strLineName, strLineName);
	CDistUiPrIntDef prIntType(strPrompt,NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return FALSE;
	nType = prIntType.GetValue();

	//得到蓝线实体（两个途径：选择，绘制）
	AcDbObjectId objId;
	AcDbEntity * pEnt=NULL; 
	AcDbPolyline* pLine=NULL; 
	CDistEntityTool tempTool;

	//b.新建边界
	if(1 == nType)  
	{
		//////////////////////////////////////////////////////////////////////////
		ads_name lastEnt;
		AcDbObjectId lastEntId;// 最后一次产生的实体ID
		// 得到最后一次产生的实体ID
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

		//根据实体ID修改其图层名称和线宽并高亮显示
		if(Acad::eOk != acdbOpenObject(pEnt, objId,AcDb::kForWrite)) return FALSE;
		if(pEnt->isKindOf(AcDbPolyline::desc())==false)
		{
			pEnt->close(); return FALSE;
		}
		pLine = (AcDbPolyline*) pEnt;

		if(bMustClose && !pLine->isClosed())
		{
			CString strMessage;
			strMessage.Format("%s没有闭合，无法保存，是否自动闭合？", strLineName);
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
	//a.选择边界
	else if(2 == nType) 
	{
		objId.setNull();

		ads_point pt;  
		ads_name entName;

		//设置过虑器
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
				strMessage.Format("%s没有闭合，无法保存，是否自动闭合？", strLineName);
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

		//设置过虑器
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
				acutPrintf(_T("\n选择集无效."));
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
			acutPrintf("\n删除失败！");
			return FALSE;
		}
		acutPrintf("\n删除成功！");
	}
	else
		return FALSE;

	return TRUE;
}

// 功能：创建范围线边界
// 参数：strLyName     图层名称
//       strLineName   范围线名称
// 返回：无
BOOL gCmdCreateBound(CString strCadLyName, CString strLineName, AcDbObjectId& ObjId)
{

	int nType=1;
	//交互选择操作类型
	//CDistUiPrIntDef prIntType("\n选择类型 (1)提取边界 (2)选择边界 (3)新建边界   默认",NULL,CDistUiPrInt::kRange,1);
	CString strPrompt;
	strPrompt.Format("\n选择类型 (1)提取%s (2)选择%s (3)新建%s   默认", strLineName, strLineName, strLineName);
	CDistUiPrIntDef prIntType(strPrompt,NULL,CDistUiPrInt::kRange,1); 
	prIntType.SetRange(1,3);
	if(prIntType.Go()!=CDistUiPrBase::kOk) return FALSE;
	nType = prIntType.GetValue();

	//得到蓝线实体（三个途径：提取，选择，绘制）
//	AcDbObjectId ObjId;
	AcDbEntity * pEnt=NULL; 
	AcDbPolyline* pLine=NULL; 
	CDistEntityTool tempTool;
	//tempTool.CreateLayer(ObjId,strCadLyName,nColorIndex);
	//a.提取边界
	if(1 == nType)  
	{
		ads_name en;
		if(acdbEntLast(en)!=RTNORM){  //获取数据库中最后一个实体
			acutPrintf("\n没有实体被选择！"); return FALSE;
		}

		ads_point adsPt;
		int nResult = acedGetPoint(NULL,"\n选择多边形内部点:", adsPt);
		if(nResult != RTNORM) return FALSE;
		char strPt[40]={0};
		sprintf(strPt,"%0.3f,%0.3f",adsPt[0],adsPt[1]);
		if(acedCommand(RTSTR,"-BOUNDARY",RTSTR,strPt,RTSTR,"",RTNONE)!=RTNORM) return FALSE; //调用命令生成边界(RTSTR,PAUSE)
		while(acdbEntNext(en,en)==RTNORM)//获取生成的边界
		{
			ObjId.setNull();
			if(Acad::eOk != acdbGetObjectId(ObjId, en)) 
			{
				ObjId.setNull();
				return FALSE;  //得到实体ObjectId
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

	//b.选择边界
	else if(2 == nType) 
	{
		ObjId.setNull();

		ads_point pt;  
		ads_name entName;

		if(acedEntSel("\n请选择一个闭合的实体： ", entName, pt)!=RTNORM) return FALSE;
		if(acdbGetObjectId(ObjId, entName)!=Acad::eOk) return FALSE;
		if(acdbOpenObject(pEnt, ObjId, AcDb::kForRead)!=Acad::eOk) return FALSE;
		if(pEnt->isKindOf(AcDbPolyline::desc()))
		{
			pLine = (AcDbPolyline*) pEnt;
			if(!pLine->isClosed())
			{
				acutPrintf("\n选择实体不闭合");
				pEnt->close(); return FALSE; 
			}
		}
		else
		{
			acutPrintf("\n选择实体类型不是Polyline线");
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

	//c.新建边界
	else if(3 == nType)  
	{
		//////////////////////////////////////////////////////////////////////////
		ads_name lastEnt;
		AcDbObjectId lastEntId;// 最后一次产生的实体ID
		// 得到最后一次产生的实体ID
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

	//根据实体ID修改其图层名称和线宽并高亮显示
	if(Acad::eOk != acdbOpenObject(pEnt, ObjId,AcDb::kForWrite)) return FALSE;
	if(pEnt->isKindOf(AcDbPolyline::desc())==false)
	{
		pEnt->close(); return FALSE;
	}
	pLine = (AcDbPolyline*) pEnt;
	if(pLine->isClosed()==false)
	{
//		acutPrintf("\n%s未闭合", strLineName);

		CString strMessage;
		strMessage.Format("%s没有闭合，无法保存，是否自动闭合？", strLineName);
		if (AfxMessageBox(strMessage, MB_YESNO) != IDYES)
		{
			pLine->erase();
			pLine->close();
			return FALSE;
		}

		pLine->setClosed(Adesk::kTrue);
	}

	//int nNumVert = pLine->numVerts(); //获取点数量
	//for(int i=0; i<nNumVert; i++)
	//	pLine->setWidthsAt(i,dWidth,dWidth);

	//pLine->setColorIndex(nColorIndex);
	pLine->setLayer(strCadLyName);
	
	//double dArea =0.0;
	//pLine->getArea(dArea);
	//acutPrintf("\n当前地块面积为：%0.2f 平方米",dArea);

/*	if (!CheckPolylineSelfCross(pLine))
	{
		acutPrintf("\n%s自相交", strLineName);
		pLine->erase();
		pLine->close();
		return FALSE;
	}
*/
	pLine->close();
	return TRUE;
}

// 保存项目范围线
BOOL SaveXMFWX(LPCTSTR lpCadLayer, LPCTSTR lpSdeLayer, AcDbObjectId& objId)
{
	CDlgAttribute dlg;
	dlg.SetValues(lpCadLayer, lpSdeLayer, FALSE, objId);
	if (dlg.DoModal() != IDOK)
		return FALSE;

	return TRUE;
}

// 保存项目红线
BOOL SaveXMHX(LPCTSTR lpCadLayer, LPCTSTR lpSdeLayer, AcDbObjectIdArray& objIds)
{
	CDlgAttribute dlg;
	dlg.SetValues(lpCadLayer, lpSdeLayer, TRUE, objIds);
	if (dlg.DoModal() != IDOK)
		return FALSE;

	return TRUE;
}

//判断是否自相交
BOOL CheckPolylineSelfCross(AcDbPolyline * pPline)
{
	if (!pPline->isClosed())
	{
		return FALSE;
	}

	pPline->close();

	//不能构面的闭合曲线是自相交的
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