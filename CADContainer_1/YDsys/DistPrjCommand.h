///////////////////////////////////////////////////////////////////////////////
//上海数慧系统技术有限公司拥有本代码之完全版权，未经上海数慧系统技术有限公司 //
//的明确书面许可，不得复制、取用、分发此源代码。                             //
//任何单位和个人在未获得许可情况下，不得以任何形式复制、修改和发布本软件的任 //
//何部分，否则将视为非法侵害，我公司将保留依法追究其责任的权利。在未获得许可 //
//情况下，任何组织或个人发布的产品都不能使用Dist的标志和Logo。               //
//                                                                           //
//support@dist.com.cn                                                        //
//www.dist.com.cn                                                            //
//                                                                           //
//作者 : 王晖                                                                //
//                                                                           //
// 版权所有 (C) 2007－2010 Dist Inc. 保留所有权利。                          //
///////////////////////////////////////////////////////////////////////////////

//文件说明：该文件中只包含执行命令函数和需要保存的全局变量
#ifndef  __DIST__PRJ__COMMAND__H__
#define  __DIST__PRJ__COMMAND__H__
#include "DistDocManagerReactor.h"

//创建属性块
//void gCreateBlockAttr();

// 函数功能：创建项目蓝线边界
// 实现步骤：
// 其他备注：
void gCmdCreateLXBound(CString strCadLyName,int nColorIndex,double dWidth,CString strSdeLyName);

// 功能：设置蓝线属性
// 参数：
// 返回：
void gCmdInputLXProperty(CString strCadLyName,CString strSdeLyName,CString strURL,CString strXMBH,CString strPrePrjId);
void gTestInput(CString strCadLyNameDK,CString strCadLyNameXM,
				CString strSdeLyNameDK,CString strSdeLyNameXM,CString strURL,CString strProcessId);

void gCmdSearchXMXX(CString strCadLyName,CString strSdeLyName);

// 功能：创建蓝线坐标标注(依次遍历所有点)
// 参数：
// 返回：
void gCmdCreatLabelAll(CString strCadLyName);


// 功能：创建蓝线坐标标注
// 参数：
// 返回：
void gCmdCreatLabel(CString strCadLyName);

void gCmdDeleteLabel(CString strCadLyName);


// 功能：根据指定图层名称读SDE图层数据
// 函数：
// 返回：
void gCmdReadSdeLys(CStringArray& strLyArray,/*int* pAccessPower,*/CString strCadLXName,CString strXMBH/*,CString strXZQH*/);

// 功能：根据指定图层名称和范围读SDE图层数据
// 函数：
// 返回：
void gCmdReadSdeLayerByRect(CString strSDELayerName,CString strMINX,CString strMAXX,CString strMINY,CString strMAXY,CString strXMBH);

//
void gCmdSaveCurXMObjectInfo(CString strCadLyName,const char* strCurXMBH);


void gCmdSaveBackDwg(CDistCommandManagerReactor* pCmdReactor);

void gCmdOpenBackDwg();

void gShowOrHideCadLayer(CString strCadLyName,BOOL bIsShow);

BOOL gReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag);

BOOL gReadSdeByRect(CString strSdeLyName,CString strXZQH,double& d_maxX,double& d_maxY,double& d_minX,double& d_minY);


//根据指定的精度拟合多边形
BOOL gTurnArc(AcDbObjectId& OutObjId,AcDbObjectId InObjId,char* strXDataRegName,int nMaxNum,double dPrecision);

//根据三点计算外接圆半径和圆心
bool gGetCenterAndRadiusWith3Pt(double& r,double& a,double& b,double x0,double y0,double x1,double y1,double x2,double y2);

//根据起点,终点,圆心和半径计算凸度
double gCalcBlg(double centX,double centY,double fromX,double fromY,double toX,double toY,double dR);
BOOL  ReadCfgLyNameArray(CStringArray& strLyNameArray,CString strTag);

#endif