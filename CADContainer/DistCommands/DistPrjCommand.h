//文件说明：该文件中只包含执行命令函数和需要保存的全局变量
#ifndef  __DIST__PRJ__COMMAND__H__
#define  __DIST__PRJ__COMMAND__H__
#include "DistBasePara.h"

// 函数功能：创建项目蓝线边界
// 实现步骤：
// 其他备注：
void gCmdCreateLXBound(CString strCadLyName,int nColorIndex,double dWidth);

// 功能：设置蓝线属性
// 参数：
// 返回：
void gCmdInputLXProperty(CString strCadLyName,CString strSdeLyName,HWND hDapHwnd,CString strURL,CString strXMBH);


void gCmdSearchXMXX(CString strCadLyName,CString strSdeLyName);

// 功能：创建蓝线坐标标注(依次遍历所有点)
// 参数：
// 返回：
void gCmdCreatLabelAll(CString strCadLyName);


// 功能：创建蓝线坐标标注
// 参数：
// 返回：
void gCmdCreatLabel(CString strCadLyName);


// 功能：根据指定图层名称和访问权限读SDE图层数据
// 函数：
// 返回：
void gCmdReadSdeLys(CStringArray& strLyArray,int* pAccessPower,CString strCadLXName,CString strXMBH);


//
void gCmdSaveCurXMObjectInfo(CString strCadLyName,const char* strCurXMBH);


void gCreateLXBlkDef(char* strCadName,char* strSdeName,double dmaxx,double dmaxy,double dminx,double dminy);

void gCmdChangeOutState();
void gOutRunInfo (long rc, char *szComment);
#endif