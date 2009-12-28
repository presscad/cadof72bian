#ifndef  __DIST__SDE__DATABASE__H__
#define  __DIST__SDE__DATABASE__H__


#include "sdeerno.h"
#include "sdetype.h"
#include "sderaster.h"

#include "DistBaseInterface.h"
#include "DistIntArray.h"
/////////////////////////////////////////////////////////////////////////////
//说明：考虑到接口定义的通用性，接口定义会有些臃肿，在实现接口时，只需根据
//      自己的应用需求，选择实现对应接口函数；将不需要实现的接口函数放到实
//      现类的私有部分即可。
//修改记录：1.完善了读取字段别名的函数
//          2.增加了GetLayerType，GetLayerCoordref，GetCurUserAllLyName等函数
//          3.扩展了宽字符类型
//          4.增加了权限操作函数(表权限，图层权限)
//==============================  类声明  =================================//
//SDE数据库直接相关类
class CSdeConnection;
class CSdeCommand;
class CSdeRecordSet;

//其他辅助类
class CSdeRaster;



//==============================  类定义  =================================//
//SDE数据库连接类
class CSdeConnection : public IDistConnection
{	
public:
	//构造和析构
             CSdeConnection();
	virtual ~CSdeConnection();

	// 功能：获取数据库连接对象指针
	// 参数：无
	// 返回：返回对象指针
	SE_CONNECTION GetConnectObj(void);

	// 功能：取当前连接的数据库名和用户名
	// 参数：strDBName    数据库名称
	//       strUserName   用户名称
	// 返回：无
	void GetUserDBInfo(char* strDBName,char* strUserName);

public:
	// 功能：创建连接
	// 参数：strServer    服务器名称
	//       strInstance  服务实体对象名称
	//       strDatabase  数据库名称
	//       strUserName  用户名称
	//       strPassword  用户密码
	// 返回：成功返回 1，失败返回 0，其他参考定义中的DIST_ENUM_CONNECTRESULT
	virtual long Connect(const char* strServer,      
		                 const char* strInstance,
		                 const char* strDatabase, 
						 const char* strUserName,
					     const char* strPassword);

	// 功能：断开连接
	// 参数：无
	// 返回：无
	virtual long Disconnect(void);

	// 功能：检查连接
	// 参数：IntervalTime检测超时
	// 返回：TRUE 连接成功，FALSE 连接失败
	virtual long CheckConnect(int IntervalTime=60);
	
	// 功能：获取参数
	// 参数：param        参数类型
	//       strParameter 参数内容
	// 返回：无
	virtual void GetParameter(PARAMETERTYPE param, char* strParameter);
    
	// 功能：设置参数
	// 参数：param        参数类型
	//       strParameter 参数内容
	// 返回：无
	virtual void SetParameter(PARAMETERTYPE param, char* strParameter);

	// 功能：获取SDE数据库连接对象 SE_CONNECTION
	// 参数：无
	// 返回：数据库连接对象
	virtual void* GetConnectObjPointer(void);

	// 功能：释放对象
	// 参数：无
	// 返回：无
	virtual void Release(void);

private:

	//该函数供XML文件连接使用，这里不实现
	virtual long Connect(const char* strConnect);

private:
	//数据库连接对象指针
	SE_CONNECTION m_pConnectObj;  
	char m_strDBName[SE_MAX_DATABASE_LEN];
	char m_strUserName[SE_MAX_OWNER_LEN];
	
};


//SDE数据库操作执行类
class CSdeCommand : public IDistCommand
{
public:
	//构造和析构
	         CSdeCommand(void);
	virtual ~CSdeCommand(void);

public:

	// 功能：设置数据库连接对象
	// 参数：pConnect  数据库连接对象
	// 返回：无
	virtual void SetConnectObj(IDistConnection* pConnect);


	// 功能：释放对象
	// 参数：无
	// 返回：无
	virtual void Release(void);


	//=====================记录数据相关操作=======================//


	// 功能：从数据库中查询数据
	// 参数：strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x#)
	//       IRcdSet        根据查询条件返回的记录集
	//       pSpatialFilter 空间搜索条件(主要针对SDE实体空间查询)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long SelectData(const char* strSQL,IDistRecordSet** IRcdSet,void* pSpatialFilter=NULL);


	// 功能：向数据库指定表中添加记录
	// 参数：strTableName   操作表名称
	//       pRowId         返回添加记录的ID值
	//       ...            数据字段值列表(类型与F:x,x,x描述对应)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long InsertData(int* pRowId,const char* strSQL,...);//添加记录



	// 功能：向数据库指定表中添加记录
	// 参数：strTableName   操作表名称
	//       pRowId         返回添加记录的ID值
	//       strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x#)
	//       strValueArray  值数组
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long InsertDataAsString(int* pRowId,const char* strSQL,char** strValueArray);
	

	// 功能：修改数据库表中指定记录
	// 参数：strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x#)
	//       pSpatialFilter 空间搜索条件(主要针对SDE实体空间查询)
	//       ...            数据字段值列表(类型与F:x,x,x描述对应)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long UpdateData(const char* strSQL,void* pSpatialFilter=NULL,...); //修改记录


	// 功能：修改数据库表中指定记录
	// 参数：strSQL         不是SQL语句，它包含：字段，表，WHERE条件信息(格式 F:x,x,x,T:x,x,x,W:x#)
	//       pSpatialFilter 空间搜索条件(主要针对SDE实体空间查询)
	//       ...            数据字段值列表(类型为字符串)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long UpdateDataAsString(const char *strSQL,char** strValueArray);


	// 功能：删除数据库表中的记录
	// 参数：strSQL         不是SQL语句，它包含:表名，WHERE条件信息(格式 T:x,x,x,W:x#)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DeleteData(const char* strSQL); 

	// 获取服务器名称
	virtual long GetServerTime(struct tm& tm_server);

	// 功能：开始事务操作
	// 参数：无
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long BeginTrans(void);

	// 功能：事务提交
	// 参数：无
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long CommitTrans(void);

	// 功能：事务回滚
	// 参数：无
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long RollbackTrans(void);


	//========================表结构操作==========================//

	// 功能：获取数据表结构信息
	// 参数：pParam          返回表字段结构描述数组
	//       nparamNums      返回字段结构个数
	//       strName         表名称
	//       strOwnerGDB     GDB的拥有者
	// 返回：成功返回 1，失败返回 0，其他参考定义
	// 注意：该函数参数pParam的空间分配是在函数内完成，所有pParam的空间由CSdeCommand类
	//       自己来释放，在外部只是在CSdeCommand对象作用域范围内，使用了它的指针，如果
	//       需要全局来使用，需要将内容复制出来
	virtual long GetTableInfo(IDistParameter** pParam,int* nPamramNum,const char* strName,const char* strOwnerGDB=NULL);

	// 功能：创建数据表
	// 参数：strName         创建的表名称
	//       pParam          表字段结构描述数组
	//       nparamNums      字段结构个数
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long CreateTable(const char* strName,IDistParameter* pParam,int nPamramNum,const char* strKeyword="DEFAULTS");


	// 功能：删除指定数据表
	// 参数：strName         表名称
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DropTable(const char* strName);




	// 功能：根据坐标点信息创建SHAPE
	// 参数：pShape      返回创建SHAPE的指针(即&Shpae)
	//       strLyName   SDE图层名称
	//       ptArray     坐标点数组（1个创建点，多个且首尾点不等创建线，闭合创建面)
	//       nPtCount    坐标点个数
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long CreateShapeByCoord(int* pShape,const char* strLyName,IDistPoint* ptArray,int nPtCount);


	// 功能：复制图层或表指定条件的记录(只复制名称相同的字段)
	// 参数：strSrcLyName    源图层名称
	//       strTargeLyName  目标图层名称
	//       strWhere        指定条件(如：OBJECTID>100)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long CopyRecordByCondition(const char* strTargeLyName,const char* strSrcLyName,const char* strWhere, 
		bool bAddVersion = false, const char* strUserName = NULL, const char* strCurTime = NULL);

	// 跨图层拷贝对象
	virtual long CopyRecordsAcrossLayer(LPCTSTR lpDestLayer, LPCTSTR lpSrcLayer, CUIntArray& rowIds, LPCTSTR lpUserName, int nEditRowId);

	// 功能：根据SHAPE结构指针，获取其点数组
	// 参数：pShapeType  返回SHAPE类型
	//       ptArray     返回坐标点数组
	//       nPtCount    返回坐标点个数
	//       pShape      创建SHAPE的指针(注意是SE_SHAPE*)
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetShapePointArray(int* pShapeType,IDistPoint** ptArray,int* pPtCount,int* pShape);


	//=========================图层操作===========================//

	// 功能：返回当前数据库中所有图层的名称
	// 参数：strLyNameArray   返回图层名数组
	//       pCount           返回数组数量
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetCurUserAllLyName(char*** strLyNameArray,long* pCount);

	// 功能：返回图层信息
	// 参数：strTableName       图层关联表名称
	//       strSpacialFldName  图层和表关联的空间字段
	//       pLyInfo            图层信息SE_LAYERINFO指针
    // 返回：TRUE 操作成功，FALSE 操作失败
	virtual long GetLayerInfo(const char* strTableName,const char* strSpacialFldName,void** pLyInfo);


	// 功能：返回图层信息
	// 参数：strLyName        图层名称
	//       strShapeFldName  图层和表关联的空间字段
	//       pType            返回图层所属类型
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetLayerType(const char* strLyName,const char* strShapeFldName,long* pType);


	// 功能：返回图层坐标系信息
	// 参数：strLyName        图层关联表名称
	//       strShapeFldName  图层和表关联的空间字段
	//       pCoordref        图层坐标系信息指针
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GetLayerCoordref(const char* strLyName,const char* strShapeFldName,void** pCoordref);
	
	// 功能：创建SDE图层(存放CAD实体对象)
	// 参数：strTableName      SDE关联的属性数据表名称
	//       strSpacialFldName 图层与属性表关联的空间字段名(图层创建后会在属性表中增加该字段)
	//       dScale            图层比例尺
	//       ox,oy             坐标系原点
	//       dGridSize         网格大小
	//       lTypeMask         图层类型
	//       strKeyword        关键字
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long CreateLayer(const char* strTbleName,
		                     const char* strSpacialFldName,
							 double dScale,
							 double ox = 0,
							 double oy = 0,
							 double dGridSize = 1000,
							 LONG   lTypeMask = kShapeNil | kShapePoint |
							                    kShapeLine | kShapeSimpleLine|
							                    kShapeArea | kShapeMultiPart,
							 const char* strKeyword="DEFAULT");


    // 功能：删除SDE图层(存放CAD实体对象)
	// 参数：strTableName      SDE关联的属性数据表名称
	//       strSpacialFldName 图层与属性表关联的空间字段名(图层删除后会在属性表中删除该字段)
    // 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long DropLayer(const char* strTableName,const char* strSpacialFldName);




	//======================== 权限操作 ==========================//

	// 功能：为用户分配图层访问权限
	// 参数：strUser                  授权用户名称
	//       strLayerName             图层名称
	//       strFldNameArray          授权字段列表
	//       nFldCount                字段数量
	//       nPrivilege               访问权限（参考DIST_ENUM_ACCESS_PRIV定义,多个权限组合用 '|'）
	//       bAllowPrivilegesToOther  是否允许将该权限授权给其他用户    
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GrantUserAccessLayerPrivilege(const char *strUser,const char *strLayerName,
		                                       char** strFldNameArray,int nFldCount,
		                                       long nPrivilege=kSelectPrivilege,
											   BOOL bAllowPrivilegesToOther=FALSE);

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
											                   kInsertPrivilege | kDeletePrivilege);

	// 功能：为用户分配工作表访问权限
	// 参数：strUser                  授权用户名称
	//       strTableName             工作表名称
	//       nPrivilege               访问权限（参考DIST_ENUM_ACCESS_PRIV定义,多个权限组合用 '|'）
	//       bAllowPrivilegesToOther  是否允许将该权限授权给其他用户    
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long GrantUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                       long nPrivilege=kSelectPrivilege,
		                                       BOOL bAllowPrivilegesToOther=FALSE);


	// 功能：删除用户工作表指定访问权限
	// 参数：strUser                  授权用户名称
	//       strTableName             工作表名称
	//       nPrivilege               访问权限(参考DIST_ENUM_ACCESS_PRIV定义,多个权限组合用 '|')
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long RemoveUserAccessTablePrivilege(const char *strUser,const char* strTableName,
		                                        long nPrivilege=kSelectPrivilege | kUpdatePrivilege | 
											                   kInsertPrivilege | kDeletePrivilege);




	//==========================锁操作============================//

	// 功能：锁定指定图层数据
	// 参数：strLyName        图层名称
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long LockLayerData(const char* strLyName);

	// 功能：锁定指定数据表
	// 参数：strName          表名称
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long LockTable(const char* strName);

	// 功能：锁定指定数据表中的一条记录
	// 参数：strName          表名称
	//       nIndexRow        指定行
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long LockTableARow(const char* strName, int nIndexRow);

	// 功能：锁定指定程序对象
	// 参数：pObjs             对象ID数组
	// 返回：成功返回 1，失败返回 0，其他参考定义
	virtual long LockApplicationObj(int* pObjs);


	



	//=======================光栅图像操作=========================//(待续)

	//创建坐标系和光栅图像的读存
	virtual long CreateCoordinate(void);
	virtual long ReadRasterData(void);
	virtual long WriteRasterData(void);


private:
	//辅助函数(实现时添加)


	//功能：解析自定义SQL语句，提取字段，表，where条件信息
	//参数：strSQL         待解析的字符串
	//      strTableName   表名称数组
	//      nTableNums     表个数
	//      strFieldNames  字段名数组
	//      nFieldNums     字段数量
	//      strWhere       WHERE条件
	//返回：成功返回TRUE，失败返回FALSE
	long ReduceSQL(const char* strSQL, 
		           char*** strTableNames,int& nTableNum, 
				   char*** strFieldNames,int& nFieldNum,
				   char** strWhere);

	long ReduceSQL( const char* strSQL, 
					char*** strTableNames,int& nTableNum, 
					char*** strFieldNames,int& nFieldNum,
					char** strWhere, char** strOrderby);

	//功能：根据查询条件，将表的RowId字段查询结果写到临时文件中
	//参数：strTableName   表名称
	//      strFldName     RowID字段名称
	//      strWhere       Where条件
	//      pSpatialFilter 空间过滤条件
	//返回：无
	void WriteRowIdToArray(const char* strTableName,const char* strFldName,
		                  const char* strWhere,void* pSpatialFilter,CDistIntArray* pData);

private:
	SE_CONNECTION m_pConnectObj;  //数据库连接对象
	IDistParameter *m_pParams;
	int             m_nParamCount;

	char            m_strCurTableName[SE_MAX_TABLE_LEN ]; //当前操作表名称
	int             m_nFldTypeArray[SE_MAX_COLUMNS]; //字段类型数组

	char**          m_strAllLayerNameArray;
	int             m_nLayerNameCount;

	char            m_strDBName[SE_MAX_DATABASE_LEN];  //登陆数据库名
	char            m_strUserName[SE_MAX_OWNER_LEN];   //登陆用户名

	SE_SHAPE        m_tempShape;    //供外部创SHAPE
	IDistPoint*     m_pTempPtArray; //供外部使用的点数组
};


//SDE数据库记录集合类
class CSdeRecordSet : public IDistRecordSet
{
public:
	//构造和析构
	         CSdeRecordSet();
	virtual ~CSdeRecordSet();


	//功能：设置结果流，查询信息，空间过滤信息(供CSdeCommand使用)
	//参数：pStream    结果流
	//      sqlInfo    查询信息结构
	//      pFilter    空间过滤结构
	//      pData      RowId数组
	//返回：无
	void SetInfo(SE_STREAM* pStream,SE_QUERYINFO* sqlInfo,SE_FILTER* pFilter,CDistIntArray* pData);

public:
    
	//功能：根据指定的名称，取对应字段的值
	//参数：strFldName 字段名
	//返回：返回字段值(读取需要根据实际类型，强制转换)
	virtual void* operator[] (const char* strFldName);

	//功能：根据指定的序号，取对应字段的值
	//参数：nIndex 序号
	//返回：返回字段值(读取需要根据实际类型，强制转换)
	virtual void* operator[] (int nIndex);

	//功能：根据指定的序号，取对应字段的值,结果值为字符串
	//参数：strValue 返回结果字符串 
	//      nIndex   序号
	//返回：1 表示成功，0 表示失败，其他参考定义
	virtual long GetValueAsString(char *strValue,int nIndex);

	virtual long GetValueAsString(long& nType, char *strValue,int nIndex);


	//功能：根据指定的序号，取对应字段的值,结果值为字符串
	//参数：strValue   返回结果字符串 
	//      strFldName 字段名
	//返回：1 表示成功，0 表示失败，其他参考定义
	virtual long GetValueAsString(char *strValue,const char* strFldName);
	
	virtual long GetValueAsString(long& nType, char *strValue,const char* strFldName);

	//记录集操作

	//功能：批次读取查询结果
	//参数：无
	//返回：有后续记录返回TRUE，记录读完返回FALSE
	virtual long BatchRead(void);

	//功能：返回当前查询条件下的记录集的第一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MoveFirst(void);

	//功能：返回当前查询条件下的记录集的最后一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MoveLast(void);

	//功能：返回当前查询条件下的记录集当前记录的下一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MoveNext(void);

	//功能：返回当前查询条件下的记录集当前记录的上一条记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MovePrevious(void);

	//功能：返回当前查询条件下指定的一条记录
	//参数：nIndex   指定记录号(1 ... n)
	//返回：操作成返回TRUE，失败返回FALSE
	virtual long MoveTo(int nIndex);

	//功能：根读取记录集的记录数量
	//参数：无
	//返回：返回记录数量
	virtual int  GetRecordCount(void);

	//功能：根读取记录集的字段数量
	//参数：无
	//返回：返回字段数量
	virtual int  GetFieldNum(void);

	//功能：释放对象
	//参数：无
	//返回：无
	virtual void Release(void);


private:

	//功能：根据游标位置读取对应的记录
	//参数：无
	//返回：操作成返回TRUE，失败返回FALSE
	long ReadARow(void);

private:
	SE_CONNECTION    m_pConnectObj;       //数据库连接对象
	SE_STREAM        m_pStream;           //查询结果流
	SE_COORDREF      m_coordref;          //坐标系描述(主要针对SHAPE字段)


	SE_COLUMN_DEF   *m_pParams;           //字段描述数组
	int              m_nParamNum;         //字段个数

	SE_QUERYINFO     m_SqlInfo;           //当前返回流的查询条件描叙
	SE_FILTER*       m_pFilter;           //空间过滤条件
    long             m_nTableNum;         //表数量
	
	char            *m_strOldWhere; //旧的Where条件
	int              m_nRecordCount;      //记录数量     

	CDistIntArray   *m_pRowIdData;       //记录集合RowId数组
	char             m_strRowIdName[32];

	int              m_nPointer ;         //游标位置
	BOOL             m_bBatch;            //是否连续批次读数据


	//为返回值提供临时存储位置
	short            m_nValue16;
	int              m_nValue32;
	float            m_fValue;
	double           m_dValue;
	char             m_strValue[SE_MAX_MESSAGE_LENGTH];
	SE_WCHAR         m_strValueW[SE_MAX_MESSAGE_LENGTH];//宽字符
	SE_BLOB_INFO     m_pBlob; // 暂时不作处理
	DIST_DATETIME_X    m_tmValue;
	SE_SHAPE         m_shape;
};

#endif //__DIST__SDE__DATABASE__H__
