#ifndef  _DISTLAYERTREE_EXT_H_
#define  _DISTLAYERTREE_EXT_H_

// �����õĺ꣬������ģ��ʱ���ú�
#ifdef DISTLAYERTEE_EXPORTS 
#define DISTLAYERTEE_EXT __declspec(dllexport)
#else 
#define DISTLAYERTEE_EXT __declspec(dllimport)
#endif

DISTLAYERTEE_EXT BOOL Sys_LoadLayerTree(LPCTSTR lpXml, long nType);

DISTLAYERTEE_EXT void Sys_GetCheckedLayers(CStringArray& arrayLayers);

DISTLAYERTEE_EXT void Sys_SetCheckedLayers(CStringArray& arrayLayers);

DISTLAYERTEE_EXT void Sys_ShowDockTreePane(long nType); // nType�� ��ĿID ���ߵ���Ϊ0,����չ��Ϊ-1

DISTLAYERTEE_EXT void Sys_ShowSampToolBar(BOOL bShow);

DISTLAYERTEE_EXT void Sys_StartEdit(); // ��ʼ�༭

DISTLAYERTEE_EXT void Sys_EndEdit(); // ȡ���༭

DISTLAYERTEE_EXT void Sys_CancelEdit(); // �����༭

#endif  _DISTLAYERTREE_EXT_H_
