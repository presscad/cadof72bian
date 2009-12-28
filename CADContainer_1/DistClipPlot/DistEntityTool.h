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

#ifndef __DIST_ENTITY_TOOL_H__
#define __DIST_ENTITY_TOOL_H__
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//文件说明：公共函数
//函数说明：1.以功能为条件，将函数划分到不同的子工具类
//          2.函数功能实现尽量独立，简捷；避免函数多层次调用，参数多层次传递，降低函数之间的耦合
//          3.函数定义规则：a> 函数返回值为long型（1成功，0失败，其他值参考错误定义）
//                          b> 返回参数放在函数参数的开始位置,然后放必需参数，最后放默认参数
//                          c> 参数严格按照匈牙利方法命名，同类函数的同类参数，参数的位置次序和名称尽量一致
//                          d> 函数命名要求准确，简捷，错落有致
//          4.代码编写风格要一致，不要搞所谓的“创新”
//
//作者：王晖 <wanghui@dist.com.cn>
//日期：2007年4月,上海.
//修改： 

#include "StdAfx.h"

#define  CURDBPOINTER acdbHostApplicationServices()->workingDatabase()

class CDistEntityTool
{
public:
	               CDistEntityTool(void);
	virtual       ~CDistEntityTool(void);

public:

	//功能：创建图层
	//参数：resultId    返回创建实体的ID
	//      strLyName   图层名
	//      nClrIndex   颜色号
	//      strLnType   线型
	//      dLineWeigth 线宽
	//      pDB         数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreateLayer(AcDbObjectId& resultId,const char *strLyName, int  nClrIndex = 7,const char *strLnType = "Continuous",
		             AcDb::LineWeight dLineWeigth = AcDb::kLnWtByLwDefault, AcDbDatabase* pDB = CURDBPOINTER);

	//功能：设置CAD当前操作图层
	//参数：strLayerName   图层名
	//      pDB            数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long SetCurLayer(const char* strLayerName, AcDbDatabase* pDB= CURDBPOINTER);


	long SelectEntityInLayer(AcDbObjectIdArray& IdArray,const char *strLyName);

	//功能：删除指定图层中的所有实体
	//参数：strLyName   图层名
	//      pDB            数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long DeleteAllEntityInLayer(char *strLyName,AcDbDatabase* pDB= CURDBPOINTER);


	//功能：创建单行文本
	//参数：resultId       返回创建实体的ID
	//      ptAlign        文字对齐点坐标
	//      strText        文本内容
	//      strLyName      图层名
	//      nClrIndex      颜色号
	//      strFontType    字体
	//      nHorzAlignType 文字横向对齐方式(0:左对齐,1:居中对齐,2:右对齐)
	//      nVertAlignType 文字竖向对齐方式(0:下对齐,1:居中对齐,2:上对齐)
	//      dHeight        文字高度
	//      dWidFactor     文字宽高比
	//      dAngle         旋转角度(弧度)
	//      pDB            数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreateText(AcDbObjectId& resultId,AcGePoint3d& ptAlign,char* strText,char* strLyName="0",int  nClrIndex=0,char* strFontType="Standard",
		            int nHorzAlignType=1,int nVertAlignType=0,double dHeight=3.0,double dWidFactor=0.8,double dAngle=0.0, AcDbDatabase* pDB = CURDBPOINTER);



	//功能：创建多行文本
	//参数：resultId     返回创建实体的ID
	//      pt           文本插入点
	//      strText      文本内容
	//      strLyName    图层名
	//      nClrIndex    颜色号
	//      strFontType  字体
	//      nAlignType   对齐方式(0:左对齐,1:居中对齐,2:右对齐)
	//      dHeight      文字高度
	//      dAngle       旋转角度(弧度)
	//      pDB          数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreateMText(AcDbObjectId& resultId,AcGePoint3d& pt,char* strText,char* strLyName="0",int  nClrIndex=0, 
		             char* strFontType="Standard",int nAlignType=0,double dHeight=3.0,double dAngle=0.0,AcDbDatabase* pDB = CURDBPOINTER);

	//功能：设置CAD当前字体样式
	//参数：strTextStyleName  字体名
	//      pDB               数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long SetCurTextStyle(const char* strTextStyleName, AcDbDatabase* pDB = CURDBPOINTER);

	//功能：创建块引用
	//参数：resultId      返回创建实体的ID
	//      pt            块引用插入点
	//      strBlockName  块名称
	//      strLyName     图层名
	//      nClrIndex     颜色号
	//      strScale      缩放比例(x,y,z)参考格式 xxx#yyy#zzz#
	//      dAngle        旋转角度(弧度)
	//      pDB           数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreateBlockReference(AcDbObjectId& resultId,AcGePoint3d& pt,const char *strBlockName,const char *strLyName = "0",
		                     int  nClrIndex = 0,char *strScale ="1.0#1.0#1.0#",double dAngle = 0.0,AcDbDatabase* pDB = CURDBPOINTER); 

	//功能：创建点
	//参数：resultId      返回创建实体的ID
	//      pt            点坐标
	//      strLyName     图层名
	//      nClrIndex     颜色号
	//      pDB           数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreatePoint(AcDbObjectId& resultId,AcGePoint3d& pt,const char *strLyName="0",int nClrIndex=0,AcDbDatabase *pDB = CURDBPOINTER);


	//功能：创建线段
	//参数：resultId    返回创建实体的ID
	//      ptS,ptE     线段的起点和终点
	//      strLyName   图层名
	//      nClrIndex   颜色号
	//      strLnType   线型
	//      dLineWeigth 线宽
	//      dThickness  厚度
	//      pDB         数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreateLine(AcDbObjectId& resultId,AcGePoint3d& ptS,AcGePoint3d& ptE,const char* strLyName = "0",int nClrIndex =0,
	                const char* strLnType = "Continuous",AcDb::LineWeight dLineWeigth = AcDb::kLnWtByLwDefault, 
					double dThickness = 0.0,AcDbDatabase* pDB = CURDBPOINTER);


	//功能：创建二维多段线
	//参数：resultId      返回创建实体的ID
	//      ptArray       多段线的点集
	//      strLyName     图层名
	//      nClrIndex     颜色号
	//      strLnType     线型
	//      dLineWeigth   线宽
	//      dGlobeWeigth  全局宽度
	//      bIsClosed,    是否闭合
	//      dThickness    厚度
	//      pDB           数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreatePoly2dline(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName="0",int  nClrIndex=0,               
						  const char* strLnType="Continuous",AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,   
						  double dGlobeWeigth=0.0,bool   bIsClosed=false,double dThickness = 0.0,AcDbDatabase* pDB=CURDBPOINTER);


	//功能：创建多段线
	//参数：resultId      返回创建实体的ID
	//      ptArray       多段线的点集
	//      strLyName     图层名
	//      nClrIndex     颜色号
	//      strLnType     线型
	//      dLineWeigth   线宽
	//      dGlobeWeigth  全局宽度
	//      bIsClosed,    是否闭合
	//      dThickness    厚度
	//      pDB           数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreatePolyline(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName="0",int  nClrIndex=0,               
		                const char* strLnType="Continuous",AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,   
		                double dGlobeWeigth=0.0,bool   bIsClosed=false,double dThickness = 0.0,AcDbDatabase* pDB=CURDBPOINTER);

	long CreatePolylineX(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strLyName="0",int  nClrIndex=0,               
		                const char* strLnType="Continuous",AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,   
		                double dGlobeWeigth=0.0,bool   bIsClosed=false,double dThickness = 0.0,AcDbDatabase* pDB=CURDBPOINTER);
 

	//功能：创建圆
	//参数：resultId    返回创建实体的ID
	//      ptCen       中心点
	//      dRadius     半径
	//      strLyName   图层名
	//      nClrIndex   颜色号
	//      strLnType   线型
	//      dLineWeigth 线宽
	//      dThickness  厚度
	//      pDB         数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreateCircle(AcDbObjectId& resultId,AcGePoint3d& ptCen,double dRadius,const char *strLyName="0",int  nClrIndex=0,
		              const char* strLnType="Continuous",AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,double dThickness = 0.0,AcDbDatabase* pDB = CURDBPOINTER);


	//功能：创建圆弧
	//参数：resultId    返回创建实体的ID
	//      ptCen       中心点
	//      dRadius     半径
	//      dAngleStart 起点角度(弧度)
	//      dAngleEnd   终点角度(弧度)
	//      strLyName   图层名
	//      nClrIndex   颜色号
	//      strLnType   线型
	//      dLineWeigth 线宽
	//      dThickness  厚度
	//      pDB         数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreateArc(AcDbObjectId& resultId,AcGePoint3d& ptCen,double dRadius,double dAngleStart,double dAngleEnd,
		           const char *strLyName="0",int  nClrIndex=0,const char* strLnType="Continuous",
				   AcDb::LineWeight dLineWeigth= AcDb::kLnWtByLwDefault,double dThickness = 0.0,AcDbDatabase* pDB = CURDBPOINTER);


	//功能：生成多边形填充(无关联边界)
	//参数：resultId      返回创建实体的ID
	//      ptArray       点坐标数组
	//      strHatchName  填充样式名称
	//      dScale        填充图案比例
	//      dAngle        填充图案旋转角度
	//      strLyName     图层名称
	//      nClrIndex     颜色号
	//      pDB           数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreatePolyHatch(AcDbObjectId& resultId,AcGePoint3dArray& ptArray,const char *strHatchName,double dScale=1.0,
		                 double dAngle=0,const char *strLyName="0",int  nClrIndex=0,AcDbDatabase* pDB = CURDBPOINTER);

	
	//功能：生成多边形填充(关联边界)
	//参数：resultId      返回创建实体的ID
	//      dbObjIds      关联实体Id数组
	//      strHatchName  填充样式名称
	//      dScale        填充图案比例
	//      dAngle        填充图案旋转角度
	//      strLyName     图层名称
	//      nClrIndex     颜色号
	//      pDB           数据库对象指针
	//返回：成功返回 1 ，失败返回 0，其他参考定义
	long CreatePolyHatch(AcDbObjectId& resultId,AcDbObjectIdArray& dbObjIds,const char *strHatchName,double dScale=1.0,
		                 double dAngle=0,const char *strLyName="0",int  nClrIndex=0,AcDbDatabase* pDB= CURDBPOINTER);


public:

	//获取当前DWG文件的所有图层名
	long  GetAllLayerName(CStringArray& layerArray,AcDbDatabase* pDB = CURDBPOINTER);
	long  GetAllLayerName(CStringArray& layerArray,CStringArray& clrIndexArray,AcDbDatabase* pDB= CURDBPOINTER);

	//读写实体的扩展数据
	void SetXData(AcDbEntity* pEnt,CStringArray& strValue,const char* strRegName="DIST_XDATA");
	void GetXData(AcDbEntity* pEnt,CStringArray& strValue,const char* strRegName="DIST_XDATA");

public:
	//辅助函数

	// 查找CAD实体线型
	long GetLineTypeId(AcDbObjectId& resultId,const char* strLnTypeName,AcDbDatabase* pDB = CURDBPOINTER);

	// 加载CAD实体线型
	long LoadLineType(AcDbObjectId& resultId,const char* strLnTypeName,AcDbDatabase* pDB = CURDBPOINTER);

	// 线宽类型转换位Double
	double LnWeightToDouble(AcDb::LineWeight type);
	AcDb::LineWeight DoubleToLnWeight(double dLnWid);

	//创建字体样式，并返回ID
	long CreateTextStyle(AcDbObjectId& resultId,const char* csTsName,const char* szFlNm,const char* szBigFlNm,AcDbDatabase* pDB = CURDBPOINTER);

	//获取字体样式ID
	long GetTextStyleObjID(AcDbObjectId& resultId,const char* strTextStyleName,AcDbDatabase* pDB = CURDBPOINTER);

	//根据字体样式ID得到字体名称
	long GetTextStyleName(char *strTextStyleName,AcDbObjectId& ObjId,AcDbDatabase *pDB = CURDBPOINTER);

	// 添加实体到模型空间
	long AddToCurrentSpace(AcDbObjectId& resultId,AcDbEntity* pEnt,AcDbDatabase* pDB = CURDBPOINTER);

	//
	long GetPoly2dlinePts(AcDbObjectId poly2dlineId,AcGePoint3dArray& ptArray);
	long GetPoly2dlinePts(AcDb2dPolyline* pPolyline2d,AcGePoint3dArray& ptArray);

	long GetPolylinePts(AcDbObjectId polylineID,AcGePoint3dArray& ptArray);
	long GetPolylinePts(AcDbPolyline* pPolyline,AcGePoint3dArray& ptArray);

	long GetPoly3dlinePts(AcDbObjectId poly3dlineId,AcGePoint3dArray& ptArray);
	long GetPoly3dlinePts(AcDb3dPolyline* pPolyline3d,AcGePoint3dArray& ptArray);

	long GetSplinePts(AcGePoint3dArray& ptArray,AcDbSpline* pSpline,int nSegPartNum=10);
	long GetEllipsPts(AcGePoint3dArray& ptArray,AcDbEllipse* pE,int nSegPartNum=10);
	long GetArcPts(AcGePoint3dArray& ptArray,AcDbArc* pArc,int nSegPartNum=10);
	long GetCirclePts(AcGePoint3dArray& ptArray,AcDbCircle* pCircle,int nSegPartNum=10);

	void TransformToWcs(AcDbEntity* ent, AcDbDatabase* pDB);

	long DyZoom(AcGePoint2d& CenterPt,double dHeight,double dWidth);

	//得到文本边界
	long GetTextBoundary(AcGePoint3dArray& ptArray,AcDbObjectId& objectId,double dOffset=0);

public:
	AcGeVector3d Convert2dVectorTo3d(AcGeVector2d pt);
	AcGeVector2d Convert3dVectorTo2d(AcGeVector3d pt);
	AcGePoint3d Convert2dPointTo3d(AcGePoint2d pt);
	AcGePoint2d Convert3dPointTo2d(AcGePoint3d pt);
	void ReduceBulge(AcGePoint3dArray &ptDfArr,AcGePoint3d ptA,AcGePoint3d ptB,double dBulge,int nPartNum=10);

	//已知一段弧的起点和终点以及其凸度,求其圆心,和弧长
	long GetArcCenterPt(ads_point startPoint,ads_point endPoint,double bulge,ads_point& center,double& dArcLen);
	
	void PolylinePtUniteArc(AcDbObjectId objId, int Step);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//函数功能：求线段BL,线段BR的夹角，并确定BL，BR那个线段在左侧，那个线段在右侧
	//入口参数：
	//          bx,by,lx,ly    线段BL 
	//          .. ..,rx,ry    线段BR
	//出口参数：
	//          dAngle         夹角大小(弧度)
	//返回值：-1:顺时针   0 共线   1 逆时针
	////////////////////////////////////////////////////////////////////////////////////////////////
	long AngleWithTowLineSect(double lx,double ly,double bx,double by,double rx,double ry,double& dAngle);

	long SimulationArc(AcGePoint3dArray& ptArray,AcGeDoubleArray& resultBlgArray);

	void OpenDwgFile(CString strFileName);

	void SendCommandToAutoCAD(CString cmd);

	void ShowCommandLineWindow(BOOL bShow = TRUE);

	BOOL OpenOrCloseLayer(BOOL bOpen, CStringArray &strCadLyNameArray);
	BOOL InsertBlkAttr(char* strBlkName,CStringArray& strAttrArray,char* strTitleName,int nColorIndex);
	BOOL InsertBlkAttrRef(AcDbObjectId& resultId,AcGePoint3d pt,char* strBlkName,char* strCadLyName,
		                  double dFontHeight,CStringArray& strTagArray,CStringArray& strTextArray);

	BOOL CreateBlkRefAttr(AcDbObjectId& resultId,AcGePoint3d basePt,char* strBlkName,char* strLyName,
		                  int  nClrIndex,char *strScale,double dAngle,//double dFontHeight,
		                  CStringArray& strTagArray,CStringArray& strTextArray,AcDbDatabase* pDB = CURDBPOINTER);

	AcDbViewTableRecord GetCurrentView();
	AcDbObjectId CreateBlockByDwgFile(CString strDwgFileName);

private:
	static void OpenDocHelper(void *pData);
};


#endif  //__DIST_ENTITY_TOOL_H__