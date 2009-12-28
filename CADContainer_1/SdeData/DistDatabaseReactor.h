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

#ifndef __DISTDBREACTOR__H__
#define __DISTDBREACTOR__H__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//类名称   ：数据库反应器
//设计思路 ：通过DIST_STRUCT_CHANGEROWID结构链表，来记录不同文档的不同图层中，实体编辑情况
//           所以结构定义中包含了strFileName,strCadLyName两成员，已区分不同得编辑位置；该
//           类在继承实现AcDbDatabaseReactor类基础上，增加了操作结构链表的几个函数，来实现
//           对结构中RowId的增，删，改，查等功能
//
//使用说明 ：因为是数据库反应器，所以该类对象在CAD中只需要存在唯一实例对象，且为全局对象，
//           便于不同文档之间的调用
//           注册 ->  CDistDatabaseReactor::rxInit();
//           创建 ->  CDistDatabaseReactor* g_pInstance = CDistDatabaseReactor::GetInstance();
//           释放->   CDistDatabaseReactor::Release();
//修改说明： DIST_STRUCT_CHANGEROWID结构中增加strSdeLyName字段，来适应单CAD图层对应多个SDE图层


#define  PCURDB acdbHostApplicationServices()->workingDatabase()

//存放修改记录，结构链表
struct DIST_STRUCT_CHANGEROWID
{
	int              nID;                //AcDbDatabase 对象的ID(主要区分多文档)
	char             strCadLyName[255];  //CAD图层名
	char             strSdeLyName[255];  //SDE图层名
	CArray<int,int>  RowIdArray;         //RowId数组

	DIST_STRUCT_CHANGEROWID* pNext;

	DIST_STRUCT_CHANGEROWID()//初始化函数
	{
		nID = -1;
		memset(strCadLyName,0,sizeof(char)*255);
		memset(strSdeLyName,0,sizeof(char)*255);
		RowIdArray.RemoveAll();
		pNext = NULL;
	}

};


//只读图层列表结构
struct DIST_STRUCT_READONLYLAYER
{
	int nID;
	char strSdeLyName[255];
	DIST_STRUCT_READONLYLAYER* pNext;

	DIST_STRUCT_READONLYLAYER()
	{
		{
			nID = -1;
			memset(strSdeLyName,0,sizeof(char)*255);
			pNext = NULL;
		}
	}

};

class CDistDatabaseReactor : public AcDbDatabaseReactor 
{
public:
	ACRX_DECLARE_MEMBERS(CDistDatabaseReactor);

    // 功能：析构函数
	// 参数：无
	// 返回：无
    virtual  ~CDistDatabaseReactor();

	// 功能：获取类唯一实例对象指针
	// 参数：无
	// 返回：实例对象指针
	static CDistDatabaseReactor* GetInstance();

	// 功能：释放对象
	// 参数：无
	// 返回：无
	static void Release();

	// 功能：设置参数(SDE图层中RowId名称，扩展属性名称，默认为RowId_Objs)
	// 参数：strRowId      SDE图层中RowId名称
	//       strXDataName  扩展属性名称
	// 返回：无
	void SetParam(const char* strRowIdName,const char* strXDataName);

	// 功能：判断指定属性实体是否存在
	// 参数：strCadLyName     CAD图层名称
	//       strSdeLyName     SDE图层名称
	//       nRowId           实体RowId
	//       pDB              数据库指针(主要区分多文档)
	// 返回：存在返回 1 ；不存在返回 0 ；其他参考定义
	long  IsRowIdExists(const char* strCadLyName,const char *strSdeLyName,int nRowId,AcDbDatabase* pDB=PCURDB);
	
	// 功能：根据指定属性条件查询对应实体的RowID
	// 参数：nRowId        返回实体RowId
	//       strCadLyName  CAD图层名称
	//       strSdeLyName  SDE图层名称
	//       nIndex        在数组中的序号
	//       pDB           数据库指针(主要区分多文档)
	// 返回：存在返回 1 ；不存在返回 0 ；其他参考定义
	long  SelectRowId(int& nRowId,const char* strCadLyName,const char *strSdeLyName,int nIndex,AcDbDatabase* pDB= PCURDB);
	
	// 功能：向链表中插入一个实体信息
	// 参数：strCadLyName     CAD图层名称
	//       strSdeLyName     SDE图层名称
	//       nRowId           实体RowId
	//       pDB              数据库指针(主要区分多文档)
	// 返回：成功返回 1 ；失败返回 0 ；其他参考定义
	void InsertRowId(const char* strCadLyName,const char *strSdeLyName,int nRowId,AcDbDatabase* pDB=PCURDB);
	
	// 功能：删除链表中指定实体信息
	// 参数：strCadLyName     CAD图层名称
	//       strSdeLyName     SDE图层名称
	//       nRowId           实体RowId
	//       pDB              数据库指针(主要区分多文档)
	// 返回：无
	void DeleteRowId(const char* strCadLyName,const char *strSdeLyName,int nRowId,AcDbDatabase* pDB = PCURDB);
	
	// 功能：删除链表中指SDE图层组合CAD图层的所有实体
	// 参数：strCadLyName     CAD图层名称
	//       strSdeLyName     SDE图层名称
	//       pDB              数据库指针(主要区分多文档)
	// 返回：无
	void DeleteLayerRowIdArray(const char* strCadLyName,const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);
	
	// 功能：获取链表中指SDE图层组合CAD图层的实体数量
	// 参数：strCadLyName     CAD图层名称
	//       strSdeLyName     SDE图层名称
	//       pDB              数据库指针(主要区分多文档)
	// 返回：实体数量
	long GetRowIdCount(const char* strCadLyName,const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);

	long IsReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);
	void InsertReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);
	void DeleteReadOnlyLayer(const char *strSdeLyName,AcDbDatabase* pDB = PCURDB);


private:

	// 功能：写信息到指定实体的扩展属性
	// 参数：pObj      指定实体指针
	//       pDB       数据库指针
	// 返回：无
	void WriteInfoToXRecord(const AcDbObject* pObj,const AcDbDatabase* pDB);

private:

    // 禁用函数
							CDistDatabaseReactor();
							CDistDatabaseReactor(const& CDistDatabaseReactor);
    CDistDatabaseReactor&  operator=(const& CDistDatabaseReactor);



public:
	//======================== 实现基类虚函数 ==========================//
    virtual void    objectAppended(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectUnAppended(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectReAppended(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectOpenedForModify(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectModified(const AcDbDatabase* dwg, const AcDbObject* obj);
    virtual void    objectErased(const AcDbDatabase* dwg, const AcDbObject* obj,Adesk::Boolean pErased = Adesk::kTrue);
    virtual void    headerSysVarWillChange(const AcDbDatabase* dwg, const char* name);
    virtual void    headerSysVarChanged(const AcDbDatabase* dwg, const char* name,Adesk::Boolean bSuccess);
    virtual void	proxyResurrectionCompleted(const AcDbDatabase* dwg,const char* appname, AcDbObjectIdArray& objects);
    virtual void	goodbye(const AcDbDatabase* dwg);

private:
	static CDistDatabaseReactor*  m_pInstance;         // 唯一实例
	char                          m_strRowIdName[60];  // RowID在SDE数据库中的字段名称
	char                          m_strXDataName[160]; // 扩展属性XData注册名称
	DIST_STRUCT_CHANGEROWID*      m_pRowIdData;        // RowID数组结构链表
	DIST_STRUCT_READONLYLAYER*    m_pReadOnlyLayer;    // 只读图层结构链表
};


#endif    // __DISTDBREACTOR__H__
