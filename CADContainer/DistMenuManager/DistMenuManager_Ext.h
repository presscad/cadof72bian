#ifndef  _DISTMENUMANAGER_EXT_H_
#define  _DISTMENUMANAGER_EXT_H_

// �����õĺ꣬������ģ��ʱ���ú�
#ifdef DISTMENUMANAGER_MODULE 
#define DISTMENUMANAGER_EXT __declspec(dllexport)
#else 
#define DISTMENUMANAGER_EXT __declspec(dllimport)
#endif

DISTMENUMANAGER_EXT BOOL Sys_LoadDistMenu(LPCTSTR lpMenuName);

DISTMENUMANAGER_EXT BOOL Sys_UnLoadDistMenu(LPCTSTR lpMenuName);

#endif  _DISTMENUMANAGER_EXT_H_
