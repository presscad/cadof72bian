#ifndef  _DISTTOOLBAR_EXT_H_
#define  _DISTTOOLBAR_EXT_H_

// �����õĺ꣬������ģ��ʱ���ú�
#ifdef DISTTOOLBAR_EXPORTS 
#define DISTTOOLBAR_EXT __declspec(dllexport)
#else 
#define DISTTOOLBAR_EXT __declspec(dllimport)
#endif

DISTTOOLBAR_EXT BOOL Sys_LoadDistToolBar(LPCTSTR lpXml);

DISTTOOLBAR_EXT BOOL Sys_UnLoadDistToolBar();

#endif  _DISTTOOLBAR_EXT_H_
