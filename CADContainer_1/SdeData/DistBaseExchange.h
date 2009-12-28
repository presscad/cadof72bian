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

#ifndef __CBASESDEENTITY__H__
#define __CBASESDEENTITY__H__

//////////////////////////////////////////////////////////////////////////////////////////
// 功能结构：
// CExchangeData 负责对接口的实现
// CBaseExchange 负责对CExchangeData功能的拆分实现
//                    CPointExchange， CAnnotationExchange，...
//修改记录：1.增加了多点，多线，多面的处理；建立实体之间的联系
//          2.修改ReadSdeDBToCAD函数，提供用户自定义SQL查询语句
//          3.增加了SDE图层与CAD图层，一对一，一对多，多对一的识别
//          4.增加了对符号化信息的模糊解析
//          5.SDE图层通配符的识别

#include <sdeerno.h>
#include <sdetype.h>
#include <sderaster.h>
#include <pe.h>
#include <pedef.h>
#include <pef.h>


#include "DistBaseInterface.h"
#include "DistExchangeInterface.h"
#include "DistEntityTool.h"
#include "DistDatabaseReactor.h"



//类声明
class CBaseExchange;
class CPointExchange;
class CTextExchange;
class CPolylineExchange;
class CPolygonExchange;
class CMultpatchExchange;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DIST_SYMBOLFLD;
struct DIST_FILLCOLORMAP;

typedef CArray<DIST_FILLCOLORMAP,DIST_FILLCOLORMAP&> DIST_COLORMAPARRAY;

DIST_POINT_STRUCT* gPtArray = NULL;


class CExchangeInfo
{
public:
	         CExchangeInfo();
	virtual ~CExchangeInfo();

public:
	IDistConnection    *m_pConnection;                    //连接对象指针
	char                m_strCadLyName[160];              //CAD图层名称
	char                m_strSdeLyName[160];              //SDE图层名
	char                m_strSdeLyKind[60];               //SDE图层类型

	DIST_SYMBOLFLD     *m_pSymbolInfo;	                  //符号画信息数组	            
	int                 m_nSymbolCount;                   //数组大小
	int                 m_nPos;                           //标记当前读取符号画记录

	IDistParameter     *m_pParam;                         //字段信息数组
	int                 m_nParamCount;                    //字段数量

	long                m_nShapeType;                     //图层类型
	SE_COORDREF         m_Coordref;				          //坐标系

	char                m_strRowIdFldName[60];            //RowId字段名称
	char                m_strShapeFldName[60];            //Shape字段名称

	CStringArray        m_strFldNameArray;                //符号画字段
	CStringArray        m_strXDataArrayOut;               //附加到实体扩展属性的字段

	char                m_strXDataName[255];              //XData扩展属性注册名称

	CDistEntityTool     m_EntTool;                        //实体工具对象

	DIST_COLORMAPARRAY  m_MapColorArray;                  //用地性质和颜色对照表

private:

	//禁用操作
	                    CExchangeInfo(const CExchangeInfo&);
	CExchangeInfo&	    operator=(const CExchangeInfo&);

};



//接口实现类
class CExchangeData : public IDistExchangeData
{
public:
	//构造和析构
	         CExchangeData(void);
	virtual ~CExchangeData(void);

public:

	//功能：释放对象
	//参数：无
	//返回：无
	virtual void Release(void);

	//功能：获取当前SDE层对应的CAD层名称
	//参数：返回CAD图层名称
	//返回：无
	virtual void GetCurCadLyName(char* strCadLyName);

	//功能：获取指定搜索条件下SHAPE的点数组
	//参数：strSQL  搜索条件
	//      ptArray 返回Shape结构的点数组
	//返回：无
	virtual void GetSearchShapePointArray(const char* strSQL,AcGePoint3dArray* ptArray);

	//功能：设置数据库反应器开关
	//参数：bOpen  是否打开数据库反应器
	//返回：无
	virtual void SetDbReactorStatues(bool bOpen=true);

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
								CStringArray* pXdataArray=NULL);

	//功能：读SDE数据到CAD中显示
	//参数：pObjIdArray  返回绘制CAD实体的ID数组(类型为AcDbObjectIdArray)
	//      pFilter      空间查询方式
	//      strUserWhere 用户自定义的SQL查询条件
	//返回：成功返回 1，失败返回 0，-1 表示条件不满足，其他参考定义
	virtual long ReadSdeDBToCAD(void* pObjIdArray,
		                        DIST_STRUCT_SELECTWAY* pFilter=NULL,
		                        const char* strUserWhere=NULL,BOOL bSingleToSingle = TRUE,BOOL bIsReadOnly=FALSE);


	//功能：保存CAD实体信息到SDE中
	//参数：SelectWay           空间查询方式
	//      bAfterSaveToDelete  保存后是否从CAD图层中删除对应的实体
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long SaveEntityToSdeDB(DIST_STRUCT_SELECTWAY* pFilter=NULL,BOOL bAfterSaveToDelete= TRUE,BOOL bSingleToSingle = TRUE);

private:
	
	//功能：根据提供的位置结构信息，生成空间过滤条件
	//参数：pFilterInfo  位置结构信息
	//      pFilter      空间过滤条件
	//返回：成功返回 1，失败返回 0，其他参考定义
	long   CreateFilter(DIST_STRUCT_SELECTWAY* pFilterInfo,SE_FILTER *pFilter);

	//功能：删除当前查询范围内中的所有实体(避免重复绘制)
	//参数：strCadName   CAD图层名
	//      strSdeName   SDE图层名(如果没有SDE图层条件，说明CAD层与SDE层是多对一或一对一)
	//      pFilter      空间过滤条件
	//返回：成功返回 1，失败返回 0，其他参考定义
	long   DeleteEntityInCAD(const char* strCadName,const char* strSdeName=NULL,
		                     DIST_STRUCT_SELECTWAY* pFilter=NULL);

	//功能：选择当前查询范围内中的所有实体(避免重复提交)
	//参数：strCadName   CAD图层名
	//      strSdeName   SDE图层名(如果没有SDE图层条件，说明CAD层与SDE层是多对一或一对一)
	//      pFilter      空间过滤条件
	//返回：成功返回 1，失败返回 0，其他参考定义
	long   SelectEntityInCAD(AcDbObjectIdArray& ObjIdArray,const char* strCadName,
		                     const char* strSdeName=NULL,DIST_STRUCT_SELECTWAY* pFilter=NULL);

	//功能：根据图数据库信息创建CAD图层
	//参数：strCadMapSdeTableName SDE与CAD图层对照表
	//      strSdeLyName          SDE图层名称
	//返回：成功返回 1，失败返回 0，其他参考定义
	long   ReadInfoToCreateCadLayer(const char *strCadMapSdeTableName,const char* strSdeName);

	//功能：判断两个字符串是否通配(例如：abc_*_cd  == abc_sadfsadfsafsdf_cd)
	//参数：strSub  子字符串
	//      strTP   通配字符串
	//返回：如果直接相等返回2，通配返回 1，不通配返回0，其他参考定义
	long   StringLikeString(CString strSub,CString strTP);


	//功能：读符号化配置信息到列表结构中保存
	//参数：strSymbolTableName    符号画表名称
	//      strSdeLyName          SDE图层名称
	//返回：成功返回 1，失败返回 0，其他参考定义
	long  ReadSymbolListToSave(const char *strSymbolTableName,const char* strSdeName);

	//功能：读用地性质和颜色对照表
	//参数：strMapTName    对照表名称
	//      strSdeLyName   SDE图层名称
	//返回：成功返回 1，失败返回 0，其他参考定义
	long  ReadYdxzColorMap(const char* strMapTName,const char* strSdeLyName);

private:
	CExchangeInfo   m_info;  //信息结构
	
};






//===============================================================================================================//
//符号表字段定义如下：
// 编号  Sde图层名    类型       查询条件      字段1   字段2   字段3   字段4   字段5   字段6  字段7  字段8 字段9 字段10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3    Fld4    Fld5    Fld6    Fld7  Fld8  Fld9   Fld10
class CBaseExchange 
{
public:
	
	//构造和析构
	         CBaseExchange(void);
			 CBaseExchange(CExchangeInfo* pInfo);
	virtual ~CBaseExchange(void);

public:

    //功能：在CAD中绘制实体
	//参数：resultId    返回添加实体ID
	//      pRcdSet     查询记录集合
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//功能：将CAD实体保存到SDE库中
	//参数：pRowId,    返回SDE记录得RowId
	//      pCommand   数据库命令对象
	//      ObjId      CAD实体ID
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);

	//功能：判断当前操作实体是否在已经操作实体数组中存在
	//参数：tempObjId   判断实体ID
	//返回： 存在返回 1，不存在返回 0 ，其他参考定义
	//备注：该函数主要针对多部分SHAPE，就是一个SHAPE对应多个关联 CAD 实体；为提高效率，提交时，遍历到其中一个
	//      子实体，就会将其他所有成员一并提交，然后在数组中记录他们已经操作过，避免重复操作
	long IsObjectHaveUsed(AcDbObjectId& tempObjId);

public:

	//根据字符串信息，获取角度信息
	//参数： dAngle   角度返回值（角度）
	//       strAngle 字符信息
	//返回：无
	//数据形式：角度  或  弧度#
	static void ParseAngleInfo(double& dAngle,const char* strAngle);


	//根据字符串信息，获取颜色序号
	//参数： nColorIndex   返回颜色序号
	//       strColor      字符信息
	//返回：无
	//数据形式：序号  或  R,G,B#
	static void ParseColorInfo(int& nColorIndex,const char* strColor,DIST_COLORMAPARRAY* pData=NULL);



	//根据字符串信息，获取块比例
	//参数： dSx,dSy,dSz   块X，Y，Z方向的比例系数
	//       strScale      字符信息
	//返回：无
	//数据形式：X比例，Y比例，Z比例 （可以是X，或X，Y 或 X，Y，Z）
	static void ParseScaleInfo(double& dSx,double& dSy,double& dSz,const char* strScale);


	//根据字符串信息，获取文字高度
	//参数： dHeight       返回文字高度
	//       strTextHeight 字符信息
	//返回：无
	//数据形式：文字高度 或 文字高度，比例系数#
	static void ParseTextHeight(double& dHeight,const char* strTextHeight);


	//根据字符串信息，获取CAD字体样式
	//参数： strStyle       返回字体样式
	//       strTextStyle   字符信息
	//返回：无
	//数据形式：CAD字体样式  或  字体#
	static void ParseTextStyle(char *strStyle,const char* strTextStyle);


	//根据字符串信息，获取横向对齐方式
	//参数： nAlign       返回对齐方式（0左对齐  1中心对齐  2右对齐）
	//       strAlign     字符信息
	//返回：无
	//数据形式：左，中，右  或  Left，Mid，Right 或 0，1，2
	static void ParseTextAlign(int& nAlign,const char* strAlign);

	static void ParseLnWidth(double& dLnWidth,const char* strLnWidth);

protected:
	//－－－－－－－－－－－－－辅助函数－－－－－－－－－－－－－//

	//功能：根据实体ID，读取扩展属性中的RowID等信息
	//参数：nSymbolPos  符号化信息在其数组中的位置
	//      nRowId      返回SDE记录编号
	//      nPartNum    返回部分数量
	//      ObjIdArray  返回所以部分对应Entity的ObjectId
	//      ObjId       CAD实体ID
	//返回：成功返回 1，失败返回 0，其他参考定义
	long CheckObjAndReadXDataInInfo(int& nIsExists,int& nSymbolPos,int& nRowId,int& nPartNum,AcDbObjectIdArray& ObjIdArray,AcDbObjectId& ObjId);


	//功能：将内部和外部使用的信息写入实体的扩展属性
	//参数：strNameArray      内部字段名称
	//      nPos              在符号化信息结构中的次序
	//      strRowId          实体在SDE库中对应记录的唯一编号
	//      strObjArray       内部字段对应值
	//      strOutXDataArray  外部字段对应值
	//返回：成功返回 1，失败返回 0，其他参考定义
	long WriteXDataToObjects(CStringArray& strNameArray,int nPos,const char* strRowId,CStringArray& strObjArray,CStringArray& strOutXDataArray);


	//功能：更新数据记录（以字符串形式提供修改值）
	//参数：pCommand              执行命令对象指针
	//      strFldNameArray       修改记录字段名
	//      strFldValueArray      修改记录值
	//      nRowId                记录的编号
	//      nInsertOrUpdate       功能标记（1 表示新增插入， 2 表示修改更新）
	//返回：无
	long SaveValuesToSDE(IDistCommand* pCommand,CStringArray& strFldNameArray,CStringArray& strFldValueArray,int& nRowId,int nInsertOrUpdate=2);

	//功能：从数组中获取指定名称字段的值
	//参数：strValue          返回值
	//      strName           指定名称
	//      strFldNameArray   指定名称数组
	//      strFldValueArray  指定值数组
	//返回：无
	void GetValueFromArray(CString& strValue,CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray);

	//功能：保存指定名称的值到数组中
	//参数：strValue          指定值
	//      strName           指定名称
	//      strFldNameArray   指定名称数组
	//      strFldValueArray  指定值数组
	//返回：无
    void SetValueToArray(CString strValue,CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray);

	//功能：从数组中删除指定名称和值
	//参数：strName           指定名称
	//      strFldNameArray   指定名称数组
	//      strFldValueArray  指定值数组
	//返回：无
	void DeleteValueFromArray(CString strName,CStringArray&strFldNameArray,CStringArray& strFldValueArray);

	//功能：比较两个点是否相等
	//参数：ptA，ptB    待比较的两个二维点
	//      dPrecision  两点比较的精度
	//返回：相等返回 1，不相等返回 0，其他参考定义
	long IsPointEqual(AcGePoint3d ptA,AcGePoint3d ptB,double dPrecision=0.0001);

protected:

	CExchangeInfo*      m_pInfo;
	AcDbObjectIdArray   m_ObjIdArrayHaveUsed;
};

//===============================================================================================================//
//符号表字段定义如下：
// 编号  Sde图层名    类型       查询条件      字段1   字段2   字段3     字段4   字段5   字段6  字段7  字段8 字段9 字段10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3      Fld4    Fld5    Fld6    Fld7  Fld8  Fld9   Fld10
//                                             颜色号  块名称  缩放比例  旋转角度
class CPointExchange : public CBaseExchange
{
private:
	         CPointExchange(void);
public:
	         CPointExchange(CExchangeInfo* pInfo);
	virtual ~CPointExchange(void);

public:

	//功能：在CAD中绘制实体
	//参数：resultId    返回添加实体ID
	//      pRcdSet     查询记录集合
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//功能：将CAD实体保存到SDE库中
	//参数：pRowId,    返回SDE记录得RowId
	//      pCommand   数据库命令对象
	//      ObjId      CAD实体ID
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);


};


//===============================================================================================================//
//符号表字段定义如下：
// 编号  Sde图层名    类型       查询条件      字段1   字段2   字段3  字段4     字段5     字段6  字段7  字段8 字段9 字段10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3   Fld4      Fld5      Fld6    Fld7  Fld8  Fld9   Fld10
//                                             颜色号  内容    字体   文字高度  旋转角度  对齐方式
class CTextExchange : public CBaseExchange
{
private:
	CTextExchange(void);
public:
	         CTextExchange(CExchangeInfo* pInfo);
	virtual ~CTextExchange(void);

public:

	//功能：在CAD中绘制实体
	//参数：resultId    返回添加实体ID
	//      pRcdSet     查询记录集合
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//功能：将CAD实体保存到SDE库中
	//参数：pRowId,    返回SDE记录得RowId
	//      pCommand   数据库命令对象
	//      ObjId      CAD实体ID
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);
};


//===============================================================================================================//
//符号表字段定义如下：
// 编号  Sde图层名    类型       查询条件      字段1   字段2   字段3  字段4     字段5     字段6  字段7  字段8 字段9 字段10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3   Fld4      Fld5      Fld6    Fld7  Fld8  Fld9   Fld10
//                                             颜色号  线型    线宽   全局线宽  是否闭合
class CPolylineExchange : public CBaseExchange
{
private:
	CPolylineExchange(void);
public:
	         CPolylineExchange(CExchangeInfo* pInfo);  
	virtual ~CPolylineExchange(void);

public:

	//功能：在CAD中绘制实体
	//参数：resultId    返回添加实体ID
	//      pRcdSet     查询记录集合
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//功能：将CAD实体保存到SDE库中
	//参数：pRowId,    返回SDE记录得RowId
	//      pCommand   数据库命令对象
	//      ObjId      CAD实体ID
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);

};



//===============================================================================================================//
//符号表字段定义如下：
// 编号  Sde图层名    类型       查询条件      字段1   字段2   字段3  字段4     字段5     字段6    字段7    字段8    字段9  字段10 
// Id    SdeLyName    SdeLyKind  SqlCondition  Fld1    Fld2    Fld3   Fld4      Fld5      Fld6     Fld7     Fld8     Fld9   Fld10
//                                            颜色号  线型     线宽   全局线宽  填充色号  图案名称 图案比例 图案角度
class CPolygonExchange : public CBaseExchange
{
private:
	CPolygonExchange(void);
public:
	         CPolygonExchange(CExchangeInfo* pInfo);
	virtual ~CPolygonExchange(void);

public:

	//功能：在CAD中绘制实体
	//参数：resultId    返回添加实体ID
	//      pRcdSet     查询记录集合
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//功能：将CAD实体保存到SDE库中
	//参数：pRowId,    返回SDE记录得RowId
	//      pCommand   数据库命令对象
	//      ObjId      CAD实体ID
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);

private:
	long GetEntityHatch(AcDbObjectId& resultId,AcDbEntity* pEnt);

};


//===============================================================================================================//
// 符号画表记录字段含义如下：
// 编号    SDE图层名   SDE类型     查询条件       字段1   字段2   字段3     字段4    字段5   字段6  字段7  字段8 字段9  字段10 
// Id      SdeLyName   SdeLyKind   SqlCondition   ×       ×      ×        ×       ×      ×     ×     ×	      
class CMultpatchExchange : public CBaseExchange
{
private:
	CMultpatchExchange(void);
public:
	         CMultpatchExchange(CExchangeInfo* pInfo);
	virtual ~CMultpatchExchange(void);

public:

	//功能：在CAD中绘制实体
	//参数：resultId    返回添加实体ID
	//      pRcdSet     查询记录集合
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DrawEntity(AcDbObjectId& resultId,IDistRecordSet* pRcdSet);

	//功能：将CAD实体保存到SDE库中
	//参数：pRowId,    返回SDE记录得RowId
	//      pCommand   数据库命令对象
	//      ObjId      CAD实体ID
	//返回：成功返回 1，失败返回 0，其他参考定义
	virtual long WriteToSDE(int* pRowId,IDistCommand* pCommand,AcDbObjectId& ObjId);
};




//===============================================================================================================//
struct DIST_SYMBOLFLD
{
public:
	char m_strSqlCondition[2048];   //符号画SQL条件
	char m_strFld[10][60];          //动态字段数组（10个字符串），存放指定字段名或数据值
	int  m_nPosArray[10];           //表示从指定字段中获取的位置数组
	int  m_nNum;                    //数组个数
	bool m_bIsDefault;              //是否为默认Default配置

	DIST_SYMBOLFLD()  //初始化函数
	{
		memset(m_strSqlCondition,0,sizeof(char)*2048);
		for(int i=0; i<10; i++){
			memset(m_strFld[i],0,sizeof(char)*60);
			m_nPosArray[i] = -1;
		}
		m_nNum = 0;
		m_bIsDefault = false;
	}
};

struct DIST_FILLCOLORMAP
{
	CString  m_strDKYDXZ;
	int      m_nClrIndex;
	DIST_FILLCOLORMAP()
	{
		m_strDKYDXZ = "";
		m_nClrIndex = 256;
	}
};

class CSplitStr
{
private:
    //切分的标志符号
    CString m_sSplitFlag;
    //连续的切分的标志符号当成一个标志处理
    BOOL m_bSequenceAsOne;
    //被切分的文本
    CString m_sData;
public:
    //得到切分好的文本串
    void GetSplitStrArray(CStringArray &array);
    //得到被切分的文本
    CString GetData();
    //设置被切分的文本
    void SetData(CString sData);
    //得到切分参数
    BOOL GetSequenceAsOne() {return m_bSequenceAsOne;};
    //设置切分参数
    void SetSequenceAsOne(BOOL bSequenceAsOne) {m_bSequenceAsOne = bSequenceAsOne;};
    //得到切分标志
    CString GetSplitFlag() {return m_sSplitFlag;};
    //设置切分标志
    void SetSplitFlag(CString sSplitFlag) {m_sSplitFlag = sSplitFlag;};
    CSplitStr();
    virtual ~CSplitStr();
};

#endif // __CBASESDEENTITY__H__
