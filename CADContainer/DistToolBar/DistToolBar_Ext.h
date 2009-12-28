#ifndef  _DISTTOOLBAR_EXT_H_
#define  _DISTTOOLBAR_EXT_H_

// 导出用的宏，及导出模板时所用宏
#ifdef DISTTOOLBAR_EXPORTS 
#define DISTTOOLBAR_EXT __declspec(dllexport)
#else 
#define DISTTOOLBAR_EXT __declspec(dllimport)
#endif

DISTTOOLBAR_EXT BOOL Sys_LoadDistToolBar(LPCTSTR lpXml);

DISTTOOLBAR_EXT BOOL Sys_UnLoadDistToolBar();

#endif  _DISTTOOLBAR_EXT_H_
