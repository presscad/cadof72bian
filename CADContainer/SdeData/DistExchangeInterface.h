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
//版权所有 (C) 2007－2010 Dist Inc. 保留所有权利。                           //
//作者：王                                                                   //
//日期：2007年4月,上海.                                                      //
//修改：                                                                     //
///////////////////////////////////////////////////////////////////////////////
#ifndef  __EXCHANGEDATAINTERFACE__H__
#define  __EXCHANGEDATAINTERFACE__H__

#include "DistBaseInterface.h"


//=============================  接口定义说明 ===============================//
// 该接口主要完成SDE数据到CAD图形实体的互相转换
#ifdef DIST_EXCHANGEDATA_EXPORTS
#define DIST_EXCHANGE_API __declspec(dllexport)
#else
#define DIST_EXCHANGE_API __declspec(dllimport)
#endif

struct IDistExchangeData;

//创建SDE库和CAD数据互操作对象
DIST_EXCHANGE_API IDistExchangeData* WINAPI CreateExchangeObjcet(void); 


//功能： 检测指定坐标范围是否与某个图层上的实体交叉或重叠
//参数： pConnect
//       strSdeLyName
//       strSQL
//       ptArray[注意闭合]
//       nPtCount  
//返回：0 失败，1 没有交叉重叠，2有重叠交叉
//"F:OBJECTID,T:%s,W:PROJECT_ID<>'%s',##"
DIST_EXCHANGE_API long WINAPI gCheckCrossOrContain(IDistConnection* pConnect,const char* strSdeLyName,
												   const char*strSQL, IDistPoint* ptArray,int nPtCount);


enum DIST_ENUM_SELECTTYPE
{
	kSM_ENVP            = 0 ,  // ENVELOPES OVERLAP 
	kSM_ENVP_BY_GRID    = 1 ,  // ENVELOPES OVERLAP 
	kSM_CP              = 2 ,  // COMMON POINT 
	kSM_LCROSS          = 3 ,  // LINE CROSS 
	kSM_COMMON          = 4 ,  // COMMON EDGE/LINE 
	kSM_CP_OR_LCROSS    = 5 ,  // COMMON POINT OR LINE CROSS 
	kSM_LCROSS_OR_CP    = 5 ,  // COMMON POINT OR LINE CROSS 
	kSM_ET_OR_AI        = 6 ,  // EDGE TOUCH OR AREA INTERSECT 
	kSM_AI_OR_ET        = 6 ,  // EDGE TOUCH OR AREA INTERSECT 
	kSM_ET_OR_II        = 6 ,  // EDGE TOUCH OR INTERIOR INTERSECT 
	kSM_II_OR_ET        = 6 ,  // EDGE TOUCH OR INTERIOR INTERSECT 
	kSM_AI              = 7 ,  // AREA INTERSECT 
	kSM_II              = 7 ,  // INTERIOR INTERSECT 
	kSM_AI_NO_ET        = 8 ,  // AREA INTERSECT AND NO EDGE TOUCH 
	kSM_II_NO_ET        = 8 ,  // INTERIOR INTERSECT AND NO EDGE TOUCH 
	kSM_PC              = 9 ,  // PRIMARY CONTAINED IN SECONDARY 
	kSM_SC              = 10,  // SECONDARY CONTAINED IN PRIMARY 
	kSM_PC_NO_ET        = 11,  // PRIM CONTAINED AND NO EDGE TOUCH 
	kSM_SC_NO_ET        = 12,  // SEC CONTAINED AND NO EDGE TOUCH 
	kkSM_PIP            = 13,  // FIRST POINT IN PRIMARY IN SEC 
	kSM_IDENTICAL       = 15,  // IDENTICAL 
	kSM_CBM				= 16   // Calculus-based method [Clementini] 
};
     

//空间过滤条件，位置结构
//结构定义说明：
//nEntityType  1 点  2 线  3圆  4 矩形  5多边形
//nSelectType  具体含义参考上面的DIST_ENUM_SELECTTYPE的定义
//ptArray      具体内容根据nEntityType的值来确定
//             1    一个点
//             2    两个或多个点
//             3    两个点；第一个点为圆心，第二个点的X值为半径
//             4    两个点；第一个点为矩形最小点，第二个点为矩形最大点
//             5    四个或更多点，数组的第一个点与最后一个点相等
struct DIST_STRUCT_SELECTWAY 
{
	int nEntityType;       
	int nSelectType;
	AcGePoint3dArray ptArray;  
	       
};

struct IDistExchangeData
{
public:

	//功能：释放对象
	//参数：无
	//返回：无
	virtual void Release(void)=0;

	//功能：设置数据库反应器开关
	//参数：bOpen  是否打开数据库反应器
	//返回：无
	virtual void SetDbReactorStatues(bool bOpen=true)=0;

	//功能：获取当前SDE层对应的CAD层名称
	//参数：返回CAD图层名称
	//返回：无
	virtual void GetCurCadLyName(char* strCadLyName)=0;

	//功能：获取指定搜索条件下SHAPE的点数组
	//参数：strSQL  搜索条件
	//      ptArray 返回Shape结构的点数组
	//返回：无
	virtual void GetSearchShapePointArray(IDistConnection* pConnection, const char* strSQL,AcGePoint3dArray* ptArray) = 0;

	virtual int CopyRecordFromOneToAnother(IDistConnection* pConnection, LPCTSTR lpSrcTable, LPCTSTR lpDestTable, LPCTSTR lpWhere) = 0;

	virtual void GetSearchFieldArray(IDistConnection* pConnection, LPCTSTR strSQL, LPCTSTR lpTable, LPCTSTR lpField, DIST_STRUCT_SELECTWAY* pFilter, CStringArray& array) = 0;


	//功能：读符号画配置表信息
	//参数：pConnect              数据库连接对象指针
	//      strSymbolTableName    符号画表名称
	//      strCadMapSdeTableName SDE与CAD图层对照表
	//      strSdeLyName          SDE图层名称
	//      strXDataName          扩展属性XData注册名称
	//      pXdataArray           需要写入实体扩展属性的字段数组
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long ReadSymbolInfo(IDistConnection *pConnect,
		                        const char* strSymbolTableName,
		                        const char *strCadMapSdeTableName,
								const char *strSdeLyName,
								const char* strXDataName="DIST_XDATA",
								CStringArray* pXdataArray=NULL)=0;

	// 通过CADMAPTOSDE表读取CAD图层的样式（颜色，线型，线宽）
	virtual long ReadCadLayerInfo(IDistConnection * pConnection, LPCTSTR lpCadLayer, int& nColorIndex, CString& lpLineType, AcDb::LineWeight& dLineWeigth) = 0;


	//功能：读SDE数据到CAD中显示
	//参数：pObjIdArray  返回绘制CAD实体的ID数组(类型为AcDbObjectIdArray)
	//      pFilter      空间查询方式
	//      strUserWhere 用户自定义的SQL查询条件
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long ReadSdeDBToCAD(void* pObjIdArray,
		                        DIST_STRUCT_SELECTWAY* pFilter=NULL,
		                        const char* strUserWhere=NULL,BOOL bSingleToSingle = TRUE,BOOL bIsReadOnly=FALSE, BOOL bDeleteExistEntity=TRUE)=0;


	//功能：保存CAD实体信息到SDE中
	//参数：SelectWay           空间查询方式
	//      bAfterSaveToDelete  保存后是否从CAD图层中删除对应的实体
	//返回：成功返回 1，失败返回 0，-1 表示条件不满足，其他参考定义
	virtual long SaveEntityToSdeDB(DIST_STRUCT_SELECTWAY* pFilter=NULL,BOOL bAfterSaveToDelete= TRUE,BOOL bSingleToSingle = TRUE)=0;

	virtual long SaveEntityToSdeDB(AcDbObjectIdArray& objIDs, BOOL bAfterSaveToDelete=TRUE, BOOL bSingleToSingle = TRUE)=0;

	virtual long SaveEntityToSdeDB(AcDbObjectIdArray& objIDs, CString strXMID, BOOL bAfterSaveToDelete=TRUE, BOOL bSingleToSingle = TRUE)=0;

	// 保存新增对象
	virtual long SaveInsertEntityToSdeDB(AcDbObjectIdArray& objIds, BOOL bAfterSaveToDelete=TRUE, BOOL bSingleToSingle = TRUE)=0;

	// 保存修改对象
	virtual long SaveModifyEntityToSdeDB(AcDbObjectIdArray& objIds, BOOL bAfterSaveToDelete=TRUE, BOOL bSingleToSingle = TRUE)=0;
};

#endif//__EXCHANGEDATAINTERFACE__H__