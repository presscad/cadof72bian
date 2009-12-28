#ifndef  _DISTLAYERTREE_EXT_H_
#define  _DISTLAYERTREE_EXT_H_

// 导出用的宏，及导出模板时所用宏
#ifdef DISTLAYERTEE_EXPORTS 
#define DISTLAYERTEE_EXT __declspec(dllexport)
#else 
#define DISTLAYERTEE_EXT __declspec(dllimport)
#endif

DISTLAYERTEE_EXT BOOL Sys_LoadLayerTree(LPCTSTR lpXml, long nType);

DISTLAYERTEE_EXT void Sys_GetCheckedLayers(CStringArray& arrayLayers);

DISTLAYERTEE_EXT void Sys_SetCheckedLayers(CStringArray& arrayLayers);

DISTLAYERTEE_EXT void Sys_ShowDockTreePane(long nType); // nType： 项目ID 六线调整为0,电子展板为-1

DISTLAYERTEE_EXT void Sys_ShowSampToolBar(BOOL bShow);

DISTLAYERTEE_EXT void Sys_StartEdit(); // 开始编辑

DISTLAYERTEE_EXT void Sys_EndEdit(); // 取消编辑

DISTLAYERTEE_EXT void Sys_CancelEdit(); // 结束编辑

#endif  _DISTLAYERTREE_EXT_H_
