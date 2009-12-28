#pragma once
#include <map>
#include "StdAfx.h"

using namespace std;
class CConfigReader :
	public CObject
{
protected:
	void * _pConfigObject;
	CString _configFileName;	
public:
	CConfigReader(void);
	virtual ~CConfigReader(void);

	virtual void * ReadConfig();
};


class CJn_CommandPower: public CObject	//济南命令权限
{	
public:
	std::map<CString,BOOL> powerList;
	std::map<CString,CString> cmdDesc;
	CJn_CommandPower()
	{
		powerList["NEW"] = 0;		cmdDesc["NEW"] = "新建图形";
		powerList["QNEW"] = 0;		cmdDesc["QNEW"] = "快速新建";
		powerList["OPEN"] = 0;		cmdDesc["OPEN"] = "打开";	
		powerList["QSAVE"] = 0;		cmdDesc["QSAVE"] = "快速保存";
		powerList["SAVEAS"] = 0;	cmdDesc["SAVEAS"] = "另存为";
		powerList["EXPORT"] = 0;	cmdDesc["EXPORT"] = "导出";
		powerList["WBLOCK"] = 0;	cmdDesc["WBLOCK"] = "写块";
		powerList["COPY"] = 0;		cmdDesc["COPY"] = "复制";
		powerList["COPYCLIP"] = 0;	cmdDesc["COPYCLIP"] = "复制到剪贴板";
		powerList["COPYBASE"] = 0;	cmdDesc["COPYBASE"] = "带基点复制";
		powerList["CUTCLIP"] = 0;	cmdDesc["CUTCLIP"] = "剪切到剪贴板";
		powerList["APPLOAD"] = 0;	cmdDesc["APPLOAD"] = "加载应用程序";
		powerList["ARX"] = 0;		cmdDesc["ARX"] = "加载ObjectArx应用程序";
		powerList["VBAIDE"] = 0;	cmdDesc["VBAIDE"] = "VBA集成开发环境";
		powerList["VBARUN"] = 0;	cmdDesc["VBARUN"] = "VBA执行";
		powerList["VBALOAD"] = 0;	cmdDesc["VBALOAD"] = "VBA加载";
		powerList["VBAMAN"] = 0;	cmdDesc["VBAMAN"] = "VBA管理";
		powerList["SAVE"] = 0;		cmdDesc["SAVE"] = "保存";
		powerList["QUIT"] = 0;		cmdDesc["QUIT"] = "退出";

		// 自定义命令
		powerList["_SHOWDOCKTREEPANE"] = 0;		cmdDesc["_SHOWDOCKTREEPANE"] = "显示目录树";
		powerList["_READSDELAYERS"] = 0;		cmdDesc["_READSDELAYERS"] = "自动读取SDE图层数据";
		powerList["_READSDELAYERSEX"] = 0;		cmdDesc["_READSDELAYERSEX"] = "手工查询SDE图层数据";
		powerList["_DISTSELPROJECT"] = 0;		cmdDesc["_DISTSELPROJECT"] = "相关项目查询";
		powerList["_SHOWOBJECTATTR"] = 0;		cmdDesc["_SHOWOBJECTATTR"] = "查看编辑属性";
		powerList["_DRAWXMFWX"] = 0;			cmdDesc["_DRAWXMFWX"] = "项目范围入库";
		powerList["_DRAWXMHX"] = 0;				cmdDesc["_DRAWXMHX"] = "制作项目红线";
		powerList["_SAVESDELAYERS"] = 0;		cmdDesc["_SAVESDELAYERS"] = "保存SDE图层数据";
		powerList["_DRAWDLHX"] = 0;				cmdDesc["_DRAWDLHX"] = "道路红线";
		powerList["_DRAWCSLX"] = 0;				cmdDesc["_DRAWCSLX"] = "城市蓝线";
		powerList["_DRAWCSHX"] = 0;				cmdDesc["_DRAWCSHX"] = "城市黄线";
		powerList["_DRAWCSLVX"] = 0;			cmdDesc["_DRAWCSLVX"] = "城市绿线";
		powerList["_DRAWCSZX"] = 0;				cmdDesc["_DRAWCSZX"] = "城市紫线";
		powerList["_DRAWCSCX"] = 0;				cmdDesc["_DRAWCSCX"] = "城市橙线";
	}
	//CJn_CommandPower(){}
	CJn_CommandPower(const  CJn_CommandPower & src);
	void operator= (const  CJn_CommandPower & src);
	virtual ~CJn_CommandPower(){}
	virtual void Serialize(CArchive& archive);
protected:
	DECLARE_SERIAL(CJn_CommandPower)
};
class CCmdPwrConfigReader : public CConfigReader
{
protected:
	virtual void * ReadConfig();
	CCmdPwrConfigReader(void);	
public:
	static CCmdPwrConfigReader& Instanse();
	CJn_CommandPower *	GetCmdPwrConfig();
	virtual ~CCmdPwrConfigReader(void);	
};