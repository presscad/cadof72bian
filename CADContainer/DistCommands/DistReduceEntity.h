////////////////////////////////////////////////////////////////////////////////////////////////////////////
//类名称  ：实现对图形的多边形裁减
//实现思路：1.根据多边形搜索出其包含的实体集合，放入数组A中
//          2.再根据多边形搜索出与其相交的实体集合，放入数组B中
//          3.依次循环数组B
//            a.从数组B中取一实体
//            b.将它转化成AcDbCurve类型
//            c.与多边形求交，得到一个或多个子AcDbCurve
//            d.判断每一个子AcDbCurve是否在多边形内，将在多边形内的子实体添加到模型空间，
//              并将ObjectId添加到数组A中；重复a,b,c,d直到遍历完所有实体
//          4.删除数组B中的实体
//使用说明：
//         CDistReduceEntity::ReduceEntities(SelectPtArray,"LyName"); 
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DISTREDUCE__ENTITY__H__
#define  __DISTREDUCE__ENTITY__H__

#include "StdAfx.h"

class CDistReduceEntity
{
public:
	         CDistReduceEntity(void);
	virtual ~CDistReduceEntity(void);

public:


#ifdef _DEBUG
	//功能：测试例程
	//参数：无
	//返回：无
	static BOOL Test();
#endif

	
	//功能：根据边界多边形对实体进行裁减
	//参数：SelectPtArray  边界数组(点数组一律视为闭合，如果不闭合会增加一个首点；所以需要值传递)
	//      strLyName      操作图层名称，如果为NULL表示操作所有图层
	//返回：成功返回 TRUE 失败返回 FALSE
	static BOOL ReduceEntities(AcGePoint3dArray SelectPtArray,const char* strLyName=NULL);

	static BOOL ReduceEntitiesRect(AcGePoint3dArray SelectPtArray,const char* strLyName=NULL);

	
private:

	//功能：判断两线段是否相交
	//参数：px1,py1,px2,py2   线段P1P2
	//      qx1,qy1,qx2,qy2   线段Q1Q2
	//返回：如果相交返回TRUE 否则返回FALSE 。
	static BOOL IsLineSectIntersectLineSect(double px1,double py1,double px2,double py2,
		                             double qx1,double qy1,double qx2,double qy2);

	//功能：判断点是否在线段上
	//参数：px1,py1,px2,py2  线段P1P2
	//      qx,qy            点Q
	//返回：如果点在线上返回 TRUE  否则返回FALSE 。
	static BOOL IsPointOnLine(double px1,double py1,double px2,double py2,double qx,double qy);


	//功能：判断点是否在多边形中
	//参数：pts  多边形点集合
	//      x,y  点坐标
	//返回：如果点在多边形内返回 TRUE 否则返回 FALSE       
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	static BOOL  IsPointInPolygon(AcGePoint3dArray& pts,double x,double y);

    //功能：将当前实体添加到模型空间
	//参数：resultId  返回添加实体的ID
	//      pEnt      实体指针
	//      pDB       当前文档数据库指针
	//返回：添加实体成功返回 TRUE 否则返回 FALSE  
	static BOOL AddToCurrentSpace(AcDbObjectId& resultId,AcDbEntity* pEnt,AcDbDatabase* pDB=acdbHostApplicationServices()->workingDatabase());

	static void ArxZoom(ads_point pt,double magnf);
};


#endif  //__DISTREDUCE__ENTITY__H__

