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
// 版权所有 (C) 2007－2010 Dist Inc. 保留所有权利。                          //
///////////////////////////////////////////////////////////////////////////////
#ifndef __BASE_INTERFACE_XML_H__
#define __BASE_INTERFACE_XML_H__





//=============================  接口定义说明 ===============================//
//                                                                           //
// 1. 实现Database,SDE,XML,Shape等数据源操作统一，将公共操作提取到接口类中   //
//    声明，然后在接口基础上，根据不同的数据源，进行不同的类封装实现。注意   //
//    不需要实现的接口方法，定义在实现类中的私有部分。                       //
//                                                                           //
// 2. 为了方便使用，访问数据源有层次，将整个操作流程分四大功能封装：         //
//    Connection 负责数据源的连接                                            //
//    Command    负责数据集和数据表的增，删，改，查等操作                    //
//    RecordSet  负责数据集合的读取，遍历，查找等操作                        //
//                                                                           //
//==============================  接口声明  =================================//
#include <string.h>

#ifdef DIST_DATABASE_XML_EXPORTS
#define DIST_DATABASE_XML_API __declspec(dllexport)
#else
#define DIST_DATABASE_XML_API __declspec(dllimport)
#endif



struct IDistConnectionXML;
struct IDistCommandXML;
struct IDistRecordSetXML;
struct IDistParameterXML;

//============================== 导出函数 ===================================//
//创建连接对象
// 
DIST_DATABASE_XML_API IDistConnectionXML* WINAPI CreateConnectObjcet(void); 

//创建操作执行对象
// 
DIST_DATABASE_XML_API IDistCommandXML*  WINAPI CreateCommandObjcet(void); 



//============================== 自定义类型 =================================//

//日期类型定义
typedef struct{
	int    tm_sec;       // seconds after the minute - [0,59]
	int    tm_min;       // minutes after the hour - [0,59]
	int    tm_hour;      // hours since midnight - [0,23] 
	int    tm_mday;      // day of the month - [1,31]
	int    tm_mon;       // months since January - [0,11] 
	int    tm_year;      // years since 1900 
	int    tm_wday;      // days since Sunday - [0,6] 
	int    tm_yday;      // days since January 1 - [0,365] 
	int    tm_isdst;     // daylight savings time flag
}DIST_DATETIME;


//二进制流格式定义
struct DIST_BLOBINFO{
	long   blob_length;  // 二进制流长度
	char   *blob_buffer; // 二进制流缓冲区 

	DIST_BLOBINFO(void)
	{
		blob_length = 0;
		blob_buffer = NULL;
	}

	DIST_BLOBINFO& operator =  (const DIST_BLOBINFO& blob)
	{
		blob_length = blob.blob_length;
		if(NULL != blob_buffer)
			delete [] blob_buffer;
		blob_buffer = new char[blob_length];
		memcpy(blob_buffer,blob.blob_buffer,blob_length*sizeof(char));

		return *this;
	}

	~DIST_BLOBINFO(void)
	{
		if(NULL != blob_buffer)
			delete [] blob_buffer;
	}
};


//=============================  接口定义  ==================================//

//<<< 1. 连接接口定义
struct IDistConnectionXML
{
	enum PARAMETERTYPE    // 参数类型
	{
		kServer = 0,
		kInstance =1,
		kDbName = 2,
		kUserName = 3
	};

	enum DIST_ENUM_CONNECTRESULT   //连接SDE返回值定义
	{
		kSucess = 0,
		kFailure = -1,
		kSdeNotStarted = -5,
		kTasksExceeded = -7,
		kLoginNotAllowed = -8,
		kInvalidUser = -9,
		kNetFailure = -10,
		kNetTimeOut = -11,
		kNoAccess = -15,
		kInvalidPointer = -65,	
		kInvalidServer = -100,
		kIomgrNotAvailable = -101,  
		kDbmsDoesNotSupport = -1008
	};

	// 功能：创建连接
	// 参数：strConnect  连接字符串
    // 返回：返回操作结果 0表示成功，其他值具体参考接口定义
	virtual LONG Connect(const char* strConnect) = 0;

	// 功能：创建连接
	// 参数：strServer    服务器名称
	//       strInstance  服务实体对象名称
	//       strDatabase  数据库名称
	//       strUserName  用户名称
	//       strPassword  用户密码
	// 返回：返回操作结果 0表示成功，其他值具体参考接口定义中的DIST_ENUM_CONNECTRESULT
	virtual LONG Connect(const char* strServer,      
		                 const char* strInstance,
		                 const char* strDatabase, 
						 const char* strUserName,
					     const char* strPassword) = 0;

	// 功能：断开连接
	// 参数：无
	// 返回：无
	virtual void Disconnect(void)  = 0;

	// 功能：检查连接
	// 参数：IntervalTime检测超时
	// 返回：TRUE 连接成功，FALSE 连接失败
	virtual BOOL CheckConnect(int IntervalTime=60)  = 0;
	
	// 功能：获取参数
	// 参数：param        参数类型
	//       strParameter 参数内容
	// 返回：无
	virtual void GetParameter(PARAMETERTYPE param, char* strParameter) = 0;
    
	// 功能：设置参数
	// 参数：param        参数类型
	//       strParameter 参数内容
	// 返回：无
	virtual void SetParameter(PARAMETERTYPE param, char* strParameter)  = 0;

	// 功能：请求一个对应当command对象
	// 参数：lplpCommand	接收返回到command对象的指针
	// 返回：无
	virtual void QueryCommand(IDistCommandXML** lplpCommand) = 0;

	// 功能：释放对象
	// 参数：无
	// 返回：无
	virtual void Release(void)  = 0;
};



#define DIST_NIL_TYPE_MASK           (1L)
#define DIST_POINT_TYPE_MASK         (1L<<1)
#define DIST_LINE_TYPE_MASK          (1L<<2)
#define DIST_SIMPLE_LINE_TYPE_MASK   (1L<<3)
#define DIST_AREA_TYPE_MASK          (1L<<4)
#define DIST_MULTIPART_TYPE_MASK     (1L<<18)


//<<< 2. 数据库操作类接口定义
struct IDistCommandXML
{
	// 功能：释放对象
	// 参数：无
	// 返回：无
	virtual void Release(void) = 0;


	//=====================记录数据相关操作=======================//


	// 功能：从数据库中查询数据
	// 参数：strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x#)
	//       IRcdSet        根据查询条件返回的记录集
	//       pSpatialFilter 空间搜索条件(主要针对SDE实体空间查询)
	// 返回：TRUE 表示有记录返回，其他情况为FALSE
	virtual LONG SelectData(const char* strSQL,IDistRecordSetXML** IRcdSet,void* pSpatialFilter=NULL) = 0;


	// 功能：向数据库指定表中添加记录
	// 参数：strTableName   操作表名称
	//       IRcdSet        需要添加到表中的记录
	// 返回：TRUE 操作成功，FALSE 操作失败
	//virtual BOOL InsertData(const char* strTableName,char** strFldNames=NULL,...);
	virtual LONG InsertData(const char* strSQL,...) = 0;//添加记录
	

	// 功能：修改数据库表中指定记录
	// 参数：strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x#)
	//       IRcdSet        记录修改的内容
	//       pSpatialFilter 空间搜索条件(主要针对SDE实体空间查询)
	// 返回：TRUE 操作成功，FALSE 操作失败
	//virtual BOOL UpdateData(const char* strSQL,IDistRecordSetXML* IRcdSet,void* pSpatialFilter=NULL);
	virtual LONG UpdateData(const char* strSQL,void* pSpatialFilter=NULL,...) = 0; //修改记录

	// 功能：删除数据库表中的记录
	// 参数：strSQL         不是SQL语句，它包含:表名，WHERE条件信息(格式 T:x,x,x,W:x#)
	// 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG DeleteData(const char* strSQL) = 0; 


	//========================表结构操作==========================//

	// 功能：获取数据表结构信息
	// 参数：strName         表名称
	//       pParam          返回表字段结构描述数组
	//       nparamNums      返回字段结构个数
	// 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG GetTableInfo(const char* strName,IDistParameterXML** pParam,int& nPamramNum) = 0;

	// 功能：创建数据表
	// 参数：strName         创建的表名称
	//       pParam          表字段结构描述数组
	//       nparamNums      字段结构个数
	// 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG CreateTable(const char* strName,IDistParameterXML** pParam,int nPamramNum,const char* strKeyword="DEFAULTS") = 0;


	// 功能：删除指定数据表
	// 参数：strName         表名称
	// 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG DropTable(const char* strName) = 0;

	// 功能：返回图层信息
	// 参数：strTableName       图层关联表名称
	//       strSpacialFldName  图层和表关联的空间字段
	//       pLyInfo            图层信息SE_LAYERINFO指针
    // 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG GetLayerInfo(const char* strTableName,const char* strSpacialFldName,void** pLyInfo) = 0;

	// 功能：创建SDE图层(存放CAD实体对象)
	// 参数：strTableName      SDE关联的属性数据表名称
	//       strSpacialFldName 图层与属性表关联的空间字段名(图层创建后会在属性表中增加该字段)
	//       dScale            图层比例尺
	//       ox,oy             坐标系原点
	//       dGridSize         网格大小
	//       lTypeMask         图层类型
	//       strKeyword        关键字
    // 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG CreateLayer(const char* strTableName,
		                     const char* strSpacialFldName,
							 double dScale,
							 double ox = 0,
							 double oy = 0,
							 double dGridSize = 1000,
							 LONG   lTypeMask = DIST_NIL_TYPE_MASK | DIST_POINT_TYPE_MASK |
							                    DIST_LINE_TYPE_MASK | DIST_SIMPLE_LINE_TYPE_MASK |
							                    DIST_AREA_TYPE_MASK | DIST_MULTIPART_TYPE_MASK,
							 const char* strKeyword="DEFAULT") = 0;


    // 功能：删除SDE图层(存放CAD实体对象)
	// 参数：strTableName      SDE关联的属性数据表名称
	//       strSpacialFldName 图层与属性表关联的空间字段名(图层删除后会在属性表中删除该字段)
    // 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG DropLayer(const char* strTableName,const char* strSpacialFldName) = 0;


	//==========================锁操作============================//

	// 功能：锁定指定图层数据
	// 参数：strLyName        图层名称
	// 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG LockLayerData(const char* strLyName) = 0;

	// 功能：锁定指定数据表
	// 参数：strName          表名称
	// 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG LockTable(const char* strName) = 0;

	// 功能：锁定指定数据表中的一条记录
	// 参数：strName          表名称
	//       nIndexRow        指定行
	// 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG LockTableARow(const char* strName, int nIndexRow) = 0;

	// 功能：锁定指定程序对象
	// 参数：pObjs             对象ID数组
	// 返回：TRUE 操作成功，FALSE 操作失败
	virtual LONG LockApplicationObj(int* pObjs) = 0;



	//=======================光栅图像操作=========================//(待续)

	//创建坐标系和光栅图像的读存
	virtual LONG CreateCoordinate(void) = 0;
	virtual LONG ReadRasterData(void) = 0;
	virtual LONG WriteRasterData(void) = 0;
};


//<<< 3. 数据库记录集类接口定义
struct IDistRecordSetXML
{
	//功能：根据指定的名称，取对应字段的值
	//参数：strFldName 字段名
	//返回：返回字段值(读取需要根据实际类型，强制转换)
	virtual void* operator[] (const char* strFldName) = 0;

	//功能：根据指定的序号，取对应字段的值
	//参数：nIndex 序号
	//返回：返回字段值(读取需要根据实际类型，强制转换)
	virtual void* operator[] (int nIndex) = 0;


	//记录集操作

	//功能：批次读取查询结果
	//参数：无
	//返回：有后续记录返回TRUE，记录读完返回FALSE
	virtual BOOL BatchRead(void) = 0;

	//功能：返回当前查询条件下的记录集的第一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual BOOL MoveFirst(void) = 0;

	//功能：返回当前查询条件下的记录集的最后一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual BOOL MoveLast(void) = 0;

	//功能：返回当前查询条件下的记录集当前记录的下一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual BOOL MoveNext(void) = 0;

	//功能：返回当前查询条件下的记录集当前记录的上一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual BOOL MovePrevious(void) = 0;

	//功能：返回当前查询条件下指定的一条记录
	//参数：nIndex   指定记录号(1 ... n)
	//返回：操作成返回TRUE，失败返回FALSE
	virtual BOOL MoveTo(int nIndex) = 0;

	//功能：根读取记录集的记录数量
	//参数：无
	//返回：返回记录数量
	virtual int  GetRecordCount(void) = 0;

	//功能：根读取记录集的字段数量
	//参数：无
	//返回：返回字段数量
	virtual int  GetFieldNum(void) = 0;

	//功能：释放对象
	//参数：无
	//返回：无
	virtual void Release(void) = 0;
};


//<<< 5. 数据库字段信息描述结构
struct IDistParameterXML{
    char  fld_strName[32];   // the column name 
    long  fld_nType;         // the SDE data type 
    long  fld_nSize;         // the size of the column values 
    short fld_nDecimal;      // number of digits after decimal 
    BOOL  fld_bNullAllow;    // allow NULL values ? 
    short fld_nRowIdType;    // column's use as table's row id 

	IDistParameterXML(void)
	{
		memset(fld_strName,0,sizeof(char)*32);
	}

	IDistParameterXML(const char* strName,long nType,int nSize,short nDecimal,BOOL bNullAllow,short nRowIdType)
	{
		strcpy(fld_strName,strName);
		fld_nType = nType;
		fld_nSize = nSize;
		fld_nDecimal = nDecimal;
		fld_bNullAllow = bNullAllow;
		fld_nRowIdType = nRowIdType;//SE_REGISTRATION_ROW_ID_COLUMN_TYPE_SDE 
	}
	
	IDistParameterXML& operator =  (const IDistParameterXML& pm)
	{
		strcpy(fld_strName,pm.fld_strName);
		fld_nType = pm.fld_nType;
		fld_nSize = pm.fld_nSize;
		fld_nDecimal = pm.fld_nDecimal;
		fld_bNullAllow = pm.fld_bNullAllow;
		fld_nRowIdType = pm.fld_nRowIdType;

		return *this;
	}
};





#endif  //__BASE_INTERFACE_XML_H__