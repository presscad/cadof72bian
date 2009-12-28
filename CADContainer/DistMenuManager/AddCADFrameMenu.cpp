/*-------------------------------------------------------------------------
作者：R
该类封装了在AuToCAD上添加菜单及子菜单的方法，使用COM接口实现
--------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "AddCADFrameMenu.h"
//#include "acad15_i.c"
#include "acadi_i.c"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CStringArray stcMyTopMenuNames;//存储新添加的CAD顶级菜单项名称

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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

CAddCADFrameMenu::CAddCADFrameMenu()
{
 m_menuCount = 0;
 m_TopMenuNames.RemoveAll();
}

CAddCADFrameMenu::~CAddCADFrameMenu()
{
 int num = m_menus.GetSize();
 for (int i=0; i<num; i++) {
  if (m_menus.GetAt(i) != NULL) 
   delete m_menus[i];
 }
}

int CAddCADFrameMenu::CCADMenuArray::AddMenu(CADMenu& menu)
{
 CADMenu* pMenu = new CADMenu;
 sprintf(pMenu->name, menu.name);
 sprintf(pMenu->order, menu.order);
 return Add(pMenu);
}

int CAddCADFrameMenu::CCADMenuArray::AddMenu(const char* menuName, const char* command)
{
 if (sizeof(menuName) > 63) {
#ifdef _DEBUG
  acutPrintf("menuName'%s'长度超出范围\n", menuName);
#endif  
  return -99; 
 }
 if (sizeof(command) > 63) {
#ifdef _DEBUG
  acutPrintf("command'%s'长度超出范围\n", command);
#endif  
  return -99; 
 }
 
 CADMenu* pMenu = new CADMenu;
 sprintf(pMenu->name, menuName);
 if (command != NULL)
  sprintf(pMenu->order, command);
 return Add(pMenu); 
}

CAddCADFrameMenu::CCADMenuArray::~CCADMenuArray()
{
 for (int i=0; i<m_nSize; i++) {
  if (GetAt(i) != NULL) {
   delete GetAt(i);
  }
 } 
}

void CAddCADFrameMenu::AddSubMenu(const char* menuName, 
          const char* command)
{
 ASSERT(menuName != NULL);
 if (m_menuCount == 0) {
#ifdef _DEBUG
  acutPrintf("添加子菜单%s前没有调用AddMenu添加一个菜单条目！无法添加子菜单\n", menuName);
#endif
  return;
 }
 m_menus[m_menuCount-1]->AddMenu(menuName, command);
}

void CAddCADFrameMenu::AddSeparator()
{
 m_menus[m_menuCount-1]->AddMenu("--", "");
}

void CAddCADFrameMenu::AddSubSeparator()
{
 m_menus[m_menuCount-1]->AddMenu("$--", "");
}

void CAddCADFrameMenu::AddMenu(const char* menuName)
{
 m_TopMenuNames.Add(menuName);
 CCADMenuArray* pArr = new CCADMenuArray;
 m_menuCount = m_menus.Add(pArr) + 1;
}

bool CAddCADFrameMenu::GetAcadApplication(LPDISPATCH * pVal)
{ 
 LPDISPATCH pDispatch = acedGetAcadWinApp()->GetIDispatch(TRUE);
 if (pDispatch == NULL)
  return false;
 *pVal = pDispatch;
 return true;
}

bool CAddCADFrameMenu::GetAcadApplication(IAcadApplication *&pAcadApp)
{
 LPDISPATCH  pDisp = NULL;
 
 if (!GetAcadApplication(&pDisp))
  return false;
 
 HRESULT hr = S_OK;
 hr = pDisp->QueryInterface(IID_IAcadApplication, (LPVOID*)&pAcadApp);
 if (FAILED(hr))
  return false;

 return true;
}

bool CAddCADFrameMenu::GetLastMenuFromMenubar(CString &sMenu)
{
 IAcadApplication *pAcad = NULL;
 if(!GetAcadApplication(pAcad))
  return false;

 IAcadMenuBar *pMenuBar=NULL;
 IAcadPopupMenu  *pPopUpMenu=NULL;
 
 pAcad->get_MenuBar(&pMenuBar);
    pAcad->Release();
    long numberOfMenus;
    pMenuBar->get_Count(&numberOfMenus);

 //得到菜单栏上面的最后一个菜单
 BSTR name;
 VARIANT vIndex;
 vIndex.vt=VT_I4;
 vIndex.lVal=numberOfMenus-1;
 
 pMenuBar->Item(vIndex,&pPopUpMenu);
 pMenuBar->Release();
 pPopUpMenu->get_NameNoMnemonic(&name);
 pPopUpMenu->Release();
 
 sMenu.Format("%S",name);//debug 大写，用于转化LPWSTR类型字符串
 SysFreeString(name);

 return true;
}

bool CAddCADFrameMenu::GetFirstMenuFromMenubar(CString &sMenu)
{
 IAcadApplication *pAcad = NULL;
 if(!GetAcadApplication(pAcad))
  return false;

 IAcadMenuBar *pMenuBar=NULL;
 IAcadPopupMenu  *pPopUpMenu=NULL;
 
 pAcad->get_MenuBar(&pMenuBar);
    pAcad->Release();

 //得到菜单栏上面的最后一个菜单
 BSTR name;
 VARIANT vIndex;
 vIndex.vt=VT_I4;
 vIndex.lVal=0;
 
 pMenuBar->Item(vIndex,&pPopUpMenu);
 pMenuBar->Release();
 pPopUpMenu->get_NameNoMnemonic(&name);
 pPopUpMenu->Release();
 
 sMenu = name;//debug 大写，用于转化LPWSTR类型字符串
 SysFreeString(name);

 return true;
}

bool CAddCADFrameMenu::GetAcadMenuGroup(IAcadMenuGroup  **pVal,LPCTSTR menuname)
{
	IAcadApplication *acadApp = NULL;
	HRESULT hr = S_OK;

	if(!GetAcadApplication(acadApp))
		return false;

	LPDISPATCH  pTempDisp = NULL;
	IAcadMenuGroups *mnuGrps = NULL;
	long cnt = 0, cntmnu=0;

	//得到菜单组集
	hr = acadApp->get_MenuGroups(&mnuGrps);
	if (FAILED(hr))
	{
		acadApp->Release();
		return false;
	}
	acadApp->Release();

	mnuGrps->get_Count(&cnt);

	//得到菜单组
	IAcadMenuGroup *mnuGrp = NULL;
	IAcadPopupMenus  *mnus = NULL;
	IAcadPopupMenu *pPopUpMenu=NULL;

	VARIANT  vtName,vtMenu;
	vtName.vt = VT_I4;
	vtMenu.vt = VT_I4;

	BSTR  mnuName;
	bool found = false ;
	for (long i=0; i < cnt; i++)
	{
		vtName.lVal = i;
		hr = mnuGrps->Item(vtName, &mnuGrp);
		if (FAILED(hr))
		{
			return false;
		}

		BSTR  grpName;
		mnuGrp->get_Name(&grpName);
		CString cgrpName(grpName);
		SysFreeString(grpName);
		if (cgrpName.CompareNoCase("Acad")!=0)
			continue;

		mnuGrp->get_Menus(&mnus);

		mnus->get_Count(&cntmnu);

		for(long j=0;j<cntmnu;j++)
		{
			vtMenu.lVal=j;
			hr=mnus->Item(vtMenu,&pPopUpMenu);
			if(FAILED(hr))
				continue;

			//   pPopUpMenu->get_Name(&mnuName);
			pPopUpMenu->get_NameNoMnemonic(&mnuName);
			pPopUpMenu->Release();

			CString sMenuName(mnuName);
			SysFreeString(mnuName);

			//   sMenuName.Replace("&","");//debug
			if(sMenuName.CompareNoCase(menuname)==0)
			{
				found=true;
				*pVal = mnuGrp;
				break;
			}
		}

		mnus->Release();

		if(found)
		{
			break;
		}
		else
		{
			mnuGrp->Release();
		}
	}

	mnuGrps->Release();

	return found;
}
/*
bool CAddCADFrameMenu::GetAcadMenuGroup(IAcadMenuGroup  **pVal,LPCTSTR menuname)
{
	IAcadMenuGroup *mnuGrp = NULL;
	if (!getAcadMenuGroup(&mnuGrp))
		return false;

	//now get all the popup menus 
	IAcadPopupMenus  *mnus = NULL;
	HRESULT hr = S_OK;
	hr = mnuGrp->get_Menus(&mnus);
	long cnt = 0l;
	hr = mnus->get_Count(&cnt);
	mnuGrp->Release();

	//now get Tools menu
	IAcadPopupMenu *toolsMenu = NULL;
	BSTR   tempName;
	VARIANT vtName;
	bool found = false;
	long iDraw=6L;
	for (long i=0; i < cnt; i++)
	{
		vtName.vt = VT_I4;
		vtName.lVal = i;
		hr = mnus->Item(vtName, &toolsMenu);
		if (FAILED(hr))
			return false;
		//bool found=false;
		hr = toolsMenu->get_NameNoMnemonic(&tempName);
		CString mnuName(tempName);
		SysFreeString(tempName);

		if(mnuName.CompareNoCase(menuname)==0)
		{
			found=true;
			break;
		}

		if (found) break;
	}

	if (found)
		mnus->Release();

	return found;
}
*/
void CAddCADFrameMenu::ClearMenus()
{
	int num = m_menus.GetSize();
	for (int i=0; i<num; i++) {
		if (m_menus.GetAt(i) != NULL) 
			delete m_menus[i];
	}
	m_menus.RemoveAll();
	m_menuCount = 0;
	m_TopMenuNames.RemoveAll();
}


void CAddCADFrameMenu::ClearMenu(LPCTSTR lpMenuName)
{
	for (int i=0; i<m_TopMenuNames.GetCount(); i++)
	{
		if (m_TopMenuNames.GetAt(i).CompareNoCase(lpMenuName) == 0)
		{
			if (m_menus.GetAt(i) != NULL)
			{
				delete m_menus[i];
				m_menus.RemoveAt(i);
			}
			m_menuCount--;
			m_TopMenuNames.RemoveAt(i);

			break;
		}
	}
}

void CAddCADFrameMenu::CreateMenus()
{
 CMDIFrameWnd * pMainFrame = acedGetAcadFrame();
 pMainFrame->LockWindowUpdate();
/* 
 // 处理二次加载
 IAcadMenuGroup *mnuGrp_Ext = NULL;
 for(int n=0;n<m_TopMenuNames.GetSize();n++)
 {
  CString sMenu=m_TopMenuNames.GetAt(n);
  sMenu.Replace("&","");

  if (!GetAcadMenuGroup(&mnuGrp_Ext,sMenu))
  {
#ifdef _DEBUG
   acutPrintf("没有找到 %s 所属的菜单组，无法卸载。\n",sMenu);
#endif
   
//   mnuGrp_Ext->Release();
   continue;
  }

  //得到所有菜单组 
  IAcadPopupMenus  *mnus = NULL;
  HRESULT hr = S_OK;
  hr = mnuGrp_Ext->get_Menus(&mnus);

  ClearMenu(mnus,sMenu);

  mnus->Release();
  mnuGrp_Ext->Release();

 }
*/
 ////得到菜单栏上面的最后一个菜单 //！！！最后一个是自定义菜单时，插入位置是按自定义菜单组中的菜单数量计算，不正确！
 //得到菜单栏上面的第一个菜单 //取得的是CAD默认菜单组
 CString sName;
 //if(!GetLastMenuFromMenubar(sName))
 if(!GetFirstMenuFromMenubar(sName))
 {
	 pMainFrame->LockWindowUpdate();
  return;
 }

 sName.Replace("&","");//debug
 
 IAcadMenuGroup *mnuGrp = NULL;
 if (!GetAcadMenuGroup(&mnuGrp,sName))
 {
	 pMainFrame->LockWindowUpdate();
  return ;
 }

 //得到所有菜单组
 IAcadPopupMenus   *mnus = NULL;
  IAcadPopupMenu   *mnu = NULL;
 IAcadPopupMenuItem  *mnuitem = NULL;
 HRESULT hr = S_OK;
 hr = mnuGrp->get_Menus(&mnus);
 long cnt = 0l;
 hr = mnus->get_Count(&cnt);
 int i = 0;
 int j = 0;
 int k = 0;
 VARIANT index;
 VariantInit(&index);
 V_VT(&index) = VT_I4;
 CArray<IAcadPopupMenu*, IAcadPopupMenu*> MnuAdrs; 

 long lIndex=0;
 WCHAR MenuName[256];
 for (i = 0; i < m_menuCount; i++) 
 {
  CString tmpMenu=m_TopMenuNames[i];
  tmpMenu.Replace("&","");
  if(IsMenuExist(mnu,lIndex,tmpMenu,mnus))//处理二次加载
  {
   V_I4(&index) = lIndex;
  }
  else
  {
   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_TopMenuNames[i], -1, MenuName, 256); 
   mnus->Add(MenuName, &mnu); 
   V_I4(&index) = cnt++;
  }
  
  MnuAdrs.Add(mnu);
  mnu->InsertInMenuBar(index);
 }

 IAcadPopupMenu *MenuItem = NULL;
 int subMenuNum = 0;
 CCADMenuArray* Menus;
 WCHAR szSubMenu[256];
 WCHAR szSubMenuMacro[256];
 CString MenusName;
 CString Menusorder;
 for (j = 0; j < m_menuCount; j++) {
  Menus = m_menus.GetAt(j);
  subMenuNum = Menus->GetSize();
  for (k = 0; k < subMenuNum; k++) { 
   mnu = MnuAdrs.GetAt(j);
   if (!strcmp("--", Menus->GetAt(k)->name)) {
    V_I4(&index) = k;
    mnu->AddSeparator(index, &mnuitem);
   }
   else {
    V_I4(&index) = k;    
    MenusName = Menus->GetAt(k)->name; 
    Menusorder = Menus->GetAt(k)->order;
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, MenusName, -1, szSubMenu, 256); 
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, Menusorder, -1, szSubMenuMacro, 256); 
    if (strcspn(Menus->GetAt(k)->name, "*") < (size_t)MenusName.GetLength()){
     MenusName.TrimLeft('*');
     MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, MenusName, -1, szSubMenu, 256); 
     hr = mnu->AddSubMenu(index, szSubMenu, &MenuItem);
    }    
    else if (strcspn(Menus->GetAt(k)->name, "$") < (size_t)MenusName.GetLength() ){        
     MenusName.TrimLeft('$');
     if(MenusName.Compare("--")==0)//添加子菜单的分隔符
     {
      hr = MenuItem->AddSeparator(index, &mnuitem);
     }
     else
     {
      MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, MenusName, -1, szSubMenu, 256); 
      hr = MenuItem->AddMenuItem(index, szSubMenu, szSubMenuMacro, &mnuitem);
     }
    }
    else {
     hr = mnu->AddMenuItem(index, szSubMenu, szSubMenuMacro, &mnuitem);
    }
   }
  }
 }

 for (i=0; i<MnuAdrs.GetSize(); i++)
 {
  MnuAdrs[i]->Release();
 }
 mnuitem->Release();
 mnus->Release();
 mnuGrp->Release();
 pMainFrame->UnlockWindowUpdate(); 
 
 stcMyTopMenuNames.Copy(m_TopMenuNames);//将顶级菜单名存入静态数组中，卸载时使用

 return ;
}

bool CAddCADFrameMenu::IsMenuExist(IAcadPopupMenu *&pMnu,long &index,const char *mnuname,IAcadPopupMenus *mnus)
{
 ASSERT(mnus!=NULL);
 
 long cnt = 0l;
 HRESULT hr = mnus->get_Count(&cnt);
 bool found = false;
 int curMenuIndex=-1;
 BSTR tempName;

#ifdef _DEBUG
 acutPrintf("IsMenuExist cnt=%ld\n",cnt);
#endif
 
 //
 IAcadPopupMenu *toolsMenu = NULL;
 for (long i=0; i < cnt; i++)
 {
  VARIANT vtName;
  VariantInit(&vtName);
  V_VT(&vtName) = VT_I4;
  V_I4(&vtName) = i;
  hr = mnus->Item(vtName, &toolsMenu);
  if (FAILED(hr))
   return false;
  hr = toolsMenu->get_NameNoMnemonic(&tempName);
  CString mnuName(tempName);

#ifdef _DEBUG
  acutPrintf("\n%d,%s",i,mnuName);
#endif
  if(mnuName.Compare(mnuname)==0)
  {
   index=i;
   pMnu=toolsMenu;
   return true;
  } 
 }
 pMnu=NULL;
 return false;
}

void CAddCADFrameMenu::CleanUpMenu(LPCTSTR lpMenuName)
{
	IAcadMenuGroup *mnuGrp = NULL;

	if (!GetAcadMenuGroup(&mnuGrp,lpMenuName))
	{
#ifdef _DEBUG
		acutPrintf("没有找到 %s 所属的菜单组，无法卸载。\n",lpMenuName);
#endif

//		mnuGrp->Release();
		
		for (int i=0; i<stcMyTopMenuNames.GetCount(); i++)
		{
			if (stcMyTopMenuNames.GetAt(i).CompareNoCase(lpMenuName) == 0)
			{
				stcMyTopMenuNames.RemoveAt(i);
				break;
			}
		}

		return;
	}

	//得到所有菜单组 
	IAcadPopupMenus  *mnus = NULL;
	HRESULT hr = S_OK;
	hr = mnuGrp->get_Menus(&mnus);

	ClearMenu(mnus,lpMenuName);

	mnus->Release();
	mnuGrp->Release();

	for (int i=0; i<stcMyTopMenuNames.GetCount(); i++)
	{
		if (stcMyTopMenuNames.GetAt(i).CompareNoCase(lpMenuName) == 0)
		{
			stcMyTopMenuNames.RemoveAt(i);
			break;
		}
	}
}

void CAddCADFrameMenu::CleanUpMenus()
{
 IAcadMenuGroup *mnuGrp = NULL;
 for(int i=0;i<stcMyTopMenuNames.GetSize();i++)
 {
  CString sMenu=stcMyTopMenuNames.GetAt(i);
  sMenu.Replace("&","");

  if (!GetAcadMenuGroup(&mnuGrp,sMenu))
  {
#ifdef _DEBUG
   acutPrintf("没有找到 %s 所属的菜单组，无法卸载。\n",sMenu);
#endif
   
//   mnuGrp->Release();
   continue;
  }

  //得到所有菜单组 
  IAcadPopupMenus  *mnus = NULL;
  HRESULT hr = S_OK;
  hr = mnuGrp->get_Menus(&mnus);

  ClearMenu(mnus,sMenu);

  mnus->Release();
  mnuGrp->Release();

 }

 stcMyTopMenuNames.RemoveAll();
}

void CAddCADFrameMenu::ClearMenu(IAcadPopupMenus *mnus,LPCTSTR menuname)
{
 long cnt = 0l;
 HRESULT hr = mnus->get_Count(&cnt);
 
 //now get Tools menu
 IAcadPopupMenu *toolsMenu = NULL;
 BSTR   tempName;
 VARIANT vtName;
 VariantInit(&vtName);

 bool found = false;
 for (long i=0; i < cnt; i++){
  vtName.vt = VT_I4;
  vtName.lVal = i;
  hr = mnus->Item(vtName, &toolsMenu);
  if (FAILED(hr))
   return ;
  
  hr = toolsMenu->get_NameNoMnemonic(&tempName);
  CString mnuName(tempName);
  SysFreeString(tempName);
  if(mnuName.CompareNoCase(menuname)==0)
  {
   found = true;
   break;
  }
 }
 
 if (!found)
 {
  acutPrintf("没有找到菜单 : %s\n",menuname);
  return;
 }
 
 hr = toolsMenu->get_Count(&cnt);
 VARIANT vIndex;
 vIndex.vt = VT_I4;
 vIndex.lVal = cnt;
 IAcadPopupMenuItem *cmd1 = NULL;
 while(cnt--)
 {
  vIndex.lVal=cnt;
  hr=toolsMenu->Item(vIndex,&cmd1);
  hr=cmd1->Delete();
  cmd1->Release();
 }
 
 toolsMenu->RemoveFromMenuBar();
 toolsMenu->Release();

 mnus->RemoveMenuFromMenuBar(vtName);
 
 return;
 
}
