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
//         CDistClipEntity::ReduceEntities(SelectPtArray,"LyName"); 
//备注：在裁减时，注意视口范围，否则将搜索不到实体
//修改记录：
//    由于开始的图框是直接绘制的,样式无变化,局限性比较大;后来改为使用插入属性块的方式(插入属性块时属性位置需要处理)
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __DISTREDUCE__ENTITY__H__
#define  __DISTREDUCE__ENTITY__H__

#include "StdAfx.h"

typedef CArray<AcGePoint3dArray,AcGePoint3dArray&> RECT_ACGE_ARRAY;

class CDistClipEntity
{
public:
	         CDistClipEntity(void);
	virtual ~CDistClipEntity(void);

public:

	//功能：测试例程
	//参数：无
	//返回：无
	static BOOL Plot();

	static BOOL PlotEx();

	static BOOL PlotByPaperSpace();

	static BOOL GetBlockNameByBlkId(CString &strBlkName,AcDbObjectId blockId);
	static BOOL FindPaperSpaceAndInsertBlkRefInfo(AcDbObjectIdArray& ObjIdArray);
	static BOOL FindPaperSpaceBlkId(AcDbObjectIdArray& ObjIdArray);

	static BOOL SaveToDwgFile(CString strFName,AcDbObjectIdArray& ObjIdArray);
	static BOOL SaveToDwgFile(CString strPrjName,AcGePoint3d& ptCen,double dH,double dW);

	static BOOL DrawCustomGrid(AcGePoint3dArray& ptArray,AcDbObjectIdArray& ObjIdArray,char* strLyName,double dEdge,double dCross,double dCrossDist,CStringArray& strValueArray);
	//功能：根据边界多边形对实体进行裁减复制
	//参数：ObjIdArrayReturn 返回裁减范围内的实体ID
	//      SelectPtArray    边界数组(点数组一律视为闭合，如果不闭合会增加一个首点；所以需要值传递)
	//      strLyName        操作图层名称，如果为NULL表示操作所有图层
	//返回：成功返回 TRUE 失败返回 FALSE
	static BOOL ClipAndCopy(AcDbObjectIdArray& ObjIdArrayReturn,const char* strLyName,AcGePoint3dArray SelectPtArray,const char* strLyNameSelect=NULL);
	static void CloneBlockRefAttr(AcDbObjectIdArray& newObjIdArray,const char* strLyName,AcDbObjectIdArray oldObjIdArray);


	//功能:实现多边形与矩形求交,结果返回零个或多个多边行点数组
	static void PolyIntersectWithRect(RECT_ACGE_ARRAY& resultArray,AcGePoint3dArray& polyPtArray,AcGePoint3dArray& rectPtArray);

	static void SelectDkAndHighLight();

	static void ReadDwgCT();

private:
	static void CalcRectByScale(double dScaleX,double dScaleY,AcGePoint3d basePt,AcGePoint3dArray& pts);
	static BOOL CreateGroup(AcDbObjectId& groupId,AcDbObjectIdArray& objIds,char* strGroupName);
	static BOOL GetBckRefInf(const AcDbObjectId & BckRefId, CStringArray &strTags, CStringArray &strContents);
	static BOOL GetBlockNameByBlkRefId(CString &strName,AcDbObjectId objId);
	static void GetProjectInfo(CStringArray& strValueArray,AcGePoint3dArray& SelectPtArray);
	
	static CString GetCurArxAppPath(CString strArxName);
	static void CreateBlockByFile();
	static void CloneEntity(AcDbObjectIdArray& newObjIdArray,const char* strLyName,AcDbObjectIdArray oldObjIdArray,AcDbObjectId* pOwnerObjId=NULL);
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


	static AcDbObjectId FindBlkRef(CString strBlkName);
	//static BOOL UpdateAttrToBlkRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue);
	static BOOL UpdateAttrToBlkRef(AcDbObjectId ObjIdRef,CStringArray& strArrayTag,CStringArray& strArrayValue,
		                           CStringArray& strArrayA1,CStringArray& strArrayA2,CStringArray& strArrayA3,CStringArray& strArrayA4);
	static BOOL ReadAttrFromRef(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen);
	//static BOOL ReadAttrFromRefDk(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen);
	static BOOL ReadAttrFromRefDk(CStringArray& strArrayTag,CStringArray& strArrayValue,AcGePoint3d& ptCen,
		                          CStringArray& strArrayA1,CStringArray& strArrayA2,CStringArray& strArrayA3,CStringArray&strArrayA4);
};


#endif  //__DISTREDUCE__ENTITY__H__

