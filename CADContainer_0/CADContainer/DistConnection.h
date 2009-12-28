#pragma once
#include "StdAfx.h"
#include <sdeerno.h>
#include <sdetype.h>
#include <sderaster.h>
#include <pe.h>
#include <pedef.h>
#include <pef.h>

#ifdef DIST_CONNECTION_EXPORTS
#define DIST_CONNECTION_API __declspec(dllexport)
#else
#define DIST_CONNECTION_API __declspec(dllimport)
#endif

//定义何种连接类型
typedef enum enC_KIND
{
	C_SDE	=  1,	//SDE
	C_MIF	=  2,	//MIF
	C_SHAPE	=  3,	//SHAPE
}C_KIND;

//============================== 导出函数 ===================================//

//根据错误编号返回错误提示
DIST_CONNECTION_API char* WINAPI ReturnErrorCodeInfo(long nErrorCode);

class CDistConnection
{
public:
	CDistConnection(void);
	~CDistConnection(void);

public:
	C_KIND m_enType;
	
	virtual long CheckConnected();

	// 功能：创建连接
	// 返回：成功返回 1，失败返回 0
	virtual long Connect();

	// 功能：断开连接
	// 返回：成功返回 1，失败返回 0
	virtual long Disconnect();
};

class CDistConnectSDE :
	public CDistConnection
{
public:
	CDistConnectSDE(void);
	~CDistConnectSDE(void);

private:
	//数据库连接对象指针
	SE_CONNECTION m_pConnectObj;
	char m_strDBName[SE_MAX_DATABASE_LEN];
	char m_strUserName[SE_MAX_OWNER_LEN];

public:

	// 功能：根据返回代码显示相应信息
	// 参数： rc          操作返回值
	//        szComment   调用函数名称
	// 返回：
	static void check_error (long rc, char *strComment);

	// 功能：获取数据库连接对象指针
	// 参数：无
	// 返回：返回对象指针
	SE_CONNECTION GetConnectObj(void);

	virtual long CheckConnected(int IntervalTime=60);

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
};

class CDistConnectMIF :
	public CDistConnection
{
public:
	CDistConnectMIF(void);
	~CDistConnectMIF(void);
};

class CDistConnectSHAPE :
	public CDistConnection
{
public:
	CDistConnectSHAPE(void);
	~CDistConnectSHAPE(void);
};
