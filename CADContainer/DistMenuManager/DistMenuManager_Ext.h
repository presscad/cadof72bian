#ifndef  _DISTMENUMANAGER_EXT_H_
#define  _DISTMENUMANAGER_EXT_H_

// 导出用的宏，及导出模板时所用宏
#ifdef DISTMENUMANAGER_MODULE 
#define DISTMENUMANAGER_EXT __declspec(dllexport)
#else 
#define DISTMENUMANAGER_EXT __declspec(dllimport)
#endif

DISTMENUMANAGER_EXT BOOL Sys_LoadDistMenu(LPCTSTR lpMenuName);

DISTMENUMANAGER_EXT BOOL Sys_UnLoadDistMenu(LPCTSTR lpMenuName);

#endif  _DISTMENUMANAGER_EXT_H_
