#include "StdAfx.h"
#include "DistToolbar.h"
#include "INISetting.h"

#include "acadi.h"
#include "acadi_i.c"

CStringArray g_saGroupName;

bool getApplication(LPDISPATCH * pVal)
{
	LPDISPATCH pDispatch = acedGetAcadWinApp()->GetIDispatch(TRUE);
	if (pDispatch == NULL)
		return false;
	*pVal = pDispatch;
	return true;
}

bool getAcadMenuGroup(IAcadMenuGroup  **pVal)
{
	IAcadApplication *acadApp = NULL;
	LPDISPATCH  pDisp = NULL;

	if (!getApplication(&pDisp))
		return false;

	HRESULT hr = S_OK;
	hr = pDisp->QueryInterface(IID_IAcadApplication, (LPVOID*)&acadApp);
	if (FAILED(hr))
		return false;

	LPDISPATCH  pTempDisp = NULL;
	IAcadMenuGroups *mnuGrps = NULL;
	long cnt = 0;

	//get the menu groups
	hr = acadApp->get_MenuGroups(&mnuGrps);
	if (FAILED(hr))
	{
		acadApp->Release();
		return false;
	}
	mnuGrps->get_Count(&cnt);

	//get AutoCAD menu group. say it is index 0.
	IAcadMenuGroup *mnuGrp = NULL;

	VARIANT  vtName;
	vtName.vt = VT_I4;
	BSTR  grpName;
	bool found = false ;
	for (long i=0; i < cnt; i++)
	{
		vtName.lVal = i;
		hr = mnuGrps->Item(vtName, &mnuGrp);
		if (FAILED(hr))
			return false;

		hr  = mnuGrp->get_Name(&grpName);
		CString cgrpName(grpName);
		if (cgrpName.CompareNoCase("Acad")==0) 
		{
			found = true;
			*pVal = mnuGrp;
			break;
		}
	}

	acadApp->Release();
	return found;
}

void CreateToolbars()
{
	IAcadMenuGroup *mnuGrp = NULL;
	if (!getAcadMenuGroup(&mnuGrp))
		return ;

	IAcadToolbars  *tlbrs = NULL;
	HRESULT hr = S_OK;
	hr = mnuGrp->get_Toolbars(&tlbrs);
	mnuGrp->Release();

	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(GetModuleHandle("shDistToolBarManager.arx"),szFileName,MAX_PATH);
	CString csPath(szFileName);
	csPath=csPath.Left(csPath.ReverseFind('\\')+1)+"Picture\\";

	INISetting ToolbarsCfgIni;
	if (!GetINISetting("ToolbarsCFG.ini", ToolbarsCfgIni))
	{
		return;
	}

	CStringArray saToolbars;
	ToolbarsCfgIni.GetAllAppName(saToolbars);

	for (int i = 0; i < saToolbars.GetCount(); i++)
	{
		CString sToolbarName = saToolbars.GetAt(i);

		CStringArray saBtnName, saBtnCmd;
		if (0 == ToolbarsCfgIni.GetAllKeysAndValues(sToolbarName, saBtnName, saBtnCmd))
		{
			continue;
		}

		IAcadToolbar  *tlbr = NULL;
		hr = tlbrs->Add(CComBSTR(sToolbarName), &tlbr);
		if FAILED(hr)
			continue;

		g_saGroupName.Add(sToolbarName);

		IAcadToolbarItem *button=NULL;
		VARIANT index;
		index.vt = VT_I4;
		index.lVal = 100l;

		VARIANT vtFalse;
		vtFalse.vt = VT_BOOL;
		vtFalse.boolVal = VARIANT_FALSE;

		for (int j = 0; j < saBtnName.GetCount(); j++)
		{
			CString sName = saBtnName.GetAt(j);

			CString sDes = sName;

			CString sBmpName = sName;

			CString sCmd = saBtnCmd.GetAt(j) + " ";

			CString sIcoName = csPath + sBmpName + ".ico";

			if (!FileExists(sIcoName))
			{
				sIcoName = csPath+"dist.ico";
			}

			if(!FileExists(sIcoName)) continue;

			hr = tlbr->AddToolbarButton(index, CComBSTR(sDes),CComBSTR(sDes), CComBSTR(sCmd), vtFalse, &button);

			hr = button->SetBitmaps(CComBSTR(sIcoName),CComBSTR(sIcoName));

			button->Release();
		}

		tlbr->Dock(acToolbarDockRight);//acToolbarFloating
		//tlbr->Dock(acToolbarDockBottom);
		tlbr->Release();
	}

	tlbrs->Release();

	return;
}

void CleanToolbars()
{
	IAcadMenuGroup *mnuGrp = NULL;
	if (!getAcadMenuGroup(&mnuGrp))
		return ;

	IAcadToolbars  *tlbrs = NULL;
	HRESULT hr = S_OK;

	hr = mnuGrp->get_Toolbars(&tlbrs);
	mnuGrp->Release();

	long cnt = 0;
	hr = tlbrs->get_Count(&cnt);

	IAcadToolbar *polyTlbr = NULL;
	BSTR  tempName;

	VARIANT vtName;
	for (long i = 0; i < cnt; i++)
	{
		vtName.vt = VT_I4;
		vtName.lVal = i;
		hr = tlbrs->Item(vtName, &polyTlbr);
		hr = polyTlbr->get_Name(&tempName);
		CString tlbrName(tempName);
		SysFreeString(tempName);

		for (int i = 0; i < g_saGroupName.GetSize(); i++)
		{
			if (tlbrName.CompareNoCase(g_saGroupName.GetAt(i))==0)
			{
				polyTlbr->Delete();
				break;
			}
			else polyTlbr->Release();
		}
	}

	tlbrs->Release();
	g_saGroupName.RemoveAll();
	return;
}

BOOL FileExists(CString sFileName)
{
	BOOL bRetValue;
	WIN32_FIND_DATA filedata;

	if (FindFirstFile(sFileName,&filedata)==INVALID_HANDLE_VALUE)
	{
		//可能是共享名
		sFileName=sFileName+_T("\\*");

		if (FindFirstFile(sFileName,&filedata)==INVALID_HANDLE_VALUE) 
			bRetValue=FALSE;
		else
			bRetValue=TRUE;
	}
	else
	{
		bRetValue=TRUE;
	}
	return bRetValue;
}

int WINAPI ReCreateToolBar(CStringArray* pArray)
{
	CleanToolbars();
	if(pArray == NULL) return 1;
	int nNum = pArray->GetCount();
	if(nNum == 0) return 1;

	IAcadMenuGroup *mnuGrp = NULL;
	if (!getAcadMenuGroup(&mnuGrp))
		return 0;

	IAcadToolbars  *tlbrs = NULL;
	HRESULT hr = S_OK;
	hr = mnuGrp->get_Toolbars(&tlbrs);
	mnuGrp->Release();

	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(GetModuleHandle("shDistToolBarManager.arx"),szFileName,MAX_PATH);
	CString csPath(szFileName);
	csPath=csPath.Left(csPath.ReverseFind('\\')+1)+"Picture\\";

	INISetting ToolbarsCfgIni;
	if (!GetINISetting("ToolbarsCFG.ini", ToolbarsCfgIni))
	{
		return 0;
	}

	CStringArray saToolbars;
	ToolbarsCfgIni.GetAllAppName(saToolbars);



	for (int i = 0; i < saToolbars.GetCount(); i++)
	{
		CString sToolbarName = saToolbars.GetAt(i);

		CStringArray saBtnName, saBtnCmd;
		if (0 == ToolbarsCfgIni.GetAllKeysAndValues(sToolbarName, saBtnName, saBtnCmd))
		{
			continue;
		}


		IAcadToolbar  *tlbr = NULL;
		hr = tlbrs->Add(CComBSTR(sToolbarName), &tlbr);
		if FAILED(hr)
			continue;

		g_saGroupName.Add(sToolbarName);

		IAcadToolbarItem *button=NULL;
		VARIANT index;
		index.vt = VT_I4;
		index.lVal = 100l;

		VARIANT vtFalse;
		vtFalse.vt = VT_BOOL;
		vtFalse.boolVal = VARIANT_FALSE;

		for (int j = 0; j < saBtnCmd.GetCount(); j++)
		{
			BOOL bFound = FALSE;
			for(int k=0; k<nNum; k++)
			{
				CString strTemp = pArray->GetAt(k);
				CString strTempCmd = saBtnCmd.GetAt(j); strTempCmd.Trim();
				if(strTemp.CompareNoCase(strTempCmd) == 0)
				{
					bFound = TRUE; break;
				}
			}
			if(bFound == FALSE) continue;
			CString sName = saBtnName.GetAt(j);

			CString sDes = sName;

			CString sBmpName = sName;

			CString sCmd = saBtnCmd.GetAt(j) + " ";

			CString sIcoName = csPath + sBmpName + ".ico";

			if (!FileExists(sIcoName))
			{
				sIcoName = csPath+"dist.ico";
			}

			if(!FileExists(sIcoName)) continue;

			hr = tlbr->AddToolbarButton(index, CComBSTR(sDes),CComBSTR(sDes), CComBSTR(sCmd), vtFalse, &button);

			hr = button->SetBitmaps(CComBSTR(sIcoName),CComBSTR(sIcoName));

			button->Release();
		}

		tlbr->Dock(acToolbarDockRight);//acToolbarFloating
		tlbr->Release();
	}

	tlbrs->Release();

	return 1;
}
