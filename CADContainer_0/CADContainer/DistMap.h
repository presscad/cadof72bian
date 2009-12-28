#pragma once
#include "stdafx.h"
#include "DistConnection.h"

//SDE图层类型
enum DIST_ENUM_SHAPETYPE     
{
	kShapeNil        = 1L,
	kShapePoint      = (1L<<1),
	kShapeLine       = (1L<<2),
	kShapeSimpleLine = (1L<<3),
	kShapeArea       = (1L<<4),
	kShapeMultiPart  = (1L<<18)
};

class CDistMap
{
public:
	CDistMap(void);
	~CDistMap(void);

public:
	//当前CAD模型中AcDbDatabase
	AcDbDatabase * m_lpCADDatabase;
	CDistConnection * m_lpDataConnection;

public:

	//CAD操作

	// 查找CAD实体线型
	//参数：strLnTypeName   线型名
	//返回：AcDbObjectId    返回线型ID 成功返回查找到线型名对应线型的ID ，失败返回NULL
	AcDbObjectId CAD_GetLineTypeId(const char* strLnTypeName);

	// 加载CAD实体线型
	//参数：strLnTypeName   线型名
	//返回：AcDbObjectId    返回线型ID 成功返回新加载线型名对应线型的ID ，失败返回NULL
	AcDbObjectId CAD_LoadLineType(const char* strLnTypeName);

	//功能：创建图层
	//参数：strLyName   图层名
	//      nClrIndex   颜色号
	//      strLnType   线型
	//      dLineWeigth 线宽
	//返回：AcDbObjectId    返回创建实体的ID 成功返回新建Layer ID ，失败返回NULL
	AcDbObjectId CAD_CreateLayer(const char *strLyName, int  nClrIndex,const char *strLnType, AcDb::LineWeight dLineWeigth);



	//SDE操作

	// 功能：创建SDE图层(存放CAD实体对象)
	// 参数：strTableName      SDE关联的属性数据表名称
	//       strSpacialFldName 图层与属性表关联的空间字段名(图层创建后会在属性表中增加该字段)
	//       dScale            图层比例尺
	//       ox,oy             坐标系原点
	//       dGridSize         网格大小
	//       lTypeMask         图层类型
	//       strKeyword        关键字
    // 返回：成功返回 1，失败返回 0，其他参考定义
	long SDE_CreateLayer(const char* strTableName,
		                 const char* strSpacialFldName,
						 double dScale,
						 double ox = 0,
						 double oy = 0,
						 double dGridSize = 1000,
						 long   lTypeMask = kShapeNil | kShapePoint |
						                    kShapeLine | kShapeSimpleLine|
						                    kShapeArea | kShapeMultiPart,
						 const char* strKeyword="DEFAULT");
};
