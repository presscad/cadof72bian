#ifndef  __ACCESS__WEBSERVICE__INFO__H__
#define  __ACCESS__WEBSERVICE__INFO__H__ 
#include "StdAfx.h"
#include "DistBasePara.h"
class CPowerConfig;
class CPoltIndex;
class CYdxz3Part;
class CBaseCfgInfo;


//===============================================================================================//
#define POWER_HIDE          0x0000      // 不可以访问权限
#define	POWER_SCAN			0x0001		// SDE的浏览和查看权限
#define POWER_EDIT			0x0002		// SDE的修改和入库权限

typedef struct
{
	CString	strTreeID;			// 树节点ID
	CString	strSDEPower;		// 权限
}POWERCONFIG;
typedef CArray<POWERCONFIG,POWERCONFIG&> POWERCONFIGARRAY;

// 权限控制类说明：
// 从业务数据库的TBRIGHT_FORM表中读取权限控制配置信息
// 字段PW_ROLE说明用户角色编号
// 字段PW_RESOURCEID说明资源信息或者编号
// 字段PW_POWER说明权限，采用16进制按位操作
// 字段PW_CATEGORY为CAD为本模块使用
class CPowerConfig
{
public:
	         CPowerConfig(void);
	virtual ~CPowerConfig(void);

public:
	static POWERCONFIGARRAY m_PowerArray;

public:
	//临时
	//static BOOL GetCurUserRight(CString strURL,CString strRoleId, CString strXMBH);
	static void Release();
};



//===============================================================================================//

typedef struct
{
	CString	Id;			// 编号
	CString	SDELayer;	// SDE表名称
	CString	AliasName;	// 中文别名(显示名称)
	CString	FieldName;	// 索引字段名称
	CString	OrderNum;	// 次序(默认为编号)
	CString Type;		// 记录类别(1表示项目编号)
}POLTINDEXCONFIG;
typedef CArray<POLTINDEXCONFIG,POLTINDEXCONFIG&> POLTINDEXARRAY;

class CPoltIndex
{
public:
	         CPoltIndex(void);
	virtual ~CPoltIndex(void);

private:

	static POLTINDEXARRAY m_PoltIndArray;

public:
	static void Realse();

	static BOOL GetPoltIndInf(const CString strURL);
	static BOOL GetLayerPoltIndexArray(CStringArray& strSdeLayerNameArray, CStringArray& strFieldNameArray);
	static BOOL GetXMBHInfo(CString& strSdeLayerName, CString& strFieldName);
};


//===============================================================================================//
//用地三级分类
typedef struct
{
	int		Type_Id;      
	int		Super_Id;
	CString	Type_Code;
	CString	Type_Name;
	CString	Description;
}YDTYPECONFIG; 

typedef CArray<YDTYPECONFIG,YDTYPECONFIG&> YDTYPEARRAY;

class CYdxz3Part
{
public:
	         CYdxz3Part(void);
	virtual ~CYdxz3Part(void);

	static BOOL GetFromDataBase(const CString strURL);
	static void Release();

	static void GetRootNames(CStringArray& saRootName);
	static void GetParentNameByName(const CString sName, CString& sParentName);
	static void GetChildNameByName(const CString sName, CStringArray& saChildName);

	static void GetCodeByName(const CString sName, CString& sCode, const int sParentId = 0);
	static void GetNameByCode(const CString sCode, CString& sName);

	static int GetIdByName(const CString sName, const int sParentId = 0);


	static YDTYPEARRAY  m_YdTypeArray;
};



//===============================================================================================//
//SDE连接信息
typedef struct
{
	CString strServer;
	CString strInstance;
	CString strDatabase;
	CString strUserName;
	CString strPassword;
	CString strGDBOwner; 
}BASESDEINFO; 

typedef struct
{
	CString strServer;
	CString strUserName;
	CString strPassword;
	int nPort;
}BASEFTPINFO; 

//业务库连接信息
typedef struct
{
	CString strUserId;
	CString strUserName;
}BASEDAPINFO; 

//项目基本信息
typedef struct
{
	CString strXMBH;       //项目编号
	CString strGLBH;       //关联编号
	CString strTFBH;       //图幅编号
	CString strProcessId;

	//
	CString strLCH;        //流程号
	CString strZH;     //证号
	CString strRQ;     //审批日期
	CString strXMMC;   //项目名称
	CString strJSDW;   //建设单位
	CString strYDXZ;   //用地性质
	CString strLXLX;   //蓝线类型

	CString strJSDZ;   //建设地址
}BASEPROJECTINFO;




class CBaseCfgInfo
{
public:
	static BASESDEINFO              m_BaseSdeInfo;
	static BASEDAPINFO              m_BaseDapInfo;
	//static BASEFTPINFO              m_BaseFtpInfo;
	static DIST_PRJBASEINFO_STRUCT  m_basePrjInfo;

	static AcGePoint3dArray   m_FWpts;     //
	static AcGePoint3dArray   m_XMBHpts;   //项目编号对应的蓝线范围

	//函数名称：	ReadSdeInfo
	//函数功能：	通过WebService读取Sde数据库连接信息
	//				在系统启动的时候读取此类配置信息
	static BOOL ReadConnectInfo(CString strURL);
	//函数名称：	ReadSdeInfoByXML
	//函数功能：	通过XML str读取Sde数据库连接信息
	//				在系统启动的时候读取此类配置信息
	static BOOL ReadConnectInfoByXML(CString strXML);

	static BOOL ReadFtpInfo(CString strURL);

	//函数名称：	ReadProjectBaseInfo
	//函数功能：	读取TBPROJECT_BASE表中关于项目的基本信息
	//				在系统启动的时候读取此类项目信息
	static BOOL ReadProjectBaseInfo(CString strXMBH, CString strURL);

	//函数名称：	ReadUserInfo
	//函数功能：	读取TBUSER表中关于当前用户的信息
	//				在系统启动的时候读取此类用户信息
	static BOOL ReadUserInfo(CString strUserId, CString strURL);

	// 获取项目编号的点数组，存储在m_XMBHpts
	BOOL GetXMBHPoints(AcGePoint3dArray& ptArray);

	// 获取绘图图幅号范围的点数组，存储在m_FWpts
	BOOL GetHTFWPoints();

};


#endif//__ACCESS__WEBSERVICE__INFO__H__