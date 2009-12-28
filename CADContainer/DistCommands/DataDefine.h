#ifndef  _DATADEFINE_H_
#define  _DATADEFINE_H_

#include "StdAfx.h"

// CAD树结构配置表
struct STB_CONTENT_TREE
{
	long nId;
	long nOwnerId;
	CString strNodeName;
	CString strSdeLyName;
	CString strPosOrder;
	BOOL bIsLocalLayer;
	BOOL bIsReadBefore;

	STB_CONTENT_TREE()
	{
		nId = -1;
		nOwnerId = -1;
		strNodeName = "";
		strSdeLyName = "";
		strPosOrder = "";
		bIsLocalLayer = TRUE;
		bIsReadBefore = FALSE; // 是否读取过
	}
};

// SDE连接参数
struct ST_SDE_CONNECTION
{
	CString strServer;
	CString strService;
	CString strDatabase;
	CString strUserName;
	CString strPassword;
};

// FTP连接参数
struct ST_FTP_CONNECTION
{
	CString strServer;
	CString strUserName;
	CString strPassword;
	WORD	nPort;

	ST_FTP_CONNECTION()
	{
		strServer = "127.0.0.1";
		strUserName = "guest";
		strPassword = "";
		nPort = 21;
	}
};

// 业务信息
struct ST_BIZZINFO
{
	CString strBizzCode; // 项目编号
	CString strBizzSame; // 关联编号
	CString strBizzName; // 项目名称
	CString strBzOwner; // 建设单位
	CString strBzIdId; // 业务类型
	CString strBzId; // 业务名称
	CString strBztmOver; // 结束时间
	CString strCurTime; // 服务器当前时间

	CString strUserId; // 用户ID
	CString strLoginUserName; // 登录用户名
};

// 图层信息
struct ST_LAYERINFO
{
	// 标注配置信息
	CString strAnnotionLine; // 标注线层
	CString strAnnotionText; // 标注点层
	
	// 范围图层配置
	CString strLayer; // 范围图层名
	CString strField; // 项目编号字段名

	// 地形图图层配置信息
	CString strMapLayer; // 索引图层名
	CString strIndexField; // 索引字段名
};


struct POWER 
{
	long nPower; // 0：隐藏 1：只读 2：可写 3：必填
	long nPrior; // 值越大优先级越高
};

typedef map<long, long> MapLong2Long;

typedef map<CString, CString> MapString2String;

typedef map<long, POWER> MapLong2Power;

typedef map<CString, POWER> MapString2Power;

typedef CArray<STB_CONTENT_TREE, STB_CONTENT_TREE> ArrayLayerTree;

typedef map<CString, STB_CONTENT_TREE> MapString2ContentTree;

#endif