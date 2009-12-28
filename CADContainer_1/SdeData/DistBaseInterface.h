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
#ifndef __BASE_INTERFACE__H__
#define __BASE_INTERFACE__H__





//=============================  接口定义说明 ===============================//
//                                                                           //
// 1. 实现Database,SDE,XML,Shape等数据源操作统一，将公共操作提取到接口类中   //
//    声明，然后在接口基础上，根据不同的数据源，进行不同的类封装实现。注意   //
//    不需要实现的接口方法，定义在实现类中的私有部分。                       //
//                                                                           //
// 2. 为了方便使用，访问数据源有层次，将整个操作流程分三大功能封装：         //
//    Connection 负责数据源的连接                                            //
//    Command    负责数据集和数据表的增，删，改，查等操作                    //
//    RecordSet  负责数据集合的读取，遍历，查找等操作                        //
//                                                                           //
//==============================  接口声明  =================================//
#include <string.h>

#ifdef DIST_DATABASE_EXPORTS
#define DIST_DATABASE_API __declspec(dllexport)
#else
#define DIST_DATABASE_API __declspec(dllimport)
#endif



//接口声明
struct IDistConnection;
struct IDistCommand;
struct IDistRecordSet;
struct IDistParameter;
struct IDistPoint;




//============================== 导出函数 ===================================//

//根据错误编号返回错误提示
DIST_DATABASE_API char* WINAPI ReturnErrorCodeInfo(long nErrorCode);

//创建连接对象
DIST_DATABASE_API IDistConnection* WINAPI CreateSDEConnectObjcet(void); 

//创建操作执行对象
// 
DIST_DATABASE_API IDistCommand*  WINAPI CreateSDECommandObjcet(void); 


//============================== 自定义类型 =================================//

// 读取连接数据库参数类型
enum PARAMETERTYPE    
{
	kServer = 0,
	kInstance =1,
	kDbName = 2,
	kUserName = 3
};

//连接SDE返回值定义
enum DIST_ENUM_CONNECTRESULT   
{
	kSucess = 1,    //修改(0 -> 1   为了与一般调用返回匹配)
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

//RowId类型(唯一字段)
enum DIST_ENUM_ROWIDTYPE   
{
	kRowIdColumnSde  = 1, 
	kRowIdColumnUser = 2,
	kRowIdColumnNone = 3
};

//SDE表字段类型
enum DIST_ENUM_FIELDTYPE   
{
	kInt16              = 1, //2-byte Integer
	kInt32              = 2, //4-byte Integer  SE_INTEGER_TYPE
	kFloat32            = 3, //4-byte Float
	kFloat64            = 4, //8-byte Float
	kString             = 5, //Null Term. Character Array
	kBLOB               = 6, //Variable Length Data
	kDate               = 7, //Struct tm Date
	kShape              = 8, //Shape geometry (SE_SHAPE)
	kRaster             = 9, //Raster  
	kXml                =10, //XML Document
	kInt64              =11, //8-byte Integer
	kUUID               =12, //A Universal Unique ID
	kCLOB               =13, //Character variable length data
	kNString            =14, //UNICODE Null Term. Character Array
	kNCLOB              =15 //UNICODE Character Large Object
};

enum DIST_ENUM_ACCESS_PRIV
{
	kSelectPrivilege = (1<<1),
	kUpdatePrivilege = (1<<2),
	kInsertPrivilege = (1<<3),
	kDeletePrivilege = (1<<4)
};

//=============================  接口定义  ==================================//

//<<< 1. 连接接口定义
struct IDistConnection
{
	// 功能：创建连接
	// 参数：strConnect  连接字符串
    // 返回：返回操作结果 0表示成功，其他值具体参考接口定义
	virtual long Connect(const char* strConnect) = 0;

	// 功能：创建连接
	// 参数：strServer    服务器名称
	//       strInstance  服务实体对象名称
	//       strDatabase  数据库名称
	//       strUserName  用户名称
	//       strPassword  用户密码
	//返回：成功返回 1，失败返回 0，其他值具体参考接口定义中的DIST_ENUM_CONNECTRESULT
	virtual long Connect(const char* strServer,      
		                 const char* strInstance,
		                 const char* strDatabase, 
						 const char* strUserName,
					     const char* strPassword) = 0;

	// 功能：断开连接
	// 参数：无
	// 返回：无
	virtual long Disconnect(void)  = 0;

	// 功能：检查连接
	// 参数：IntervalTime检测超时
	// 返回：TRUE 连接成功，FALSE 连接失败
	virtual long CheckConnect(int IntervalTime=60)  = 0;
	
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

	// 功能：获取SDE数据库连接对象 SE_CONNECTION
	// 参数：无
	// 返回：数据库连接对象
	virtual void* GetConnectObjPointer(void) = 0;

	// 功能：释放对象
	// 参数：无
	// 返回：无
	virtual void Release(void)  = 0;
};

//<<< 2. 数据库操作类接口定义
struct IDistCommand
{
	// 功能：设置数据库连接对象
	// 参数：pConnect  数据库连接对象
	// 返回：无
	virtual void SetConnectObj(IDistConnection* pConnect) = 0;


	// 功能：释放对象
	// 参数：无
	// 返回：无
	virtual void Release(void) = 0;


	//=====================记录数据相关操作=======================//


	// 功能：从数据库中查询数据
	// 参数：strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x,##)
	//       IRcdSet        根据查询条件返回的记录集
	//       pSpatialFilter 空间搜索条件(主要针对SDE实体空间查询)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long SelectData(const char* strSQL,IDistRecordSet** IRcdSet,void* pSpatialFilter=NULL) = 0;


	// 功能：向数据库指定表中添加记录
	// 参数：strTableName   操作表名称
	//       pRowId         返回添加记录的ID值
	//       strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x,##)
	//       ...            数据字段值列表(类型与F:x,x,x描述对应)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long InsertData(int* pRowId,const char* strSQL,...) = 0;//添加记录


	// 功能：向数据库指定表中添加记录
	// 参数：strTableName   操作表名称
	//       pRowId         返回添加记录的ID值
	//       strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x,##)
	//       strValueArray  值数组
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long InsertDataAsString(int* pRowId,const char* strSQL,char** strValueArray)=0;
	

	// 功能：修改数据库表中指定记录
	// 参数：strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x,##)
	//       pSpatialFilter 空间搜索条件(主要针对SDE实体空间查询)
	//       ...            数据字段值列表(类型与F:x,x,x描述对应)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long UpdateData(const char* strSQL,void* pSpatialFilter=NULL,...) = 0; //修改记录


	// 功能：修改数据库表中指定记录
	// 参数：strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x,##)
	//       pSpatialFilter 空间搜索条件(主要针对SDE实体空间查询)
	//       ...            数据字段值列表(类型为字符串)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long UpdateDataAsString(const char *strSQL,char** strValueArray)= 0;


	// 功能：删除数据库表中的记录
	// 参数：strSQL         不是SQL语句，它包含:表名，WHERE条件信息(格式 T:x,x,x,W:x,##)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DeleteData(const char* strSQL) = 0; 



	// 功能：开始事务操作
	// 参数：无
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long BeginTrans(void) = 0;

	// 功能：事务提交
	// 参数：无
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long CommitTrans(void) = 0;

	// 功能：事务回滚
	// 参数：无
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long RollbackTrans(void) = 0;


	//========================表结构操作==========================//

	// 功能：获取数据表结构信息
	// 参数：pParam          返回表字段结构描述数组
	//       nparamNums      返回字段结构个数
	//       strName         表名称
	//       strOwnerGDB     GDB的拥有者
	// 返回：成功返回 1，失败返回 0，其他参考定义
	// 注意：该函数参数pParam的空间分配是在函数内完成，所有pParam的空间由CSdeCommand类
	//       自己来释放，在外部只是在IDistCommand对象作用域范围内，使用了它的指针；如果
	//       需要全局来使用，需要将内容复制出来，否则会引发违规访问
	virtual long GetTableInfo(IDistParameter** pParam,int* nPamramNum,const char* strName,const char* strOwnerGDB=NULL) = 0;

	// 功能：创建数据表
	// 参数：strName         创建的表名称
	//       pParam          表字段结构描述数组
	//       nparamNums      字段结构个数
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long CreateTable(const char* strName,IDistParameter* pParam,int nPamramNum,const char* strKeyword="DEFAULTS") = 0;


	// 功能：删除指定数据表
	// 参数：strName         表名称
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DropTable(const char* strName) = 0;


	// 功能：根据SHAPE结构指针，获取其点数组
	// 参数：pShapeType  返回SHAPE类型
	//       ptArray     返回坐标点数组
	//       nPtCount    返回坐标点个数
	//       pShape      创建SHAPE的指针(注意是SE_SHAPE*)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetShapePointArray(int* pShapeType,IDistPoint** ptArray,int* pPtCount,int* pShape)=0;


	// 功能：返回当前登陆用户能访问所有图层的名称
	// 参数：strLyNameArray   返回图层名数组
	//       pCount           返回数组数量
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetCurUserAllLyName(char*** strLyNameArray,long* pCount)=0;

	// 功能：返回图层信息
	// 参数：strTableName       图层关联表名称
	//       strSpacialFldName  图层和表关联的空间字段
	//       pLyInfo            图层信息SE_LAYERINFO指针
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetLayerInfo(const char* strTableName,const char* strSpacialFldName,void** pLyInfo) = 0;


	// 功能：返回图层信息
	// 参数：strLyName        图层名称
	//       strShapeFldName  图层和表关联的空间字段
	//       pType            返回图层所属类型
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetLayerType(const char* strLyName,const char* strShapeFldName,long* pType)=0;


	// 功能：返回图层坐标系信息
	// 参数：strLyName        图层关联表名称
	//       strShapeFldName  图层和表关联的空间字段
	//       pCoordref        图层坐标系信息指针
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetLayerCoordref(const char* strLyName,const char* strShapeFldName,void** pCoordref)=0;


	// 功能：创建SDE图层(存放CAD实体对象)
	// 参数：strTableName      SDE关联的属性数据表名称
	//       strSpacialFldName 图层与属性表关联的空间字段名(图层创建后会在属性表中增加该字段)
	//       dScale            图层比例尺
	//       ox,oy             坐标系原点
	//       dGridSize         网格大小
	//       lTypeMask         图层类型
	//       strKeyword        关键字
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long CreateLayer(const char* strTableName,
		                     const char* strSpacialFldName,
							 double dScale,
							 double ox = 0,
							 double oy = 0,
							 double dGridSize = 1000,
							 long   lTypeMask = kShapeNil | kShapePoint |
							                    kShapeLine | kShapeSimpleLine|
							                    kShapeArea | kShapeMultiPart,
							 const char* strKeyword="DEFAULT") = 0;


    // 功能：删除SDE图层(存放CAD实体对象)
	// 参数：strTableName      SDE关联的属性数据表名称
	//       strSpacialFldName 图层与属性表关联的空间字段名(图层删除后会在属性表中删除该字段)
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DropLayer(const char* strTableName,const char* strSpacialFldName) = 0;


	//========================== 访问权限 ============================//

	// 功能：为用户分配图层访问权限
	// 参数：strUser                  授权用户名称
	//       strLayerName             图层名称
	//       strFldNameArray          授权字段列表(值为NUL时表示图层中所有字段)
	//       nFldCount                字段数量
	//       nPrivilege               访问权限（参考DIST_ENUM_ACCESS_PRIV定义,多个权限组合用 '|'）
	//       bAllowPrivilegesToOther  是否允许将该权限授权给其他用户    
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GrantUserAccessLayerPrivilege(const char *strUser,const char *strLayerName,
		                                       char** strFldNameArray,int nFldCount,
		                                       long nPrivilege=kSelectPrivilege,
											   BOOL bAllowPrivilegesToOther=FALSE)=0;

	// 功能：删除用户分配图层指定字段的访问权限
	// 参数：strUser                  授权用户名称
	//       strLayerName             图层名称
	//       strFldNameArray          授权字段列表(值为NUL时表示图层中所有字段)
	//       nFldCount                字段数量
	//       nPrivilege               访问权限（参考DIST_ENUM_ACCESS_PRIV定义,多个权限组合用 '|'） 
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long RemoveUserAccessLayerPrivilege(const char *strUser,const char *strLayerName,
		                                       char** strFldNameArray,int nFldCount,
		                                       long nPrivilege=kSelectPrivilege | kUpdatePrivilege | 
											                   kInsertPrivilege | kDeletePrivilege)=0;

	// 功能：为用户分配工作表访问权限
	// 参数：strUser                  授权用户名称
	//       strTableName             工作表名称
	//       nPrivilege               访问权限（参考DIST_ENUM_ACCESS_PRIV定义,多个权限组合用 '|'）
	//       bAllowPrivilegesToOther  是否允许将该权限授权给其他用户    
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GrantUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                       long nPrivilege=kSelectPrivilege,
		                                       BOOL bAllowPrivilegesToOther=FALSE)=0;

	// 功能：删除用户工作表指定访问权限
	// 参数：strUser                  授权用户名称
	//       strTableName             工作表名称
	//       nPrivilege               访问权限(参考DIST_ENUM_ACCESS_PRIV定义,多个权限组合用 '|')
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long RemoveUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                       long nPrivilege=kSelectPrivilege | kUpdatePrivilege | 
											                   kInsertPrivilege | kDeletePrivilege)=0;



	//==========================锁操作============================//

	// 功能：锁定指定图层数据
	// 参数：strLyName        图层名称
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long LockLayerData(const char* strLyName) = 0;

	// 功能：锁定指定数据表
	// 参数：strName          表名称
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long LockTable(const char* strName) = 0;

	// 功能：锁定指定数据表中的一条记录
	// 参数：strName          表名称
	//       nIndexRow        指定行
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long LockTableARow(const char* strName, int nIndexRow) = 0;

	// 功能：锁定指定程序对象
	// 参数：pObjs             对象ID数组
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long LockApplicationObj(int* pObjs) = 0;



	//=======================光栅图像操作=========================//(待续)

	//创建坐标系和光栅图像的读存
	virtual long CreateCoordinate(void) = 0;
	virtual long ReadRasterData(void) = 0;
	virtual long WriteRasterData(void) = 0;
};


//<<< 3. 数据库记录集类接口定义
struct IDistRecordSet
{
	//功能：根据指定的名称，取对应字段的值
	//参数：strFldName 字段名
	//返回：返回字段值(读取需要根据实际类型，强制转换)
	virtual void* operator[] (const char* strFldName) = 0;

	//功能：根据指定的序号，取对应字段的值
	//参数：nIndex 序号
	//返回：返回字段值(读取需要根据实际类型，强制转换)
	virtual void* operator[] (int nIndex) = 0;

	//功能：根据指定的序号，取对应字段的值,结果值为字符串
	//参数：strValue 返回结果字符串 
	//      nIndex   序号
	//返回：1 表示成功，0 表示失败，其他参考定义
	virtual long GetValueAsString(char *strValue,int nIndex)=0;

	//功能：根据指定的序号，取对应字段的值,结果值为字符串
	//参数：strValue   返回结果字符串 
	//      strFldName 字段名
	//返回：1 表示成功，0 表示失败，其他参考定义
	virtual long GetValueAsString(char *strValue,const char* strFldName)=0;


	//记录集操作

	//功能：批次读取查询结果
	//参数：无
	//返回：有后续记录返回TRUE，记录读完返回FALSE
	virtual long BatchRead(void) = 0;

	//功能：返回当前查询条件下的记录集的第一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MoveFirst(void) = 0;

	//功能：返回当前查询条件下的记录集的最后一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MoveLast(void) = 0;

	//功能：返回当前查询条件下的记录集当前记录的下一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MoveNext(void) = 0;

	//功能：返回当前查询条件下的记录集当前记录的上一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MovePrevious(void) = 0;

	//功能：返回当前查询条件下指定的一条记录
	//参数：nIndex   指定记录号(1 ... n)
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MoveTo(int nIndex) = 0;

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


//<<< 4. 数据库字段信息描述结构
struct IDistParameter
{
    char  fld_strName[32];   
    long  fld_nType;         // DIST_ENUM_FIELDTYPE
    long  fld_nSize;          
    short fld_nDecimal;       
    BOOL  fld_bNullAllow;    
    short fld_nRowIdType;    // DIST_ENUM_ROWIDTYPE
	char  fld_strAliasName[32]; //别名字段

	IDistParameter(void)
	{
		memset(fld_strName,0,sizeof(char)*32);
		memset(fld_strAliasName,0,sizeof(char)*32);
	}

	IDistParameter(const char* strName,const char* strAliasName,long nType,int nSize,short nDecimal,BOOL bNullAllow,short nRowIdType)
	{
		strcpy(fld_strName,strName);
		strcpy(fld_strAliasName,strAliasName);
		fld_nType = nType;
		fld_nSize = nSize;
		fld_nDecimal = nDecimal;
		fld_bNullAllow = bNullAllow;
		fld_nRowIdType = nRowIdType;//SE_REGISTRATION_ROW_ID_COLUMN_TYPE_SDE 
	}
	
	IDistParameter& operator =  (const IDistParameter& pm)
	{
		strcpy(fld_strName,pm.fld_strName);
		strcpy(fld_strAliasName,pm.fld_strAliasName);

		fld_nType = pm.fld_nType;
		fld_nSize = pm.fld_nSize;
		fld_nDecimal = pm.fld_nDecimal;
		fld_bNullAllow = pm.fld_bNullAllow;
		fld_nRowIdType = pm.fld_nRowIdType;

		return *this;
	}
};

//日期类型定义
struct DIST_DATETIME_X{
	int    tm_sec;       // seconds after the minute - [0,59]
	int    tm_min;       // minutes after the hour - [0,59]
	int    tm_hour;      // hours since midnight - [0,23] 
	int    tm_mday;      // day of the month - [1,31]
	int    tm_mon;       // months since January - [0,11] 
	int    tm_year;      // years since 1900 
	int    tm_wday;      // days since Sunday - [0,6] 
	int    tm_yday;      // days since January 1 - [0,365] 
	int    tm_isdst;     // daylight savings time flag
};

struct IDistPoint
{
	double x;
	double y;
};

/*


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
};*/

#endif  //__BASE_INTERFACE__H__
