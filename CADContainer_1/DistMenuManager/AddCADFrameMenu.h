/*-------------------------------------------------------------------------
作者： R
该类封装了在AuToCAD上添加菜单及子菜单的方法，使用COM接口实现
--------------------------------------------------------------
 使用方法：
CAddCADFrameMenu menu;
menu.AddMenu("我的菜单");
menu.AddSubMenu("弹出对话框", "_DLG ");
menu.AddSubMenu("绘制线条", "_DRAWL ");
menu.AddSubMenu("*Line");// 加"*"号表示是二级子菜单
menu.AddSubMenu("$CIRCLE", "_CIRCLE ");//加"$"号表示二级子菜单下的菜单条
menu.AddSubSeparator();
menu.AddSubMenu("$PLINE", "_PLINE ");//加"$"号表示二级子菜单下的菜单条
menu.AddSeparator();
menu.AddSubMenu("选择文件夹", "_SELFOLDER ");
menu.AddSubMenu("*TEXT");// 加"*"号表示是二级子菜单
menu.AddSubMenu("$MTEXT", "_TEXT ");//加"$"号表示二级子菜单下的菜单条
menu.CreateMenus();
--------------------------------------------------------------

2006-07-22 修正版
1.将查找ACAD菜单组改为查找菜单栏中最后一个菜单对应的菜单组，确保任何情况下均可加载菜单
2.修正了菜单二次加载后CAD出错的bug
3.修正了程序卸载后菜单没有卸载的bug
4.添加了增加子菜单seperator的函数
----------------------------------------------------------------------------*/

#if !defined(AFX_ADDCADFRAMEMENU_H__153847F5_DE2C_4B6E_83E8_5BE9FE780CD8__INCLUDED_)
#define AFX_ADDCADFRAMEMENU_H__153847F5_DE2C_4B6E_83E8_5BE9FE780CD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
//#include "acad15.h"
#include "acadi.h"

 

typedef struct CADMenu
{
 char name[64];
 char order[64];
}CADMenu;


class CAddCADFrameMenu  
{
public:
 CAddCADFrameMenu();
 virtual ~CAddCADFrameMenu();

 // 创建菜单
 void CreateMenus();
 // 添加菜单
 void AddMenu(const char* menuName);
 // 添加子菜单，menuName-菜单名"--"为分隔符,command要加空格
 void AddSubMenu(const char* menuName, const char* command=NULL);
 // 添加分隔符
 void AddSeparator();
 // 添加二级子菜单分隔符
 void AddSubSeparator();

 static bool IsMenuExist(IAcadPopupMenu *&pMnu,long &index,const char *mnuname,IAcadPopupMenus *mnus);//菜单是否存在，避免二次加载 add by zhengzhilin
 static bool GetAcadApplication(IAcadApplication *&pAcadApp);
 static bool GetAcadMenuGroup(IAcadMenuGroup  **pVal,LPCTSTR menuname);
 static bool GetLastMenuFromMenubar(CString &sMenu);
 static bool GetFirstMenuFromMenubar(CString &sMenu);
 static void ClearMenu(IAcadPopupMenus *mnus,LPCTSTR menuname);
 static bool GetAcadApplication(LPDISPATCH * pVal);
 static bool GetAcadMenuGroup(IAcadMenuGroup  **pVal);
 static void CleanUpMenus(); //创建了Menu一定要调用这个函数

private:

 class CCADMenuArray : public CArray <CADMenu*, CADMenu*>
 {
 public:
  int AddMenu(CADMenu& menu);
  int AddMenu(const char* menuName, const char* command);
  ~CCADMenuArray();
 };
 
 CArray <CCADMenuArray*, CCADMenuArray*> m_menus;
 int m_menuCount;  // 菜单个数

 CStringArray m_TopMenuNames; // 菜单名


};

#endif // !defined(AFX_ADDCADFRAMEMENU_H__153847F5_DE2C_4B6E_83E8_5BE9FE780CD8__INCLUDED_)

