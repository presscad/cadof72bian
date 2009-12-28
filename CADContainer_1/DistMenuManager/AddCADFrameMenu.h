/*-------------------------------------------------------------------------
���ߣ� R
�����װ����AuToCAD����Ӳ˵����Ӳ˵��ķ�����ʹ��COM�ӿ�ʵ��
--------------------------------------------------------------
 ʹ�÷�����
CAddCADFrameMenu menu;
menu.AddMenu("�ҵĲ˵�");
menu.AddSubMenu("�����Ի���", "_DLG ");
menu.AddSubMenu("��������", "_DRAWL ");
menu.AddSubMenu("*Line");// ��"*"�ű�ʾ�Ƕ����Ӳ˵�
menu.AddSubMenu("$CIRCLE", "_CIRCLE ");//��"$"�ű�ʾ�����Ӳ˵��µĲ˵���
menu.AddSubSeparator();
menu.AddSubMenu("$PLINE", "_PLINE ");//��"$"�ű�ʾ�����Ӳ˵��µĲ˵���
menu.AddSeparator();
menu.AddSubMenu("ѡ���ļ���", "_SELFOLDER ");
menu.AddSubMenu("*TEXT");// ��"*"�ű�ʾ�Ƕ����Ӳ˵�
menu.AddSubMenu("$MTEXT", "_TEXT ");//��"$"�ű�ʾ�����Ӳ˵��µĲ˵���
menu.CreateMenus();
--------------------------------------------------------------

2006-07-22 ������
1.������ACAD�˵����Ϊ���Ҳ˵��������һ���˵���Ӧ�Ĳ˵��飬ȷ���κ�����¾��ɼ��ز˵�
2.�����˲˵����μ��غ�CAD�����bug
3.�����˳���ж�غ�˵�û��ж�ص�bug
4.����������Ӳ˵�seperator�ĺ���
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

 // �����˵�
 void CreateMenus();
 // ��Ӳ˵�
 void AddMenu(const char* menuName);
 // ����Ӳ˵���menuName-�˵���"--"Ϊ�ָ���,commandҪ�ӿո�
 void AddSubMenu(const char* menuName, const char* command=NULL);
 // ��ӷָ���
 void AddSeparator();
 // ��Ӷ����Ӳ˵��ָ���
 void AddSubSeparator();

 static bool IsMenuExist(IAcadPopupMenu *&pMnu,long &index,const char *mnuname,IAcadPopupMenus *mnus);//�˵��Ƿ���ڣ�������μ��� add by zhengzhilin
 static bool GetAcadApplication(IAcadApplication *&pAcadApp);
 static bool GetAcadMenuGroup(IAcadMenuGroup  **pVal,LPCTSTR menuname);
 static bool GetLastMenuFromMenubar(CString &sMenu);
 static bool GetFirstMenuFromMenubar(CString &sMenu);
 static void ClearMenu(IAcadPopupMenus *mnus,LPCTSTR menuname);
 static bool GetAcadApplication(LPDISPATCH * pVal);
 static bool GetAcadMenuGroup(IAcadMenuGroup  **pVal);
 static void CleanUpMenus(); //������Menuһ��Ҫ�����������

private:

 class CCADMenuArray : public CArray <CADMenu*, CADMenu*>
 {
 public:
  int AddMenu(CADMenu& menu);
  int AddMenu(const char* menuName, const char* command);
  ~CCADMenuArray();
 };
 
 CArray <CCADMenuArray*, CCADMenuArray*> m_menus;
 int m_menuCount;  // �˵�����

 CStringArray m_TopMenuNames; // �˵���


};

#endif // !defined(AFX_ADDCADFRAMEMENU_H__153847F5_DE2C_4B6E_83E8_5BE9FE780CD8__INCLUDED_)

